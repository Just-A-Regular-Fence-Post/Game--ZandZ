#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <conio.h>          // for _kbhit, _getch
#include <windows.h>        // for GetConsoleScreenBufferInfo
#include <cstdlib>
#include <ctime>

struct Effect {
    enum Type { HLASER, VLASER, EXPLOSION, SPIKES } type;
    int x, y;
    int lifetime;       
    int duration;       
    bool active;
};

// Globals
int SCREEN_W = 80;
int SCREEN_H = 25;
std::vector<char> buffer;
std::vector<Effect> effects;

// --------------------------------------------------
void getConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    SCREEN_W = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    SCREEN_H = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    buffer.resize(SCREEN_W * SCREEN_H, ' ');
}

// --------------------------------------------------
void clearBuffer() {
    std::fill(buffer.begin(), buffer.end(), ' ');
}

void drawToBuffer(int x, int y, char c) {
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
        buffer[y * SCREEN_W + x] = c;
    }
}

void render() {
    COORD origin = {0, 0};
    DWORD written;
    WriteConsoleOutputCharacterA(
        GetStdHandle(STD_OUTPUT_HANDLE),
        buffer.data(),
        buffer.size(),
        origin,
        &written
    );
}

// --------------------------------------------------
void spawnEffect(Effect::Type t) {
    Effect e;
    e.type = t;
    e.active = true;
    e.duration = (30 * (1 + rand() % 4)); // 1–4 seconds
    e.lifetime = e.duration;

    switch (t) {
        case Effect::HLASER:
            e.x = 0;
            e.y = rand() % SCREEN_H;
            break;
        case Effect::VLASER:
            e.x = rand() % SCREEN_W;
            e.y = 0;
            break;
        case Effect::EXPLOSION:
            e.x = rand() % SCREEN_W;
            e.y = rand() % SCREEN_H;
            break;
        case Effect::SPIKES:
            e.x = rand() % SCREEN_W;
            e.y = SCREEN_H - 1;
            break;
    }
    effects.push_back(e);
}

// --------------------------------------------------
void updateEffects() {
    for (auto &e : effects) {
        if (!e.active) continue;

        e.lifetime--;
        if (e.lifetime <= 0) {
            e.active = false;
            continue;
        }

        switch (e.type) {
            case Effect::HLASER: {
                // advance horizontally over lifetime
                float progress = 1.0f - (float)e.lifetime / e.duration;
                int length = (int)(progress * SCREEN_W);
                for (int i = 0; i < length; i++) {
                    drawToBuffer(i, e.y, '-');
                }
            } break;

            case Effect::VLASER: {
                float progress = 1.0f - (float)e.lifetime / e.duration;
                int length = (int)(progress * SCREEN_H);
                for (int i = 0; i < length; i++) {
                    drawToBuffer(e.x, i, '|');
                }
            } break;

            case Effect::EXPLOSION: {
                // simple pulse effect
                float progress = 1.0f - (float)e.lifetime / e.duration;
                int radius = (int)(progress * 5);
                for (int dy = -radius; dy <= radius; dy++) {
                    for (int dx = -radius; dx <= radius; dx++) {
                        if (dx*dx + dy*dy <= radius*radius) {
                            drawToBuffer(e.x + dx, e.y + dy, '*');
                        }
                    }
                }
            } break;

            case Effect::SPIKES: {
                float progress = 1.0f - (float)e.lifetime / e.duration;
                int height = (int)(progress * 5);
                for (int h = 0; h < height; h++) {
                    drawToBuffer(e.x, e.y - h, '^');
                }
            } break;
        }
    }

    // Remove inactive
    std::vector<Effect> newList;
    for (auto &e : effects)
        if (e.active) newList.push_back(e);
    effects.swap(newList);
}

// --------------------------------------------------
void handleInput() {
    while (_kbhit()) {
        char c = _getch();
        switch (c) {
            case '1': spawnEffect(Effect::HLASER); break;
            case '2': spawnEffect(Effect::VLASER); break;
            case '3': spawnEffect(Effect::EXPLOSION); break;
            case '4': spawnEffect(Effect::SPIKES); break;
            case 27:  exit(0); // ESC to quit
        }
    }
}

// --------------------------------------------------
int main() {
    srand((unsigned int)time(nullptr));
    getConsoleSize();

    // Hide cursor
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);

    const int FPS = 30;
    const int frameTime = 1000 / FPS;

    while (true) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        handleInput();
        clearBuffer();
        updateEffects();
        render();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
        if (elapsed < frameTime)
            std::this_thread::sleep_for(std::chrono::milliseconds(frameTime - elapsed));
    }

    return 0;
}
