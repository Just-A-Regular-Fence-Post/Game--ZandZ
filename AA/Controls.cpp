#include "Controls.h"
#include <conio.h>
#include <Windows.h>
#include <iostream>

int mapKeyForAsyncKey(int key) {
    if (key == 224) { // special key prefix
        int arrow = _getch();
        switch (arrow) {
        case 72: return VK_UP;
        case 75: return VK_LEFT;
        case 77: return VK_RIGHT;
        case 80: return VK_DOWN;
        }
    }
    return key;
}

int askAnyKey(const std::string& action) {
    std::cout << "Press a key for " << action << ": ";
    int key = _getch();
    int mappedKey = mapKeyForAsyncKey(key);

    if (mappedKey < 256) std::cout << (char)mappedKey << "\n";
    else std::cout << "(Arrow key)\n";

    return mappedKey;
}

Controls queryUserControls() {
    Controls c;
    c.moveLeft  = askAnyKey("Move Left");
    c.moveRight = askAnyKey("Move Right");
    c.jump      = askAnyKey("Jump");
    c.attack    = askAnyKey("Attack");
    return c;
}
