#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <windows.h> // For console handling

// ---------------- Console Helpers ----------------
void resetCursor() {
    std::cout << "\x1b[H";
}

void clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, ' ', cells, {0, 0}, &written);
    SetConsoleCursorPosition(hConsole, {0, 0});
}

// ---------------- ASCII Sprites ----------------
std::vector<std::string> person = {
" O ",
"/|\\",
"/ \\"
};

std::vector<std::string> personThrow = {
" O ",
"-|\\",
"/ \\"
};

std::vector<std::string> personHit = {
" O ",
"/|\\",
"/X\\"
};

void drawSprite(const std::vector<std::string>& sprite, int x, int y, std::vector<char>& buffer, int WIDTH, int HEIGHT) {
    for (int sy = 0; sy < sprite.size(); sy++) {
        for (int sx = 0; sx < sprite[sy].size(); sx++) {
            char c = sprite[sy][sx];
            int bx = x + sx;
            int by = y + sy;
            if (c != ' ' && bx >= 0 && bx < WIDTH && by >= 0 && by < HEIGHT)
                buffer[by * WIDTH + bx] = c;
        }
    }
}

// ---------------- Main ----------------
int main() {
    const int WIDTH = 145;
    const int HEIGHT = 45;
    const int FRAMES = 30;

    std::vector<char> buffer(WIDTH * HEIGHT, ' ');

    int throwerX = 10;
    int throwerY = HEIGHT / 2;
    int targetX = WIDTH - 15;
    int targetY = HEIGHT / 2;

    // Precompute rock path for parabolic motion
    std::vector<int> rockX(FRAMES);
    std::vector<int> rockY(FRAMES);

    for (int i = 0; i < FRAMES; i++) {
        float t = i / static_cast<float>(FRAMES - 1); // 0..1
        rockX[i] = throwerX + static_cast<int>((targetX - throwerX) * t);
        // Parabolic height: peak at t=0.5
        float heightOffset = -15.0f * (4.0f * t * (1 - t)); // peak -15 lines
        rockY[i] = throwerY + static_cast<int>(heightOffset);
        if (rockY[i] < 0) rockY[i] = 0;
    }

    clearScreen();

    // Animate thrower and rock
    for (int frame = 0; frame < FRAMES; frame++) {
        std::fill(buffer.begin(), buffer.end(), ' ');

        // Thrower: animate windup for first few frames
        if (frame < 5)
            drawSprite(personThrow, throwerX, throwerY, buffer, WIDTH, HEIGHT);
        else
            drawSprite(person, throwerX, throwerY, buffer, WIDTH, HEIGHT);

        // Target: normal
        drawSprite(person, targetX, targetY, buffer, WIDTH, HEIGHT);

        // Draw rock
        buffer[rockY[frame] * WIDTH + rockX[frame]] = 'o';

        // Render buffer
        resetCursor();
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                std::cout << buffer[y * WIDTH + x];
            }
            std::cout << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Rock hits target: show reaction for 5 frames
    for (int frame = 0; frame < 5; frame++) {
        std::fill(buffer.begin(), buffer.end(), ' ');

        drawSprite(person, throwerX, throwerY, buffer, WIDTH, HEIGHT);
        drawSprite(personHit, targetX, targetY, buffer, WIDTH, HEIGHT);

        resetCursor();
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                std::cout << buffer[y * WIDTH + x];
            }
            std::cout << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "\nAnimation complete!\n";

    return 0;
}
