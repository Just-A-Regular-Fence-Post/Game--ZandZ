#pragma once
#include <string>

struct Controls {
    int moveLeft;
    int moveRight;
    int jump;
    int attack;
};

// Maps _getch input to WinAPI VK_ codes
int mapKeyForAsyncKey(int key);

// Prompts the user for a key binding
int askAnyKey(const std::string& action);

// Query all controls from the user
Controls queryUserControls();
