#include <iostream>
#include <vector>
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <windows.h> // Console size

// Get console width
int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return 80; // fallback
}

// Get console height
int getConsoleHeight() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    return 25; // fallback
}

int main() {
    // Seed random generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int WIDTH = getConsoleWidth();
    int HEIGHT = getConsoleHeight();

    // Create buffer
    std::vector<char> buffer(WIDTH * HEIGHT, ' ');

    // Fill buffer with random dots and spaces
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char c = (std::rand() % 2 == 0) ? '.' : ' ';
            buffer[y * WIDTH + x] = c;
        }
    }

    // Clear console
    std::cout << "\x1b[2J\x1b[H";

    // Print buffer
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            std::cout << buffer[y * WIDTH + x];
        }
        std::cout << "\n";
    }

    return 0;
}
