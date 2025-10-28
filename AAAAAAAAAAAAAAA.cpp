#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <conio.h>
#include <windows.h>
#include <cstdlib>

int SCREEN_W, SCREEN_H;
std::vector<char> buffer;

// ==========================================================
// BASIC RENDER FUNCTIONS
// ==========================================================

void getConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    SCREEN_W = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    SCREEN_H = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    buffer.resize(SCREEN_W * SCREEN_H, ' ');
}

void clearBuffer() {
    std::fill(buffer.begin(), buffer.end(), ' ');
}

void drawToBuffer(int x, int y, char c) {
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H)
        buffer[y * SCREEN_W + x] = c;
}

void render() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hOut, pos);
    for (int y = 0; y < SCREEN_H; y++) {
        for (int x = 0; x < SCREEN_W; x++)
            std::cout << buffer[y * SCREEN_W + x];
        std::cout << '\n';
    }
}

// ==========================================================
// ASCII ART AND ANIMATION FUNCTIONS
// ==========================================================

std::vector<std::string> titleArt = {
    "  ____        _      _      _   _      _ _ ",
    " |  _ \ _   _| | ___| | __ | | | | __| | |",
    " | |_) | | | | |/ _ \ |/ / | |_| |/ _ \ | |",
    " |  __/| |_| | |  __/   <  |  _  |  __/ | |",
    " |_|    \__,_|_|\___|_|\_\ |_| |_|\___|_|_|",
};

void drawAsciiArt(int x, int y, const std::vector<std::string>& art) {
    for (int row = 0; row < (int)art.size(); row++) {
        for (int col = 0; col < (int)art[row].size(); col++) {
            drawToBuffer(x + col, y + row, art[row][col]);
        }
    }
}

void playStartAnimation() {
    clearBuffer();
    int targetY = SCREEN_H / 3;
    int startY = -10;
    int x = (SCREEN_W - (int)titleArt[0].size()) / 2;

    // Slide down
    for (int y = startY; y <= targetY; y++) {
        clearBuffer();
        drawAsciiArt(x, y, titleArt);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    // Bounce a bit
    for (int i = 0; i < 6; i++) {
        clearBuffer();
        drawAsciiArt(x, targetY + (i % 2 == 0 ? 1 : 0), titleArt);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void pressAnyKeyPrompt() {
    int blinkCounter = 0;
    int x = SCREEN_W / 2 - 6;
    int y = SCREEN_H / 2 + 4;

    while (true) {
        clearBuffer();
        drawAsciiArt((SCREEN_W - (int)titleArt[0].size()) / 2, SCREEN_H / 3, titleArt);

        if ((blinkCounter / 15) % 2 == 0) {
            std::string msg = "PRESS ANY KEY";
            for (int i = 0; i < (int)msg.size(); i++)
                drawToBuffer(x + i, y, msg[i]);
        }

        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
        blinkCounter++;

        if (_kbhit()) {
            _getch();
            break;
        }
    }
}

// ==========================================================
// MAIN
// ==========================================================

int main() {
    srand((unsigned int)time(nullptr));
    getConsoleSize();

    // Hide cursor
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);

    // Title animation
    playStartAnimation();
    pressAnyKeyPrompt();

    // Transition to main game
    clearBuffer();
    render();
    std::cout << "\n\nGame starting...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Placeholder main loop
    while (true) {
        // (Game code goes here)
        break;
    }

    return 0;
}
