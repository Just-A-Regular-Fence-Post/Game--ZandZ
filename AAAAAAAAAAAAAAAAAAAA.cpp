#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <random>

int SCREEN_W, SCREEN_H;
std::vector<char> buffer;

// ==========================================================
// BASIC RENDER FUNCTIONS
// ==========================================================

void getConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    SCREEN_W = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    SCREEN_H = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    buffer.resize(SCREEN_W * SCREEN_H, ' ');
}

void clearBuffer() {
    std::fill(buffer.begin(), buffer.end(), ' ');
}

void drawToBuffer(int x, int y, char c) {
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H)
        buffer[y * SCREEN_W + x] = c;
}

void render() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hOut, pos);
    for (int y = 0; y < SCREEN_H; y++) {
        for (int x = 0; x < SCREEN_W; x++)
            std::cout << buffer[y * SCREEN_W + x];
        std::cout << '\n';
    }
}

// ==========================================================
// ASCII ART AND ANIMATION FUNCTIONS
// ==========================================================

std::vector<std::string> titleArt = {
"                                              ",
"     ____   _   _   ___    ____        ____   _   _   ___    ____   '   ____         ",
"    / ___| | | | | / _ \\  |  _ \\      / ___| | | | | / _ \\  |  _ \\     / ___| ",
"   | |     | |_| || /_\\ \\ | |_) |    | |     | |_| || /_\\ \\ | |_) |    \\___ \\ ",
"   | |___  |  _  ||  _  | |  _ <     | |___  |  _  ||  _  | |  _ <      ___) |",
"    \\____| |_| |_||_| |_| |_| \\_\\     \\____| |_| |_||_| |_| |_| \\_\\    |____/ ",

"    _____   _     ___   __  __   _____   ____   ____     ___  __     __  _____ ",
"   |  ___| | |   / _ \\ |  \\/  | | ____| / ___| |  _ \\   / _ \\ \\ \\   / / | ____|",
"   | |_    | |  | /_\\ \\| |\\/| | |  _|  | |  _  | |_) | | | | | \\ \\ / /  |  _|  ",
"   |  _|   | |__|  _  || |  | | | |___ | |_| | |  _ <  | |_| |  \\ V /   | |___ ",
"   |_|     |____|_| |_||_|  |_| |_____| \\____| |_| \\_\\  \\___/    \\_/    |_____|",
};

// ==========================================================
// ASCII LETTERS
// ==========================================================
std::map<char, std::vector<std::string>> letters = {
    {'A', {
        "  ___  ",
        " / _ \\ ",
        "/ /_\\ \\",
        "|  _  |",
        "|_| |_|"
    }},
    {'C', {
        "  ____ ",
        " / ___|",
        "| |    ",
        "| |___ ",
        " \\____|"
    }},
    {'H', {
        " _   _ ",
        "| | | |",
        "| |_| |",
        "|  _  |",
        "|_| |_|"
    }},
    {'R', {
        " ____  ",
        "|  _ \\ ",
        "| |_) |",
        "|  _ < ",
        "|_| \\_\\"
    }},
    {'S', {
        " ____  ",
        "/ ___| ",
        "\\___ \\ ",
        " ___) |",
        "|____/ "
    }},
    {'F', {
        " _____ ",
        "|  ___|",
        "| |_   ",
        "|  _|  ",
        "|_|    "
    }},
    {'L', {
        " _     ",
        "| |    ",
        "| |    ",
        "| |___ ",
        "|_____|"
    }},
    {'M', {
        " __  __ ",
        "|  \\/  |",
        "| |\\/| |",
        "| |  | |",
        "|_|  |_|"
    }},
    {'E', {
        " _____ ",
        "| ____|",
        "|  _|  ",
        "| |___ ",
        "|_____|"
    }},
    {'G', {
        "  ____ ",
        " / ___|",
        "| |  _ ",
        "| |_| |",
        " \\____|"
    }},
    {'V', {
        "__     __",
        "\\ \\   / /",
        " \\ \\ / / ",
        "  \\ V /  ",
        "   \\_/   "
    }},
    {'O', {
        "  ___  ",
        " / _ \\ ",
        "| | | |",
        "| |_| |",
        " \\___/ "
    }},
    {'\'', {
        " ' "
    }},
    {' ', {
        "  ",
        "  ",
        "  ",
        "  ",
        "  "
    }}
};
void drawAsciiArt(int x, int y, const std::vector<std::string>& art) {
    for (int row = 0; row < (int)art.size(); row++) {
        for (int col = 0; col < (int)art[row].size(); col++) {
            drawToBuffer(x + col, y + row, art[row][col]);
        }
    }
}

void playStartAnimation() {
    clearBuffer();
    int targetY = SCREEN_H / 3;
    int startY = -10;
    int x = (SCREEN_W - (int)titleArt[0].size()) / 2;

    // Slide down
    for (int y = startY; y <= targetY; y++) {
        clearBuffer();
        drawAsciiArt(x, y, titleArt);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    // Bounce a bit
    for (int i = 0; i < 20; i++) {
        clearBuffer();
        drawAsciiArt(x, targetY + (i % 2 == 0 ? 1 : 0), titleArt);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void pressAnyKeyPrompt() {
    int blinkCounter = 0;
    int x = SCREEN_W / 2 - 6;
    int y = SCREEN_H / 2 + 4;

    while (true) {
        clearBuffer();
        drawAsciiArt((SCREEN_W - (int)titleArt[0].size()) / 2, SCREEN_H / 3, titleArt);

        if ((blinkCounter / 15) % 2 == 0) {
            std::string msg = "PRESS ANY KEY";
            for (int i = 0; i < (int)msg.size(); i++)
                drawToBuffer(x + i, y, msg[i]);
        }

        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
        blinkCounter++;

        if (_kbhit()) {
            _getch();
            break;
        }
    }
}


std::vector<std::string> mainDrop = {
    "/\\",
    "\\/",
};


void mainDropper() {
    int secondStartY = -10;
    int secondTargetY = SCREEN_H / 2;
    int x = (SCREEN_W - (int)mainDrop[0].size()) / 2;
    for (int y = secondStartY; y <= secondTargetY; y++) {
        clearBuffer();
        drawAsciiArt(x, y, mainDrop);
        render();
    std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
}

void shifty(int duration, int speed = 200) {
    for (int i = 0; i < duration; i++) {
    system("color F0");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color E1");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color D2");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color C3");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color B4");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color A5");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color 96");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color 87");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color 78");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color 69");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color 5A");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color 4B");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color 3C");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color 2D");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color 1E");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    system("color 0F");
    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
}
};

void nerd() {
    clearBuffer();
    drawAsciiArt(SCREEN_W / 2 - 10, SCREEN_H / 2 - 3, {
" _   _ _____ ____   ____   ",
"| \\ | |  ___|  _ \\ |  _ \\  ",
"|  \\| | __| | |_) || | | | ",
"| |\\  | |___|  _ < | |_| | ",
"|_| \\_|_____|_| \\_\\|____/  ",
    })
;
    render();
    std::this_thread::sleep_for(std::chrono::seconds(2));
}


//===========================================================
// Game Functions
// ==========================================================
struct deets {
    std::string name;   
    std::string projectiletype;
    float firerate{};
    float speed{};
    float distance{};
    float cooldown{};
    float damage{};
    float amount{};
    float duration{};
    float window{};
    float durability{};
    float height{};
};

class askills {
public:
    bool selected = false;
    std::string name;

    explicit askills(std::string name) : name(std::move(name)) {}
    virtual ~askills() = default;

    virtual void display() const {
        std::cout << "Skill: " << name << "\n";
    }
};

// --- ACTIVE SKILLS ---

class Move : public askills {
public:
    deets deety;

    Move(std::string name, float speed, float distance, float cooldown, float damage)
        : askills("Move") {
        deety.name = std::move(name);
        deety.speed = speed;
        deety.distance = distance;
        deety.cooldown = cooldown;
        deety.damage = damage;
    }

    void display() const override {
        std::cout << "Move Skill: " << deety.name
                  << " | Speed: " << deety.speed
                  << " | Distance: " << deety.distance
                  << " | Cooldown: " << deety.cooldown
                  << " | Damage: " << deety.damage << "\n";
    }
};

class Dash : public askills {
public:
    deets deety;

    Dash(std::string name, float speed, float distance, float cooldown, float damage)
        : askills("Dash") {
        deety.name = std::move(name);
        deety.speed = speed;
        deety.distance = distance;
        deety.cooldown = cooldown;
        deety.damage = damage;
    }

    void display() const override {
        std::cout << "Dash Skill: " << deety.name
                  << " | Speed: " << deety.speed
                  << " | Distance: " << deety.distance
                  << " | Cooldown: " << deety.cooldown
                  << " | Damage: " << deety.damage << "\n";
    }
};

class Shoot : public askills {
public:
    deets deety;

    Shoot(std::string name, std::string projectiletype, float firerate, float damage)
        : askills("Shoot") {
        deety.name = std::move(name);
        deety.projectiletype = std::move(projectiletype);
        deety.firerate = firerate;
        deety.damage = damage;
    }

    void display() const override {
        std::cout << "Shoot Skill: " << deety.name
                  << " | Projectile: " << deety.projectiletype
                  << " | Firerate: " << deety.firerate
                  << " | Damage: " << deety.damage << "\n";
    }
};

class Heal : public askills {
public:
    deets deety;

    Heal(std::string name, float amount, float cooldown, float duration)
        : askills("Heal") {
        deety.name = std::move(name);
        deety.amount = amount;
        deety.cooldown = cooldown;
        deety.duration = duration;
    }

    void display() const override {
        std::cout << "Heal Skill: " << deety.name
                  << " | Amount: " << deety.amount
                  << " | Cooldown: " << deety.cooldown
                  << " | Duration: " << deety.duration << "\n";
    }
};

class Parry : public askills {
public:
    deets deety;

    Parry(std::string name, float window, float cooldown)
        : askills("Parry") {
        deety.name = std::move(name);
        deety.window = window;
        deety.cooldown = cooldown;
    }

    void display() const override {
        std::cout << "Parry Skill: " << deety.name
                  << " | Window: " << deety.window
                  << " | Cooldown: " << deety.cooldown << "\n";
    }
};

class Shield : public askills {
public:
    deets deety;

    Shield(std::string name, float durability, float duration, float cooldown)
        : askills("Shield") {
        deety.name = std::move(name);
        deety.durability = durability;
        deety.duration = duration;
        deety.cooldown = cooldown;
    }

    void display() const override {
        std::cout << "Shield Skill: " << deety.name
                  << " | Durability: " << deety.durability
                  << " | Duration: " << deety.duration
                  << " | Cooldown: " << deety.cooldown << "\n";
    }
};

class Jump : public askills {
public:
    deets deety;

    Jump(std::string name, float height, float cooldown)
        : askills("Jump") {
        deety.name = std::move(name);
        deety.height = height;
        deety.cooldown = cooldown;
    }

    void display() const override {
        std::cout << "Jump Skill: " << deety.name
                  << " | Height: " << deety.height
                  << " | Cooldown: " << deety.cooldown << "\n";
    }
};

class DoubleJump : public askills {
public:
    deets deety;

    DoubleJump(std::string name, float height, float cooldown)
        : askills("DoubleJump") {
        deety.name = std::move(name);
        deety.height = height;
        deety.cooldown = cooldown;
    }

    void display() const override {
        std::cout << "DoubleJump Skill: " << deety.name
                  << " | Height: " << deety.height
                  << " | Cooldown: " << deety.cooldown << "\n";
    }
};

class Hover : public askills {
public:
    deets deety;

    Hover(std::string name, float duration, float cooldown)
        : askills("Hover") {
        deety.name = std::move(name);
        deety.duration = duration;
        deety.cooldown = cooldown;
    }

    void display() const override {
        std::cout << "Hover Skill: " << deety.name
                  << " | Duration: " << deety.duration
                  << " | Cooldown: " << deety.cooldown << "\n";
    }
};


class pskills{
    public:
bool selected = false;
std::string regen, kbresist, cdr, survive, fury, armor;

};

class effects{
    public:
    std::string burn, freeze, shock, poison, bleed, stun, slow, weaken;
};

std::vector<std::unique_ptr<askills>> skillslist;
std::vector<skillslist> activeSkills;

void skillSelect() {
    //mark active skills as selected
    for (auto &skill : skillslist) {
        if (askills.selected = true){ 
            //add to player's active skills
            skillslist.push_back(askills);
}
    }
}

void testDisplay() {
    std::cout << "select a skill";

    std::cout << std::vector<class> skillslist;
}

void barbar() {

}





// ==========================================================
// MAIN
// ==========================================================

int main() {
    srand((unsigned int)time(nullptr));
    getConsoleSize();

    // Hide cursor
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);

    // Title animation
    playStartAnimation();
    pressAnyKeyPrompt();

    // Transition to main game
    clearBuffer();
    render();
    std::cout << "\n\nGame starting...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    mainDropper();
    clearBuffer();
    render();
    shifty(20, 10);

    // Placeholder main loop
    while (true) {
        // (Game code goes here)
        nerd();
        break;
    }

    return 0;
}
