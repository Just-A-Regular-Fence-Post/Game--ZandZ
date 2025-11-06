#include <iostream>
#include <vector>
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <windows.h> // Console size
#include <thread>    // sleep_for
#include <chrono>    // milliseconds

// ----------------------- Console Helpers -----------------------
int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return 80;
}

int getConsoleHeight() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    return 25;
}

// Clear console screen 
void clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, ' ', cells, {0, 0}, &written);
    SetConsoleCursorPosition(hConsole, {0, 0});
}

// ----------------------- Main -----------------------
int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int WIDTH = getConsoleWidth();
    int HEIGHT = getConsoleHeight();

    // Initialize a vector for the rain columns (each column has its y position)
    std::vector<int> columnY(WIDTH, 0);

    // Clear screen initially
    clearScreen();

    while (true) {
        // Build frame buffer
        std::vector<char> buffer(WIDTH * HEIGHT, ' ');

        // Update each column
        for (int x = 0; x < WIDTH; x++) {
            // Randomly decide if current column should drop a dot
            if (std::rand() % 2 == 0) {
                buffer[columnY[x] * WIDTH + x] = '.';
            }

            // Move dot down
            columnY[x]++;
            if (columnY[x] >= HEIGHT) {
                columnY[x] = 0; // reset to top
            }
        }

        // Move cursor to top-left
        std::cout << "\x1b[H";

        // Print buffer
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                std::cout << buffer[y * WIDTH + x];
            }
            std::cout << "\n";
        }

        // Sleep for ~30 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    return 0;
}
