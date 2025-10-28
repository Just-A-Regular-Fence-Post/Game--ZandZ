#define NOMINMAX
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <algorithm>
#include <memory>
#include <cstdlib>
#include <ctime>

// Global console dimensions and single char buffer
int SCREEN_W = 80;
int SCREEN_H = 25;
std::vector<char> buffer;

// ---------------------- Console / Rendering Helpers ----------------------

void getConsoleSizeAndFixBuffer() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
        // fallback defaults
        SCREEN_W = 80;
        SCREEN_H = 25;
        buffer.resize(SCREEN_W * SCREEN_H, ' ');
        return;
    }

    // Window size (visible)
    SHORT winW = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    SHORT winH = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // Force buffer size to match the window size to avoid unexpected wrapping
    COORD newSize = { winW, winH };
    SetConsoleScreenBufferSize(hOut, newSize);

    SCREEN_W = winW;
    SCREEN_H = winH;
    buffer.assign(SCREEN_W * SCREEN_H, ' ');
}

void clearBuffer() {
    std::fill(buffer.begin(), buffer.end(), ' ');
}

inline void drawToBuffer(int x, int y, char c) {
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H)
        buffer[y * SCREEN_W + x] = c;
}

void render() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hOut, pos);

    // Build a single string representing the whole screen (lines end with '\n')
    std::string output;
    output.reserve((SCREEN_W + 1) * SCREEN_H);
    for (int y = 0; y < SCREEN_H; ++y) {
        int rowStart = y * SCREEN_W;
        output.append(&buffer[rowStart], &buffer[rowStart] + SCREEN_W);
        output.push_back('\n');
    }

    DWORD written = 0;
    WriteConsoleA(hOut, output.c_str(), (DWORD)output.size(), &written, nullptr);
}

// ---------------------- ASCII Art Helpers ----------------------

int getArtWidth(const std::vector<std::string>& art) {
    int maxW = 0;
    for (const auto& line : art) maxW = std::max(maxW, (int)line.size());
    return maxW;
}

void drawAsciiArt(int x, int y, const std::vector<std::string>& art) {
    for (int row = 0; row < (int)art.size(); ++row) {
        const std::string& line = art[row];
        for (int col = 0; col < (int)line.size(); ++col) {
            drawToBuffer(x + col, y + row, line[col]);
        }
    }
}

// ---------------------- ASCII art + small utilities ----------------------

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

std::vector<std::string> mainDrop = {
    "/\\",
    "\\/",
};

// ---------------------- Animations and effects ----------------------

void playStartAnimation() {
    clearBuffer();
    int targetY = SCREEN_H / 3;
    int startY = - (int)titleArt.size() - 1;
    int artW = getArtWidth(titleArt);
    int x = (SCREEN_W - artW) / 2;

    // Slide down
    for (int y = startY; y <= targetY; ++y) {
        clearBuffer();
        drawAsciiArt(x, y, titleArt);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    // Bounce a bit
    for (int i = 0; i < 20; ++i) {
        clearBuffer();
        drawAsciiArt(x, targetY + (i % 2 == 0 ? 1 : 0), titleArt);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void pressAnyKeyPrompt() {
    int blinkCounter = 0;
    int artW = getArtWidth(titleArt);
    int artX = (SCREEN_W - artW) / 2;
    int artY = SCREEN_H / 3;

    std::string msg = "PRESS ANY KEY";
    int msgX = (SCREEN_W - (int)msg.size()) / 2;
    int msgY = SCREEN_H / 2 + 4;

    while (true) {
        clearBuffer();
        drawAsciiArt(artX, artY, titleArt);

        if ((blinkCounter / 15) % 2 == 0) {
            for (int i = 0; i < (int)msg.size(); ++i)
                drawToBuffer(msgX + i, msgY, msg[i]);
        }

        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
        ++blinkCounter;

        if (_kbhit()) {
            _getch();
            break;
        }
    }
}

void mainDropper() {
    int secondStartY = - (int)mainDrop.size() - 1;
    int secondTargetY = SCREEN_H / 2;
    int x = (SCREEN_W - (int)mainDrop[0].size()) / 2;
    for (int y = secondStartY; y <= secondTargetY; ++y) {
        clearBuffer();
        drawAsciiArt(x, y, mainDrop);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
}

void shifty(int duration, int speed = 200) {

    for (int i = 0; i < duration; ++i) {
        system("color F0"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color E1"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color D2"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color C3"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color B4"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color A5"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color 96"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color 87"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color 78"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color 69"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color 5A"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color 4B"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color 3C"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color 2D"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color 1E"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("color 0F"); std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }
}

void nerd() {
    clearBuffer();
    std::vector<std::string> art = {
        " _   _ _____ ____   ____   ",
        "| \\ | |  ___|  _ \\ |  _ \\  ",
        "|  \\| | __| | |_) || | | | ",
        "| |\\  | |___|  _ < | |_| | ",
        "|_| \\_|_____|_| \\_\\|____/  ",
    };
    int w = getArtWidth(art);
    drawAsciiArt((SCREEN_W - w) / 2, SCREEN_H / 2 - (int)art.size()/2, art);
    render();
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

// ---------------------- Skill system ----------------------

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


class Move : public askills {
public:
    deets deety;
    Move(std::string name, float speed, float distance, float cooldown, float damage)
        : askills("Move") {
        deety.name = std::move(name);
        deety.speed = speed; deety.distance = distance; deety.cooldown = cooldown; deety.damage = damage;
    }
    void display() const override {
        std::cout << "Move Skill: " << deety.name << " | Speed: " << deety.speed << " | Distance: " << deety.distance << " | Cooldown: " << deety.cooldown << " | Damage: " << deety.damage << "\n";
    }
};

class Dash : public askills {
public:
    deets deety;
    Dash(std::string name, float speed, float distance, float cooldown, float damage)
        : askills("Dash") {
        deety.name = std::move(name);
        deety.speed = speed; deety.distance = distance; deety.cooldown = cooldown; deety.damage = damage;
    }
    void display() const override {
        std::cout << "Dash Skill: " << deety.name << " | Speed: " << deety.speed << " | Distance: " << deety.distance << " | Cooldown: " << deety.cooldown << " | Damage: " << deety.damage << "\n";
    }
};

class Shoot : public askills {
public:
    deets deety;
    Shoot(std::string name, std::string projectiletype, float firerate, float damage)
        : askills("Shoot") {
        deety.name = std::move(name);
        deety.projectiletype = std::move(projectiletype);
        deety.firerate = firerate; deety.damage = damage;
    }
    void display() const override {
        std::cout << "Shoot Skill: " << deety.name << " | Projectile: " << deety.projectiletype << " | Firerate: " << deety.firerate << " | Damage: " << deety.damage << "\n";
    }
};

class Heal : public askills {
public:
    deets deety;
    Heal(std::string name, float amount, float cooldown, float duration)
        : askills("Heal") {
        deety.name = std::move(name); deety.amount = amount; deety.cooldown = cooldown; deety.duration = duration;
    }
    void display() const override {
        std::cout << "Heal Skill: " << deety.name << " | Amount: " << deety.amount << " | Cooldown: " << deety.cooldown << " | Duration: " << deety.duration << "\n";
    }
};

class Block : public askills {
public:
    deets deety;
    Block(std::string name, float durability, float duration, float cooldown)
        : askills("Block") {
        deety.name = std::move(name); deety.durability = durability; deety.duration = duration; deety.cooldown = cooldown;
    }
    void display() const override {
        std::cout << "Block Skill: " << deety.name << " | Durability: " << deety.durability << " | Duration: " << deety.duration << " | Cooldown: " << deety.cooldown << "\n";
    }
};

class Jump : public askills {
public:
    deets deety;
    Jump(std::string name, float height, float cooldown)
        : askills("Jump") {
        deety.name = std::move(name); deety.height = height; deety.cooldown = cooldown;
    }
    void display() const override {
        std::cout << "Jump Skill: " << deety.name << " | Height: " << deety.height << " | Cooldown: " << deety.cooldown << "\n";
    }
};

class DoubleJump : public askills {
public:
    deets deety;
    DoubleJump(std::string name, float height, float cooldown)
        : askills("DoubleJump") {
        deety.name = std::move(name); deety.height = height; deety.cooldown = cooldown;
    }
    void display() const override {
        std::cout << "DoubleJump Skill: " << deety.name << " | Height: " << deety.height << " | Cooldown: " << deety.cooldown << "\n";
    }
};

class Hover : public askills {
public:
    deets deety;
    Hover(std::string name, float duration, float cooldown)
        : askills("Hover") {
        deety.name = std::move(name); deety.duration = duration; deety.cooldown = cooldown;
    }
    void display() const override {
        std::cout << "Hover Skill: " << deety.name << " | Duration: " << deety.duration << " | Cooldown: " << deety.cooldown << "\n";
    }
};

class pskills {
public:
    bool selected = false;
    std::string regen, kbresist, cdr, survive, fury, rage, armor;
};

class effects {
public:
    std::string stun, slow;
};

// Skill containers
std::vector<std::unique_ptr<askills>> skillslist;
std::vector<askills*> activeSkills;

// Select skills
void skillSelect() {
    activeSkills.clear();
    for (auto& s : skillslist) {
        if (s->selected) {
            activeSkills.push_back(s.get());
        }
    }
}

// Small demo function that populates skillslist with some sample skills
void populateSampleSkills() {
    skillslist.clear();
    skillslist.push_back(std::make_unique<Move>("Quickstep", 10.0f, 5.0f, 1.0f, 0.0f));
    skillslist.push_back(std::make_unique<Dash>("Burst", 15.0f, 8.0f, 2.0f, 1.0f));
    skillslist.push_back(std::make_unique<Shoot>("Firebolt", "bolt", 0.25f, 5.0f));
    skillslist.push_back(std::make_unique<Heal>("Mend", 20.0f, 10.0f, 3.0f));

    // Mark first two selected for demo
    if (skillslist.size() >= 2) {
        skillslist[0]->selected = true;
        skillslist[1]->selected = true;
    }

    skillSelect();
}

// Simple debug display of selected skills to console (not in the buffer)
void showSelectedSkillsConsole() {
    std::cout << "\nSelected Active Skills (" << activeSkills.size() << "):\n";
    for (auto* s : activeSkills) {
        s->display();
    }
    std::cout << std::flush;
}

// ---------------------- Full-Screen Skill Wheel w/ Green Selection ----------------------

std::map<std::string, std::vector<std::string>> skillArtWheel;

void initSkillArtWheel() {
    skillArtWheel["Quickstep"] = {
        " AAAAA ",
    };
    skillArtWheel["Burst"] = {
        " BBBBB ",
    };
    skillArtWheel["Firebolt"] = {
        " CCCCC ",
    };
    skillArtWheel["Mend"] = {
        " DDDDD ",
    };
    skillArtWheel["Shield"] = {
        " EEEEE ",
    };
    skillArtWheel["Dash"] = {
        " FFFFF ",
    };
    skillArtWheel["Hover"] = {
        " GGGGG ",
    };
}

// Draw ASCII art with optional color
void drawAsciiArtColor(int x, int y, const std::vector<std::string>& art, WORD color = 7) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOut, color);

    for (int row = 0; row < (int)art.size(); ++row) {
        for (int col = 0; col < (int)art[row].size(); ++col) {
            drawToBuffer(x + col, y + row, art[row][col]);
        }
    }

    SetConsoleTextAttribute(hOut, 7);
}

// Draw wheel with smooth offset and green highlight for center if selected
void drawSkillWheelScrollColor(const std::vector<std::string>& skillNames, int selectedIndex, float offset, bool isSelected) {
    clearBuffer();
    int centerY = SCREEN_H / 2;

    for (int i = 0; i < (int)skillNames.size(); ++i) {
        int relative = i - selectedIndex;
        float y = centerY + (relative - offset) * 10; // vertical spacing

        auto& art = skillArtWheel[skillNames[i]];
        int artW = getArtWidth(art);
        int x = (SCREEN_W - artW) / 2;

        if (y + art.size() > 0 && y < SCREEN_H) {
            if (relative == 0 && isSelected) {
                drawAsciiArtColor(x, (int)y, art, 10); // green
            } else {
                drawAsciiArt(x, (int)y, art); // default
            }
        }
    }

    render();
}

// Smooth scrolling selector with green toggle
int skillWheelSelectorSmoothColor(const std::vector<std::string>& skillNames) {
    int selected = 0;
    bool isSelected = false;

    while (true) {
        drawSkillWheelScrollColor(skillNames, selected, 0.0f, isSelected);
        int key = _getch();

        if (key == 224) { // arrow keys
            key = _getch();
            if (key == 72) { // up
                int next = (selected - 1 + skillNames.size()) % skillNames.size();
                for (int step = 0; step <= 10; ++step) {
                    drawSkillWheelScrollColor(skillNames, selected, step / 10.0f, isSelected);
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }
                selected = next;
            }
            if (key == 80) { // down
                int next = (selected + 1) % skillNames.size();
                for (int step = 0; step <= 10; ++step) {
                    drawSkillWheelScrollColor(skillNames, selected, -step / 10.0f, isSelected);
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }
                selected = next;
            }
        } else if (key == 's' || key == 'S') { // toggle select/unselect
            isSelected = !isSelected;
        } else if (key == '\r') { // enter
            return selected;
        }
    }
}


// ---------------------- Room / Player / Boss ----------------------

struct Entity {
    int x, y;
    char sprite;
    int width = 1, height = 1;

    Entity(int x_, int y_, char c) : x(x_), y(y_), sprite(c) {}
    
    void draw() const {
        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j)
                drawToBuffer(x + j, y + i, sprite);
    }
};

struct Platform {
    int x, y, w;
    Platform(int x_, int y_, int w_) : x(x_), y(y_), w(w_) {}
    void draw() const {
        for (int i = 0; i < w; ++i)
            drawToBuffer(x + i, y, '=');
    }
    bool collides(int px, int py) const {
        return py == y - 1 && px >= x && px < x + w;
    }
};

struct Projectile {
    int x, y;
    int dx, dy;
    char sprite;
    bool alive = true;

    Projectile(int x_, int y_, int dx_, int dy_, char s) 
        : x(x_), y(y_), dx(dx_), dy(dy_), sprite(s) {}
    
    void update() {
        x += dx;
        y += dy;
        if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) alive = false;
    }

    void draw() const {
        if (alive) drawToBuffer(x, y, sprite);
    }
};

class Boss {
public:
    int x, y;
    char sprite = 'B';
    std::vector<Projectile> projectiles;
    int cooldown = 0;

    Boss(int x_, int y_) : x(x_), y(y_) {}

    void update(Entity& player) {
        // Spawn a projectile towards player every 30 frames
        if (cooldown <= 0) {
            int dx = (player.x > x) ? 1 : (player.x < x ? -1 : 0);
            int dy = (player.y > y) ? 1 : (player.y < y ? -1 : 0);
            projectiles.emplace_back(x, y, dx, dy, '*');
            cooldown = 30;
        } else cooldown--;

        for (auto& p : projectiles) p.update();
        projectiles.erase(
            std::remove_if(projectiles.begin(), projectiles.end(),
                           [](const Projectile& p){ return !p.alive; }),
            projectiles.end()
        );
    }

    void draw() const {
        drawToBuffer(x, y, sprite);
        for (auto& p : projectiles) p.draw();
    }
};

void runRoom() {
    // Player setup
    Entity player(SCREEN_W/2, SCREEN_H-5, '@');
    int playerVy = 0;
    bool onGround = false;

    // Simple platforms
    std::vector<Platform> platforms = {
        Platform(5, SCREEN_H-2, 20),
        Platform(30, SCREEN_H-5, 25),
        Platform(60, SCREEN_H-8, 15)
    };

    Boss boss(SCREEN_W/2, 2);

    while (true) {
        clearBuffer();

        // Input
        if (_kbhit()) {
            int key = _getch();
            if (key == 224) { // arrow keys
                key = _getch();
                if (key == 75) player.x = std::max(0, player.x - 1); // left
                if (key == 77) player.x = std::min(SCREEN_W-1, player.x + 1); // right
                if (key == 72 && onGround) { playerVy = -3; onGround = false; } // jump
            }
        }

        // Gravity
        playerVy += 1; // gravity
        int nextY = player.y + playerVy;
        onGround = false;

        for (auto& plat : platforms) {
            if (plat.collides(player.x, nextY)) {
                nextY = plat.y - 1;
                playerVy = 0;
                onGround = true;
            }
        }

        // Keep player inside screen
        player.y = std::min(SCREEN_H - 1, nextY);
        player.y = std::max(0, player.y);

        // Update boss
        boss.update(player);

        // Draw everything
        for (auto& plat : platforms) plat.draw();
        player.draw();
        boss.draw();

        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


// ---------------------- Main ----------------------

int main() {
    std::cout << "Full screen this bitch and wait 10 seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    CONSOLE_CURSOR_INFO cursorInfo;
    srand((unsigned int)time(nullptr));

    getConsoleSizeAndFixBuffer();

    initSkillArtWheel();

    populateSampleSkills();

    std::vector<std::string> skillNames;
    for (auto& s : skillslist) skillNames.push_back(s->name);


    // Hide cursor
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
    if (GetConsoleCursorInfo(hOut, &cursorInfo)) {
        cursorInfo.bVisible = FALSE;
        cursorInfo.dwSize = 100; // always set this, prevents crashes
        SetConsoleCursorInfo(hOut, &cursorInfo);
    }
}


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

    // A little color flicker
    shifty(4, 80);


    //int selectedIndex = skillWheelSelectorSmoothColor(skillNames);
    //skillslist[selectedIndex]->selected = true;
    //skillSelect();


    //nerd();

    while(true) {
        runRoom();
    }

    // restore cursor visible before exit
    if (GetConsoleCursorInfo(hOut, &cursorInfo)) {
        cursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(hOut, &cursorInfo);
    }

    return 0;
}
