#pragma once
#include "Console.h"

struct Platform {
    int x, y, w, h;
    Platform(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_) {}
    void draw() const;
};
