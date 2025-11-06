#pragma once
#include "Console.h"

struct Wall {
    int x, y, w, h;
    Wall(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_) {}
    void draw() const;
};
