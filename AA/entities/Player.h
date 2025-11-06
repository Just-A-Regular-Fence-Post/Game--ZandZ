#pragma once
#include "Entity.h"
#include "Flame.h"
#include "../art/Art.h"
#include "../Controls.h"
#include <memory>

class Player : public Entity {
public:
    Controls controls;
    bool flamethrowerOn = false;
    std::unique_ptr<Flame> flame;

    float flamethrowerCooldown = 0.0f;
    float cooldownDecay = 0.4f;

    float maxHealth = 100;
    float health = 100;

    Player(int x_, int y_, Controls controls);

    void handleInput();
    void attack();
    void update(const std::vector<Platform>& platforms,
                const std::vector<Wall>& walls) override;
};
