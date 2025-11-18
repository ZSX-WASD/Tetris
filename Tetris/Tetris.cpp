#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>

using namespace std;

enum class TetrominoType { I, O, T, S, Z, J, L };

struct Tetromino {
    TetrominoType type;
    int rotation;
    int x, y;
};

class Tetris {
private:
    static const int WIDTH = 10;
    static const int HEIGHT = 20;
    vector<vector<bool>> grid;
    Tetromino current;
    bool gameOver;
    int score;
    int level;
    int dropCounter;
    const int dropDelay = 30;

    vector<pair<int, int>> getBlocks() const {
        static const vector<vector<vector<pair<int, int>>>> shapes = {
            // I
            {
                {{0,1}, {1,1}, {2,1}, {3,1}},
                {{1,0}, {1,1}, {1,2}, {1,3}},
                {{0,1}, {1,1}, {2,1}, {3,1}},
                {{1,0}, {1,1}, {1,2}, {1,3}}
            },
            // O
            {
                {{0,0}, {0,1}, {1,0}, {1,1}},
                {{0,0}, {0,1}, {1,0}, {1,1}},
                {{0,0}, {0,1}, {1,0}, {1,1}},
                {{0,0}, {0,1}, {1,0}, {1,1}}
            },
            // T
            {
                {{0,1}, {1,0}, {1,1}, {2,1}},
                {{1,0}, {1,1}, {1,2}, {2,1}},
                {{0,1}, {1,1}, {2,1}, {1,2}},
                {{1,0}, {0,1}, {1,1}, {1,2}}
            },
            // S
            {
                {{1,0}, {2,0}, {0,1}, {1,1}},
                {{1,0}, {1,1}, {2,1}, {2,2}},
                {{1,1}, {2,1}, {0,2}, {1,2}},
                {{0,0}, {0,1}, {1,1}, {1,2}}
            },
            // Z
            {
                {{0,0}, {1,0}, {1,1}, {2,1}},
                {{1,0}, {0,1}, {1,1}, {0,2}},
                {{0,1}, {1,1}, {1,2}, {2,2}},
                {{2,0}, {1,1}, {2,1}, {1,2}}
            },
            // J
            {
                {{0,0}, {0,1}, {1,1}, {2,1}},
                {{1,0}, {2,0}, {1,1}, {1,2}},
                {{0,1}, {1,1}, {2,1}, {2,2}},
                {{1,0}, {1,1}, {1,2}, {0,2}}
            },
            // L
            {
                {{2,0}, {0,1}, {1,1}, {2,1}},
                {{1,0}, {1,1}, {1,2}, {2,2}},
                {{0,1}, {1,1}, {2,1}, {0,2}},
                {{0,0}, {1,0}, {1,1}, {1,2}}
            }
        };

        int typeIdx = static_cast<int>(current.type);
        int rotIdx = current.rotation % 4;
        const auto& shape = shapes[typeIdx][rotIdx];
        vector<pair<int, int>> blocks;
        for (const auto& offset : shape) {
            blocks.push_back({ current.x + offset.first, current.y + offset.second });
        }
        return blocks;
    }

    bool isValidPosition() const {
        auto blocks = getBlocks();
        for (const auto& block : blocks) {
            int x = block.first;
            int y = block.second;
            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
                return false;
            }
            if (grid[y][x]) {
                return false;
            }
        }
        return true;
    }

public:
    Tetris() {
        grid = vector<vector<bool>>(HEIGHT, vector<bool>(WIDTH, false));
        gameOver = false;
        score = 0;
        level = 1;
        dropCounter = 0;
        spawnNewTetromino();
    }

    void spawnNewTetromino() {
        current.type = static_cast<TetrominoType>(rand() % 7);
        current.rotation = 0;
        current.x = WIDTH / 2 - 2;
        current.y = 0;
        if (!isValidPosition()) {
            gameOver = true;
        }
    }

    void moveLeft() {
        current.x--;
        if (!isValidPosition()) current.x++;
    }

    void moveRight() {
        current.x++;
        if (!isValidPosition()) current.x--;
    }

    bool moveDown() {
        current.y++;
        if (!isValidPosition()) {
            current.y--;
            return false;
        }
        return true;
    }

    void rotate() {
        int prevRot = current.rotation;
        current.rotation = (current.rotation + 1) % 4;
        if (!isValidPosition()) {
            current.rotation = prevRot;
        }
    }

    void lockTetromino() {
        auto blocks = getBlocks();
        for (const auto& block : blocks) {
            if (block.second >= 0 && block.second < HEIGHT) {
                grid[block.second][block.first] = true;
            }
        }
    }

    void clearLines() {
        int linesCleared = 0;
        for (int y = HEIGHT - 1; y >= 0; y--) {
            bool full = true;
            for (int x = 0; x < WIDTH; x++) {
                if (!grid[y][x]) {
                    full = false;
                    break;
                }
            }
            if (full) {
                linesCleared++;
                for (int yy = y; yy > 0; yy--) {
                    for (int x = 0; x < WIDTH; x++) {
                        grid[yy][x] = grid[yy - 1][x];
                    }
                }
                for (int x = 0; x < WIDTH; x++) {
                    grid[0][x] = false;
                }
                y++;
            }
        }
        if (linesCleared > 0) {
            score += linesCleared * linesCleared * 100;
            level = score / 1000 + 1;
        }
    }

    void update() {
        dropCounter++;
        int currentDelay = dropDelay - level * 3;
        if (currentDelay < 5) currentDelay = 5;

        if (dropCounter >= currentDelay) {
            if (!moveDown()) {
                lockTetromino();
                clearLines();
                spawnNewTetromino();
            }
            dropCounter = 0;
        }
    }

    void draw() const {
        system("cls");
        cout << "TETRIS (Score: " << score << " Level: " << level << ")\n";
        cout << "------------------------------------------\n";

        vector<vector<char>> display(HEIGHT, vector<char>(WIDTH, ' '));
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (grid[y][x]) {
                    display[y][x] = static_cast<char>(219); // Символ блока
                }
            }
        }

        auto blocks = getBlocks();
        for (const auto& block : blocks) {
            if (block.second >= 0 && block.second < HEIGHT &&
                block.first >= 0 && block.first < WIDTH) {
                display[block.second][block.first] = static_cast<char>(219);
            }
        }

        for (int y = 0; y < HEIGHT; y++) {
            cout << "|";
            for (int x = 0; x < WIDTH; x++) {
                cout << display[y][x] << display[y][x]; // Удвоение для лучшего отображения
            }
            cout << "|" << endl;
        }
        cout << "------------------------------------------" << endl;
        cout << "Controls: A/D - move, W - rotate, S - drop, Q - quit" << endl;
    }

    bool isGameOver() const { return gameOver; }
    void quit() { gameOver = true; }
    int getScore() const { return score; }
    int getLevel() const { return level; }
};

int main() {
    srand(static_cast<unsigned int>(time(0)));
    Tetris game;
    DWORD lastTick = GetTickCount();

    while (!game.isGameOver()) {
        if (_kbhit()) {
            char key = _getch();
            switch (tolower(key)) {
            case 'a': game.moveLeft(); break;
            case 'd': game.moveRight(); break;
            case 's':
                while (game.moveDown()) {}
                break;
            case 'w': game.rotate(); break;
            case 'q': game.quit(); break;
            }
        }

        DWORD currentTick = GetTickCount();
        if (currentTick - lastTick >= 10) {
            game.update();
            lastTick = currentTick;
        }

        game.draw();
        Sleep(10);
    }

    system("cls");
    cout << "\n\n";
    cout << "          GAME OVER!\n";
    cout << "      Final Score: " << game.getScore() << "\n";
    cout << "      Final Level: " << game.getLevel() << "\n";
    cout << "\n\nPress any key to exit...";
    _getch();
    return 0;
}