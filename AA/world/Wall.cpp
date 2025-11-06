#include "Wall.h"

void Wall::draw() const {
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            drawToBuffer(x + j, y + i, '█');
}
