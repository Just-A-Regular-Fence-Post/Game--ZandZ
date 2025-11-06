#include "Art.h"
#include <algorithm>

std::vector<std::string> titleArt = {
"     ____   _   _   ___    ____        ____   _   _   ___    ____   '   ____         ",
"    / ___| | | | | / _ \\  |  _ \\      / ___| | | | | / _ \\  |  _ \\     / ___| ",
"   | |     | |_| || /_\\ \\ | |_) |    | |     | |_| || /_\\ \\ | |_) |    \\___ \\ ",
"   | |___  |  _  ||  _  | |  _ <     | |___  |  _  ||  _  | |  _ <      ___) |",
"    \\____| |_| |_||_| |_| |_| \\_\\     \\____| |_| |_||_| |_| |_| \\_\\    |____/ ",
"",
"    _____   _     ___   __  __   _____   ____   ____     ___  __     __  _____ ",
"   |  ___| | |   / _ \\ |  \\/  | | ____| / ___| |  _ \\   / _ \\ \\ \\   / / | ____|",
"   | |_    | |  | /_\\ \\| |\\/| | |  _|  | |  _  | |_) | | | | | \\ \\ / /  |  _|  ",
"   |  _|   | |__|  _  || |  | | | |___ | |_| | |  _ <  | |_| |  \\ V /   | |___ ",
"   |_|     |____|_| |_||_|  |_| |_____| \\____| |_| \\_\\  \\___/    \\_/    |_____|",
};

std::vector<std::string> endArt = {
    " Congrats you dont have epilepsy! ",
};

std::vector<std::string> playerSprite = {
    " ____ ",
    "||  -|",
    "|_||_|",
};

std::vector<std::string> playerSpriteOtherWay = {
    " ____ ",
    "|-  ||",
    "|_||_|"
};

std::vector<std::string> bossSprite = {
    " /\\_/\\ ",
    "( o.o )",
    " > ^ < "
};

std::vector<std::string> flameRight = {
    "--===>",
    " --===>",
    "--===>"
};

std::vector<std::string> flameLeft = {
    "<===--",
    "<===-- ",
    "<===--"
};

std::vector<std::string> cliffSprite = {
    "████",
    "███",
    "█████",
    "███",
    "██████"
};

int getArtWidth(const std::vector<std::string>& art) {
    int maxW = 0;
    for (const auto& line : art)
        maxW = std::max(maxW, (int)line.size());
    return maxW;
}
