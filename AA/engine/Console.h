#pragma once
#include <vector>

extern int SCREEN_W;
extern int SCREEN_H;
extern std::vector<char> buffer;

void getConsoleSizeAndFixBuffer();
void clearBuffer();
void drawToBuffer(int x, int y, char c);
void render();
void showCursor(bool visible);
void resizer();
