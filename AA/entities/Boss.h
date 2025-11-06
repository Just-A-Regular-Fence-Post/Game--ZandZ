#pragma once
#include "Entity.h"
#include "Flame.h"
#include "../art/Art.h"
#include <vector>

enum class BossState {
    IDLE,
    APPROACH,
    RETREAT,
    ATTACK_FLAME,
    STUNNED
};

class Boss : public Entity {
public:
    BossState state = BossState::IDLE;
    float maxHealth = 500;
    float health = 500;

    Boss(int x_, int y_);

    void update(const std::vector<Platform>& platforms,
                const std::vector<Wall>& walls,
                const class Player& player);

    void takeDamage(float dmg);

private:
    float stateTimer = 0;
    void chooseState(const class Player& player);
    void handleState(const class Player& player);
};
