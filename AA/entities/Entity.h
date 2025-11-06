#pragma once
#include <vector>
#include <string>
#include "Console.h"
#include "../world/Platform.h"
#include "../world/Wall.h"

class Entity {
public:
    float x, y;
    int vx = 0;
    float vy = 0;
    int w = 1, h = 1;
    bool onGround = false;
    bool facingRight = true;

    std::vector<std::string> sprite;

    Entity(int x_, int y_, const std::vector<std::string>& art);

    virtual ~Entity() = default;
    virtual void draw() const;
    virtual void update(const std::vector<Platform>& platforms,
                        const std::vector<Wall>& walls);

protected:
    void applyPhysics(const std::vector<Platform>& platforms,
                      const std::vector<Wall>& walls);
};
