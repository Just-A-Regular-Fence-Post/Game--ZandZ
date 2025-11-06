#include "Platform.h"

void Platform::draw() const {
    for (int i = 0; i < w; i++)
        for (int j = 0; j < h; j++)
            drawToBuffer(x + i, y + j, '█');
}
