#include "Rendering.h"
#include "../engine/Console.h"

void drawHealthBar(const Player& player) {
    int barWidth = player.w;
    int filled = static_cast<int>((player.health / player.maxHealth) * barWidth);
    int barX = static_cast<int>(player.x);
    int barY = static_cast<int>(player.y) - 1;
    if (barY < 0) return;

    for (int i = 0; i < barWidth; i++)
        drawToBuffer(barX + i, barY, (i < filled) ? '#' : '-');
}

void drawBossHealthBar(const Boss& boss) {
    int barWidth = boss.w;
    int filled = static_cast<int>((boss.health / boss.maxHealth) * barWidth);
    int barX = static_cast<int>(boss.x);
    int barY = static_cast<int>(boss.y) - 1;
    if (barY < 0) return;

    for (int i = 0; i < barWidth; i++)
        drawToBuffer(barX + i, barY, (i < filled) ? '#' : '-');
}
