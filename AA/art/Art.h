#pragma once
#include <vector>
#include <string>

extern std::vector<std::string> titleArt;
extern std::vector<std::string> endArt;

extern std::vector<std::string> playerSprite;
extern std::vector<std::string> playerSpriteOtherWay;

extern std::vector<std::string> bossSprite;

extern std::vector<std::string> flameRight;
extern std::vector<std::string> flameLeft;

extern std::vector<std::string> cliffSprite;

int getArtWidth(const std::vector<std::string>& art);
