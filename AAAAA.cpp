#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <conio.h> // Windows only for _kbhit() and _getch()
#include <windows.h>

using namespace std;
using namespace std::chrono;

// --- Console Size ---
int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

int getConsoleHeight() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

// --- Frame Buffer ---
vector<char> buffer;
int WIDTH, HEIGHT;

void clearBuffer() {
    fill(buffer.begin(), buffer.end(), ' ');
}

void renderBuffer() {
    cout << "\x1b[H"; // ANSI: move cursor top-left
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++)
            cout << buffer[y * WIDTH + x];
        cout << '\n';
    }
}

inline void setPixel(int x, int y, char c) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        buffer[y * WIDTH + x] = c;
}

// --- Sprites ---
vector<string> playerSprite = {
    " /\\ ",
    "/__\\",
    " || "
};

struct Bullet {
    float x, y;
    float vy;
    char symbol;
    bool active;
};

vector<Bullet> bullets;

// --- Draw sprite ---
void drawSprite(const vector<string>& sprite, int px, int py) {
    for (int y = 0; y < sprite.size(); y++) {
        for (int x = 0; x < sprite[y].size(); x++) {
            char c = sprite[y][x];
            if (c != ' ')
                setPixel(px + x, py + y, c);
        }
    }
}

// --- Player ---
int playerX, playerY;

// --- Game Loop ---
int main() {
    WIDTH = getConsoleWidth();
    HEIGHT = getConsoleHeight();
    buffer.resize(WIDTH * HEIGHT, ' ');

    playerX = WIDTH / 2;
    playerY = HEIGHT - 5;

    auto lastTime = high_resolution_clock::now();
    int bulletCooldown = 0;

    cout << "\x1b[2J"; // Clear screen

    while (true) {
        // --- Delta Time ---
        auto now = high_resolution_clock::now();
        float dt = duration<float>(now - lastTime).count();
        lastTime = now;

        // --- Input ---
        if (_kbhit()) {
            char ch = _getch();
            switch (ch) {
                case 72: playerY = max(0, playerY - 1); break; // Up arrow
                case 80: playerY = min(HEIGHT - 3, playerY + 1); break; // Down
                case 75: playerX = max(0, playerX - 1); break; // Left
                case 77: playerX = min(WIDTH - 4, playerX + 1); break; // Right
                case ' ': // Shoot
                    if (bulletCooldown <= 0) {
                        bullets.push_back({playerX + 1, playerY - 1, -30.0f, '*', true});
                        bulletCooldown = 5; // frames between shots
                    }
                    break;
            }
        }

        if (bulletCooldown > 0) bulletCooldown--;

        // --- Update bullets ---
        for (auto &b : bullets) {
            if (!b.active) continue;
            b.y += b.vy * dt;
            if (b.y < 0) b.active = false;
        }

        // --- Clear & draw ---
        clearBuffer();
        drawSprite(playerSprite, playerX, playerY);

        for (auto &b : bullets)
            if (b.active)
                setPixel((int)b.x, (int)b.y, b.symbol);

        renderBuffer();

        // --- Frame timing (~30 FPS) ---
        this_thread::sleep_for(milliseconds(33));
    }
}
