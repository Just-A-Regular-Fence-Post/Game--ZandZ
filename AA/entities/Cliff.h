#pragma once
#include "Entity.h"
#include "../art/Art.h"

class Cliff : public Entity {
public:
    Cliff(int x_, int y_) : Entity(x_, y_, cliffSprite) {}
};
