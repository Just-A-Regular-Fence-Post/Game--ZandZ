#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <chrono>
#include <conio.h>
#include <windows.h>

#define SCREEN_W 100
#define SCREEN_H 40

// ----------------------------------------------------------
// Buffer-based rendering setup
// ----------------------------------------------------------
CHAR_INFO buffer[SCREEN_H][SCREEN_W];
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

void clearBuffer() {
    for (int y = 0; y < SCREEN_H; ++y)
        for (int x = 0; x < SCREEN_W; ++x) {
            buffer[y][x].Char.AsciiChar = ' ';
            buffer[y][x].Attributes = 7;
        }
}

void drawToBuffer(int x, int y, char c, WORD color = 7) {
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
        buffer[y][x].Char.AsciiChar = c;
        buffer[y][x].Attributes = color;
    }
}

void render() {
    COORD size = { SCREEN_W, SCREEN_H };
    COORD zero = { 0, 0 };
    SMALL_RECT rect = { 0, 0, SCREEN_W - 1, SCREEN_H - 1 };
    WriteConsoleOutputA(hOut, (CHAR_INFO*)buffer, size, zero, &rect);
}

// ----------------------------------------------------------
// Skill Art
// ----------------------------------------------------------
std::map<std::string, std::vector<std::string>> skillArtWheel;

void initSkillArtWheel() {
    skillArtWheel["Quickstep"] = { " AAAAA " };
    skillArtWheel["Burst"] =    { " BBBBB " };
    skillArtWheel["Firebolt"] = { " CCCCC " };
    skillArtWheel["Mend"] =     { " DDDDD " };
    skillArtWheel["Shield"] =   { " EEEEE " };
    skillArtWheel["Dash"] =     { " FFFFF " };
    skillArtWheel["Hover"] =    { " GGGGG " };
}

// ----------------------------------------------------------
// Drawing
// ----------------------------------------------------------
void drawAsciiArt(int x, int y, const std::vector<std::string>& art, WORD color = 7) {
    for (int row = 0; row < (int)art.size(); ++row)
        for (int col = 0; col < (int)art[row].size(); ++col)
            drawToBuffer(x + col, y + row, art[row][col], color);
}

int getArtWidth(const std::vector<std::string>& art) {
    int w = 0;
    for (auto& line : art)
        if ((int)line.size() > w) w = line.size();
    return w;
}

// ----------------------------------------------------------
// 3-Item Wheel with Smooth Scrolling
// ----------------------------------------------------------
void drawSkillWheel3Visible(
    const std::vector<std::string>& skillNames,
    float selectedIndexF,   // float for smooth scroll
    bool isSelected
) {
    clearBuffer();
    int centerY = SCREEN_H / 2;
    int total = (int)skillNames.size();

    int centerIndex = (int)selectedIndexF;
    float offset = selectedIndexF - centerIndex; // fractional offset

    // Get indices of visible skills
    int above = (centerIndex - 1 + total) % total;
    int below = (centerIndex + 1) % total;
    std::vector<int> indices = { above, centerIndex, below };

    for (int pos = 0; pos < 3; ++pos) {
        int i = indices[pos];
        auto& art = skillArtWheel[skillNames[i]];
        int artW = getArtWidth(art);
        int x = (SCREEN_W - artW) / 2;

        int y;
        if (pos == 0) y = centerY - 6 - (int)(offset * 6); // above
        else if (pos == 1) y = centerY - (int)(offset * 6); // center
        else y = centerY + 6 - (int)(offset * 6);          // below

        WORD color = (i == centerIndex && isSelected) ? 10 : 7;
        drawAsciiArt(x, y, art, color);
    }

    render();
}

int skillWheelSelector3Visible(const std::vector<std::string>& skillNames) {
    int selected = 0;
    bool isSelected = false;

    while (true) {
        float selectedIndexF = (float)selected;
        drawSkillWheel3Visible(skillNames, selectedIndexF, isSelected);

        int key = _getch();
        if (key == 224) { // arrow keys
            key = _getch();
            int next = selected;
            if (key == 72) next = (selected - 1 + skillNames.size()) % skillNames.size();
            if (key == 80) next = (selected + 1) % skillNames.size();

            // Smooth scroll animation
            int steps = 6;
            float delta = (next - selected);
            if (delta < -1) delta += skillNames.size(); // wrap up
            if (delta > 1) delta -= skillNames.size();  // wrap down

            for (int s = 1; s <= steps; ++s) {
                float interp = selected + delta * (s / (float)steps);
                drawSkillWheel3Visible(skillNames, interp, isSelected);
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
            }

            selected = next;
        } else if (key == 's' || key == 'S') {
            isSelected = !isSelected;
        } else if (key == '\r') {
            return selected;
        }
    }
}

// ----------------------------------------------------------
// MAIN
// ----------------------------------------------------------
int main() {
    SetConsoleOutputCP(437);
    SetConsoleTitleA("Skill Wheel Selector Test");

    // Set console buffer & window
    SMALL_RECT windowSize = { 0, 0, SCREEN_W - 1, SCREEN_H - 1 };
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
    COORD bufferSize = { SCREEN_W, SCREEN_H };
    SetConsoleScreenBufferSize(hOut, bufferSize);

    initSkillArtWheel();

    std::vector<std::string> skills = {
        "Quickstep", "Burst", "Firebolt", "Mend",
        "Shield", "Dash", "Hover"
    };

    int selectedIndex = skillWheelSelector3Visible(skills);

    system("cls");
    std::cout << "\nSelected Skill: " << skills[selectedIndex] << "\n\n";
}
