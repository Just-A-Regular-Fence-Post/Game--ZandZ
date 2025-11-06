#define NOMINMAX
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <algorithm>
#include <memory>

// Engine & world
#include "engine/Console.h"
#include "engine/Rendering.h"
#include "world/Platform.h"
#include "world/Wall.h"

// Entities
#include "entities/Player.h"
#include "entities/Boss.h"
#include "entities/Cliff.h"

// Art
#include "art/Art.h"

// Controls
#include "Controls.h"

// ====================================================
// Animation functions (kept here)
// ====================================================
void playStartAnimation();
void playEndAnimation();
void pressAnyKeyPrompt();
void slideBlackBars(int speed = 33);
void slideBlackBarsOpen(int speed = 33);
void shifty(int duration, int speed = 20);

void runPackage() {
    playStartAnimation();
    pressAnyKeyPrompt();
    slideBlackBars(20);
    slideBlackBarsOpen(20);
}

void runEndPackage() {
    slideBlackBars(20);
    slideBlackBarsOpen(20);
    shifty(7, 80);
}

// ====================================================
// Main
// ====================================================
int main() {
    std::this_thread::sleep_for(std::chrono::seconds(4));
    srand(static_cast<unsigned int>(time(nullptr)));

    getConsoleSizeAndFixBuffer();
    showCursor(false);

    // Query controls
    std::cout << "=== Control Setup ===\n";
    Controls userControls = queryUserControls();
    std::cout << "\nControls set! Starting game...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    clearBuffer();
    render();

    // Create player and boss
    Player player(10, SCREEN_H - 5, userControls);
    Boss boss(SCREEN_W / 2, SCREEN_H - 10);

    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();
    int frames = 0;
    float fps = 0.0f;

    // Platforms
    std::vector<Platform> platforms = {
        {0, SCREEN_H, SCREEN_W, 1},
        {0, 1, SCREEN_W, 1},
        {SCREEN_W / 5, (SCREEN_H * 4) / 5, 15, 1},
        {(SCREEN_W * 4) / 5, (SCREEN_H * 4) / 5, 15, 1},
        {SCREEN_W / 3, SCREEN_H / 2, SCREEN_W / 3, 2},
    };

    // Walls
    std::vector<Wall> walls = {
        {0, 1, 1, SCREEN_H - 1},
        {SCREEN_W - 1, 1, 1, SCREEN_H - 1},
    };

    // Cliffs
    std::vector<Cliff> cliffs = {
        {0, SCREEN_H - 10},
        {SCREEN_W - 6, SCREEN_H - 12}
    };

    // Run intro animations
    runPackage();

    while (true) {
        resizer();
        clearBuffer();

        // Update entities
        player.update(platforms, walls);
        boss.update(platforms, walls, player);

        // Draw world
        for (auto& p : platforms) p.draw();
        for (auto& w : walls) w.draw();
        for (auto& c : cliffs) c.draw();

        // Draw entities
        player.draw();
        drawHealthBar(player);

        boss.draw();
        drawBossHealthBar(boss);

        // Player flamethrower
        if (player.flamethrowerOn && player.flame) {
            player.flame->draw();
            if (player.flame->intersects(boss)) boss.takeDamage(1);
        }

        // Direct collision damage
        if (!(player.x + player.w <= boss.x || player.x >= boss.x + boss.w ||
              player.y + player.h <= boss.y || player.y >= boss.y + boss.h)) {
            player.health -= 1.1f;
            if (player.health < 0) player.health = 0;
        }

        // Victory / defeat
        if (boss.health <= 0 || player.health <= 0) {
            clearBuffer();
            runEndPackage();
            render();
            std::this_thread::sleep_for(std::chrono::seconds(3));
            break;
        }

        // FPS display
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
        for (size_t i = 0; i < fpsText.size() && i < SCREEN_W; i++)
            drawToBuffer(static_cast<int>(i), 0, fpsText[i]);

        // Render frame
        render();

        // FPS calculation
        frames++;
        auto currentTime = clock::now();
        float elapsed = std::chrono::duration<float>(currentTime - lastTime).count();
        if (elapsed >= 1.0f) {
            fps = frames / elapsed;
            frames = 0;
            lastTime = currentTime;
        }

        // Frame delay
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
}
