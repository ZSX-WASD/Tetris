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

// Цвета для консоли Windows
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

int getTetrominoColor(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return 11; // Cyan (голубой)
        case TetrominoType::O: return 14; // Yellow (желтый)
        case TetrominoType::T: return 13; // Magenta (фиолетовый)
        case TetrominoType::S: return 10; // Green (зеленый)
        case TetrominoType::Z: return 12; // Red (красный)
        case TetrominoType::J: return 9;  // Blue (синий)
        case TetrominoType::L: return 6;  // Orange (оранжевый)
        default: return 7;
    }
}

class Tetris {
private:
    static const int WIDTH = 10;
    static const int HEIGHT = 20;
    vector<vector<int>> grid; // Теперь хранит цвет блока (0 = пусто)
    Tetromino current;
    Tetromino nextTetromino;
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
        grid = vector<vector<int>>(HEIGHT, vector<int>(WIDTH, 0));
        gameOver = false;
        score = 0;
        level = 1;
        dropCounter = 0;
        generateNextTetromino();
        spawnNewTetromino();
    }

    void generateNextTetromino() {
        nextTetromino.type = static_cast<TetrominoType>(rand() % 7);
        nextTetromino.rotation = 0;
        nextTetromino.x = 0;
        nextTetromino.y = 0;
    }

    void spawnNewTetromino() {
        current = nextTetromino;
        current.x = WIDTH / 2 - 2;
        current.y = 0;
        generateNextTetromino();
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
        int color = getTetrominoColor(current.type);
        for (const auto& block : blocks) {
            if (block.second >= 0 && block.second < HEIGHT) {
                grid[block.second][block.first] = color;
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
                    grid[0][x] = 0;
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

    void drawNextPiece() const {
        setColor(15); // Белый
        cout << "  NEXT:";
        setColor(7);
        cout << "\n  ┌────┐\n";
        
        // Получаем блоки следующей фигуры
        static const vector<vector<vector<pair<int, int>>>> shapes = {
            {{{0,1}, {1,1}, {2,1}, {3,1}}}, // I
            {{{0,0}, {0,1}, {1,0}, {1,1}}}, // O
            {{{0,1}, {1,0}, {1,1}, {2,1}}}, // T
            {{{1,0}, {2,0}, {0,1}, {1,1}}}, // S
            {{{0,0}, {1,0}, {1,1}, {2,1}}}, // Z
            {{{0,0}, {0,1}, {1,1}, {2,1}}}, // J
            {{{2,0}, {0,1}, {1,1}, {2,1}}}  // L
        };
        
        int typeIdx = static_cast<int>(nextTetromino.type);
        const auto& shape = shapes[typeIdx][0];
        
        for (int y = 0; y < 2; y++) {
            cout << "  │";
            for (int x = 0; x < 4; x++) {
                bool found = false;
                for (const auto& block : shape) {
                    if (block.first == x && block.second == y) {
                        found = true;
                        break;
                    }
                }
                if (found) {
                    setColor(getTetrominoColor(nextTetromino.type));
                    cout << "█";
                    setColor(7);
                } else {
                    cout << " ";
                }
            }
            cout << "│\n";
        }
        cout << "  └────┘\n";
    }

    void draw() const {
        system("cls");
        
        // Заголовок
        setColor(15); // Яркий белый
        cout << "\n  ╔════════════════════════════╗\n";
        cout << "  ║       T E T R I S          ║\n";
        cout << "  ╚════════════════════════════╝\n\n";
        
        setColor(14); // Желтый
        cout << "  Score: " << score << "  Level: " << level << "\n\n";
        setColor(7);

        vector<vector<int>> display(HEIGHT, vector<int>(WIDTH, 0));
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                display[y][x] = grid[y][x];
            }
        }

        auto blocks = getBlocks();
        int currentColor = getTetrominoColor(current.type);
        for (const auto& block : blocks) {
            if (block.second >= 0 && block.second < HEIGHT &&
                block.first >= 0 && block.first < WIDTH) {
                display[block.second][block.first] = currentColor;
            }
        }

        // Рисуем игровое поле
        cout << "  ╔";
        for (int x = 0; x < WIDTH; x++) cout << "══";
        cout << "╗\n";

        for (int y = 0; y < HEIGHT; y++) {
            cout << "  ║";
            for (int x = 0; x < WIDTH; x++) {
                if (display[y][x] != 0) {
                    setColor(display[y][x]);
                    cout << "██";
                    setColor(7);
                } else {
                    cout << "  ";
                }
            }
            cout << "║";
            
            // Боковая панель с информацией
            if (y == 2) {
                drawNextPiece();
            } else if (y == 7) {
                setColor(11);
                cout << "  ┌──────────┐";
            } else if (y == 8) {
                cout << "  │ CONTROLS │";
            } else if (y == 9) {
                cout << "  ├──────────┤";
                setColor(7);
            } else if (y == 10) {
                cout << "  │ A - Left │";
            } else if (y == 11) {
                cout << "  │ D - Right│";
            } else if (y == 12) {
                cout << "  │ W - Rotate";
            } else if (y == 13) {
                cout << "  │ S - Drop │";
            } else if (y == 14) {
                cout << "  │ Q - Quit │";
            } else if (y == 15) {
                cout << "  └──────────┘";
            }
            cout << "\n";
        }

        cout << "  ╚";
        for (int x = 0; x < WIDTH; x++) cout << "══";
        cout << "╝\n";
        
        setColor(7); // Возврат к стандартному цвету
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
    setColor(12); // Красный
    cout << "\n\n";
    cout << "  ╔════════════════════════════╗\n";
    cout << "  ║       GAME OVER!           ║\n";
    cout << "  ╚════════════════════════════╝\n\n";
    setColor(14); // Желтый
    cout << "      Final Score: " << game.getScore() << "\n";
    cout << "      Final Level: " << game.getLevel() << "\n";
    setColor(7);
    cout << "\n\n  Press any key to exit...";
    _getch();
    return 0;
}