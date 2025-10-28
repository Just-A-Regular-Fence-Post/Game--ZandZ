#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <windows.h>
#include <conio.h> 

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

int main()
{
    int width = getConsoleWidth();
    int height = getConsoleHeight();
    std::cout << "Console Width: " << width << "\n";
    std::cout << "Console Height: " << height << "\n";
    return 0;
}

