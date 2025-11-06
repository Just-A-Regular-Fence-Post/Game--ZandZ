#include "Player.h"
#include <Windows.h>

Player::Player(int x_, int y_, Controls c)
    : Entity(x_, y_, playerSprite), controls(c) {}

void Player::handleInput() {
    vx = 0;

    if (GetAsyncKeyState(controls.moveLeft) & 0x8000) {
        vx = -3;
        facingRight = false;
    }
    if (GetAsyncKeyState(controls.moveRight) & 0x8000) {
        vx = 3;
        facingRight = true;
    }
    if (GetAsyncKeyState(controls.jump) & 0x8000 && onGround)
        vy = -9;
}

void Player::attack() {
    bool keyPressed = GetAsyncKeyState(controls.attack) & 0x8000;

    if (keyPressed && flamethrowerCooldown <= 0.0f) {
        flamethrowerOn = true;

        if (!flame)
            flame = std::make_unique<Flame>(x + (facingRight ? w : -6), y, facingRight);
        else {
            flame->x = x + (facingRight ? w : -6);
            flame->y = y;
            flame->sprite = facingRight ? flameRight : flameLeft;
        }

        flamethrowerCooldown = 0.2f;
    } 
    else if (!keyPressed) {
        flamethrowerOn = false;
        flame.reset();
    }
}

void Player::update(const std::vector<Platform>& platforms,
                    const std::vector<Wall>& walls)
{
    flamethrowerCooldown -= cooldownDecay;
    if (flamethrowerCooldown < 0.0f) flamethrowerCooldown = 0.0f;

    handleInput();
    attack();

    sprite = facingRight ? playerSprite : playerSpriteOtherWay;

    applyPhysics(platforms, walls);
}
