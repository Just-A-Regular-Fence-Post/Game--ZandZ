#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <conio.h>
#include <windows.h>
#include <cmath>


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

void resetCursor() {
    std::cout << "\x1b[H";
}

void printBuffer(const std::vector<char>& buffer, int WIDTH, int HEIGHT) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            std::cout << buffer[y * WIDTH + x];
        }
        std::cout << "\n";
    }
}

class Player {
    public:

}

class Boss {

}

class Ability {
    public:
    std::string name;
    int cooldown;
    int duration;
    int currentCooldown;
    int currentDuration;
    int damage;
    int range;
    bool selected = false;
}




int main() {
    int WIDTH = getConsoleWidth();
    int HEIGHT = getConsoleHeight();
    std::vector<char> buffer(WIDTH * HEIGHT, ' ');

    // Clear console initially
    std::cout << "\x1b[2J";

    while (true) {
        // Fill buffer with random dots and spaces
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                buffer[y * WIDTH + x] = (std::rand() % 2 == 0) ? '.' : ' ';
            }
        }

        // Move cursor to top-left
        resetCursor();

        // Print buffer
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                std::cout << buffer[y * WIDTH + x];
            }
            std::cout << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    return 0;
}

