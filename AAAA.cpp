#define NOMINMAX
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <conio.h>
#include <windows.h>
#include <cstdlib>


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



int main()
{
    

}
