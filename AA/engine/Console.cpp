#include "Console.h"
#include <windows.h>
#include <algorithm>
#include <string>

int SCREEN_W = 80;
int SCREEN_H = 25;
std::vector<char> buffer;

void getConsoleSizeAndFixBuffer() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
        SCREEN_W = 80;
        SCREEN_H = 25;
        buffer.resize(SCREEN_W * SCREEN_H, ' ');
        return;
    }

    SHORT winW = csbi.srWindow.Right - csbi.srWindow.Left;
    SHORT winH = csbi.srWindow.Bottom - csbi.srWindow.Top;

    SCREEN_W = winW;
    SCREEN_H = winH;
    buffer.assign(SCREEN_W * SCREEN_H, ' ');
}

void clearBuffer() {
    std::fill(buffer.begin(), buffer.end(), ' ');
}

void drawToBuffer(int x, int y, char c) {
    if (x>=0 && x<SCREEN_W && y>=0 && y<SCREEN_H)
        buffer[y * SCREEN_W + x] = c;
}

void render() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0,0};
    SetConsoleCursorPosition(hOut, pos);

    std::string out;
    out.reserve((SCREEN_W+1)*SCREEN_H);

    for(int y=0; y<SCREEN_H; y++){
        out.append(&buffer[y*SCREEN_W], SCREEN_W);
        out.push_back('\n');
    }

    DWORD w;
    WriteConsoleA(hOut, out.c_str(), (DWORD)out.size(), &w, nullptr);
}

void showCursor(bool visible) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(h, &ci);
    ci.bVisible = visible;
    SetConsoleCursorInfo(h, &ci);
}

void resizer() {
    static int frameCount = 0;
    static int lastW = SCREEN_W, lastH = SCREEN_H;

    if (++frameCount % 20 != 0) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        int winW = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int winH = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        if (winW != lastW || winH != lastH) {
            lastW = winW;
            lastH = winH;
            SCREEN_W = winW;
            SCREEN_H = winH;
            buffer.assign(SCREEN_W * SCREEN_H, ' ');
        }
    }
}
