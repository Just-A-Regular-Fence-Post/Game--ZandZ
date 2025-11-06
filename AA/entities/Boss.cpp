#include "Boss.h"
#include "Player.h"
#include <cmath>

Boss::Boss(int x_, int y_)
    : Entity(x_, y_, bossSprite)
{
    facingRight = false;
}

void Boss::takeDamage(float dmg) {
    health -= dmg;
    if (health < 0) health = 0;
}

void Boss::chooseState(const Player& player) {
    float dx = player.x - x;

    if (fabs(dx) < 45)       state = BossState::ATTACK_FLAME;
    else if (fabs(dx) < 100) state = BossState::APPROACH;
    else                     state = BossState::IDLE;
}

void Boss::handleState(const Player& player) {
    switch (state) {
    case BossState::IDLE:
        vx = 0;
        break;

    case BossState::APPROACH:
        vx = (player.x > x) ? 1 : -1;
        facingRight = vx > 0;
        break;

    case BossState::RETREAT:
        vx = (player.x > x) ? -1 : 1;
        break;

    case BossState::ATTACK_FLAME:
        vx = 0;
        break;

    case BossState::STUNNED:
        vx = 0;
        break;
    }
}

void Boss::update(const std::vector<Platform>& platforms,
                  const std::vector<Wall>& walls,
                  const Player& player)
{
    chooseState(player);
    handleState(player);
    applyPhysics(platforms, walls);
}
