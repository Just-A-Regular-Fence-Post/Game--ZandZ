#include "Flame.h"
#include "../art/Art.h"

Flame::Flame(int x_, int y_, bool facingRight)
    : Entity(x_, y_, facingRight ? flameRight : flameLeft) {}

bool Flame::intersects(const Entity& other) const {
    return !(x + w <= other.x || x >= other.x + other.w ||
             y + h <= other.y || y >= other.y + other.h);
}
