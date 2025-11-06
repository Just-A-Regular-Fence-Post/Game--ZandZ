#pragma once
#include "Entity.h"

class Flame : public Entity {
public:
    Flame(int x_, int y_, bool facingRight);

    bool intersects(const Entity& other) const;
};
