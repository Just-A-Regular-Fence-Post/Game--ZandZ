#include "Entity.h"
#include <algorithm>

Entity::Entity(int x_, int y_, const std::vector<std::string>& art)
    : x(x_), y(y_), sprite(art)
{
    h = (int)sprite.size();
    w = (h > 0) ? (int)sprite[0].size() : 1;
}

void Entity::draw() const {
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            if (sprite[i][j] != ' ')
                drawToBuffer(x + j, y + i, sprite[i][j]);
}

void Entity::applyPhysics(const std::vector<Platform>& platforms,
                          const std::vector<Wall>& walls)
{
    vy += 2.0f;
    if (vy > 2.9f) vy = 2.9f;
    onGround = false;

    float nextY = y + vy;
    y = nextY;

    // Platform collisions
    for (auto& p : platforms) {
        bool xOverlap = (x + w > p.x && x < p.x + p.w);
        if (xOverlap && vy >= 0 && y + h > p.y && y + h <= p.y + 3) {
            y = p.y - h;
            vy = 0;
            onGround = true;
        }
        if (xOverlap && vy < 0 && y < p.y + p.h && y + h > p.y) {
            y = p.y + p.h;
            vy = 0;
        }
    }

    // Wall vertical collisions
    for (auto& wall : walls) {
        if (x + w > wall.x && x < wall.x + wall.w &&
            y + h > wall.y && y < wall.y + wall.h)
        {
            if (vy > 0) { y = wall.y - h; onGround = true; }
            else if (vy < 0) y = wall.y + wall.h;
            vy = 0;
        }
    }

    float nextX = x + vx;
    x = nextX;

    for (auto& p : platforms) {
        bool yOverlap = (y + h > p.y && y < p.y + p.h);
        if (yOverlap) {
            if (vx > 0 && x + w > p.x && x < p.x)
                x = p.x - w, vx = 0;
            else if (vx < 0 && x < p.x + p.w && x + w > p.x + p.w)
                x = p.x + p.w, vx = 0;
        }
    }

    for (auto& wall : walls) {
        if (x + w > wall.x && x < wall.x + wall.w &&
            y + h > wall.y && y < wall.y + wall.h)
        {
            if (vx > 0) x = wall.x - w;
            else if (vx < 0) x = wall.x + wall.w;
            vx = 0;
        }
    }

    if (y > SCREEN_H - h) { y = SCREEN_H - h; vy = 0; onGround = true; }
    if (x < 0) x = 0;
    if (x >= SCREEN_W - w) x = SCREEN_W - w;
}

void Entity::update(const std::vector<Platform>& platforms,
                    const std::vector<Wall>& walls)
{
    applyPhysics(platforms, walls);
}
