#define NOMINMAX
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>
#include <ctime>
#include <conio.h> 
#include <windows.h>   

int SCREEN_W = 80;
int SCREEN_H = 25;
std::vector<char> buffer;

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

    SHORT winW = csbi.srWindow.Right - csbi.srWindow.Left; // remove if flickering
    SHORT winH = csbi.srWindow.Bottom - csbi.srWindow.Top;

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
    COORD pos = {0, 0};
    SetConsoleCursorPosition(hOut, pos);

    std::string output;
    output.reserve((SCREEN_W + 1) * SCREEN_H);
    for (int y = 0; y < SCREEN_H; y++) {
        int rowStart = y * SCREEN_W;
        output.append(&buffer[rowStart], &buffer[rowStart] + SCREEN_W);
        output.push_back('\n');
    }

    DWORD written = 0;
    WriteConsoleA(hOut, output.c_str(), (DWORD)output.size(), &written, nullptr);
}

// =================================

extern std::vector<std::string> titleArt;
extern std::vector<std::string> endArt;

int getArtWidth(const std::vector<std::string>& art) {
    int maxW = 0;
    for (const auto& line : art) maxW = std::max(maxW, (int)line.size());
    return maxW;
}

void drawAsciiArt(int x, int y, const std::vector<std::string>& art) {
    for (int row = 0; row < (int)art.size(); row++) {
        const std::string& line = art[row];
        for (int col = 0; col < (int)line.size(); col++) {
            if (y + row >= 0 && y + row < SCREEN_H)
                drawToBuffer(x + col, y + row, line[col]);
        }
    }
}

void playStartAnimation() {
    clearBuffer();
    int targetY = SCREEN_H / 3;
    int startY = - (int)titleArt.size() - 1;
    int artW = getArtWidth(titleArt);
    int x = (SCREEN_W - artW) / 2;

    for (int y = startY; y <= targetY; y++) {
        clearBuffer();
        drawAsciiArt(x, y, titleArt);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    for (int i = 0; i < 20; i++) {
        clearBuffer();
        drawAsciiArt(x, targetY + (i % 2 == 0 ? 1 : 0), titleArt);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void playEndAnimation() {
    clearBuffer();
    int targetY = SCREEN_H / 3;
    int startY = - (int)endArt.size() - 1;
    int artW = getArtWidth(endArt);
    int x = (SCREEN_W - artW) / 2;

    for (int y = startY; y <= targetY; y++) {
        clearBuffer();
        drawAsciiArt(x, y, endArt);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    for (int i = 0; i < 20; i++) {
        clearBuffer();
        drawAsciiArt(x, targetY + (i % 2 == 0 ? 1 : 0), endArt);
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
            for (int i = 0; i < (int)msg.size(); i++)
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

void slideBlackBars(int speed = 33) {
    clearBuffer();
    int mid = SCREEN_H / 2;
    for (int i = 0; i <= mid; i++) {
        clearBuffer();
        for (int y = 0; y <= i; y++)
            for (int x = 0; x < SCREEN_W; x++)
                drawToBuffer(x, y, (char)219);
        for (int y = SCREEN_H - 1; y >= SCREEN_H - 1 - i; --y)
            for (int x = 0; x < SCREEN_W; x++)
                drawToBuffer(x, y, (char)219);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void slideBlackBarsOpen(int speed = 33) {
    int mid = SCREEN_H / 2;
    for (int i = mid; i >= 0; --i) {
        clearBuffer();
        for (int y = 0; y <= i; y++)
            for (int x = 0; x < SCREEN_W; x++)
                drawToBuffer(x, y, (char)219);
        for (int y = SCREEN_H - 1; y >= SCREEN_H - 1 - i; --y)
            for (int x = 0; x < SCREEN_W; x++)
                drawToBuffer(x, y, (char)219);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }
}

void shifty(int duration, int speed = 20) {

    for (int i = 0; i < duration; i++) {
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
//=======================================

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



struct Wall {
    int x, y, w, h;
    Wall(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_) {}
    void draw() const {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                drawToBuffer(x + j, y + i, '█');
    }
};

struct Platform {
    int x, y, w, h;
    Platform(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_) {}
    void draw() const {
        for (int i = 0; i < w; i++)
            for (int j = 0; j < h; j++)
            drawToBuffer(x + i, y + j, '█');
    }
};

struct Controls {
    int moveLeft;
    int moveRight;
    int jump;
    int attack;
};

int mapKeyForAsyncKey(int key) {
    if (key == 224) { // special key prefix
        int arrow = _getch();
        switch (arrow) {
            case 72: return VK_UP;    // Up arrow
            case 75: return VK_LEFT;  // Left arrow
            case 77: return VK_RIGHT; // Right arrow
            case 80: return VK_DOWN;  // Down arrow
        }
    }
    return key; // normal key
}

int askAnyKey(const std::string& action) {
    std::cout << "Press a key for " << action << ": ";
    int key = _getch();
    int mappedKey = mapKeyForAsyncKey(key);

    if (mappedKey < 256) std::cout << (char)mappedKey << "\n";
    else std::cout << "(Arrow key)\n";

    return mappedKey;
}

Controls queryUserControls() {
    Controls c;
    c.moveLeft  = askAnyKey("Move Left");
    c.moveRight = askAnyKey("Move Right");
    c.jump      = askAnyKey("Jump");
    c.attack    = askAnyKey("Attack");
    return c;
}

struct Entity {
    float x, y; // position
    int vx = 0; // velocity
    float vy{}; // velocity
    int w = 1, h = 1; // width and height
    bool onGround = false; // is the entity on the ground
    bool facingRight = true; // direction the entity is facing

    std::vector<std::string> sprite; // visual representation

    // Constructor
    Entity(int x_, int y_, const std::vector<std::string>& art) // pos, pos, sprite
        : x(x_), y(y_), sprite(art) {
        h = (int)sprite.size();
        w = (h > 0) ? (int)sprite[0].size() : 1;
    }

    virtual ~Entity() = default;

    virtual void draw() const {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                if (sprite[i][j] != ' ')
                    drawToBuffer(x + j, y + i, sprite[i][j]);
    }

   void applyPhysics(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {

        // Gravity
        vy += 2.0f;
        if (vy > 2.9) vy = 2.9;

        onGround = false;

        // =======================
        // Vertical movement
        // =======================
        float nextY = y + vy;
        y = nextY;

        // Platform vertical collisions
        for (auto& p : platforms) {
            bool xOverlap = (x + w > p.x && x < p.x + p.w);

            // Landing on top of platform
            if (xOverlap && vy >= 0 && y + h > p.y && y + h <= p.y + 3) {
                y = p.y - h;
                vy = 0;
                onGround = true;
            }

            // Hitting head on platform
            if (xOverlap && vy < 0 && y < p.y + p.h && y + h > p.y) {
                y = p.y + p.h;
                vy = 0;
            }
        }

        // Wall vertical collisions
        for (auto& w : walls) {
            if (x + this->w > w.x && x < w.x + w.w &&
                y + this->h > w.y && y < w.y + w.h)
            {
                if (vy > 0) { y = w.y - this->h; onGround = true; }
                else if (vy < 0) y = w.y + w.h;

                vy = 0;
            }
        }

        // =======================
        // Horizontal movement
        // =======================
        float nextX = x + vx;
        x = nextX;

        // Platform side collisions (only when feet are roughly at same level)
        for (auto& p : platforms) {
            bool yOverlap = (y + h > p.y && y < p.y + p.h);

            if (yOverlap) {
                // Colliding while moving right
                if (vx > 0 && x + w > p.x && x < p.x) {
                    x = p.x - w;
                    vx = 0;
                }
                // Colliding while moving left
                else if (vx < 0 && x < p.x + p.w && x + w > p.x + p.w) {
                    x = p.x + p.w;
                    vx = 0;
                }
            }
        }

        // Wall side collisions
        for (auto& w : walls) {
            if (x + this->w > w.x && x < w.x + w.w &&
                y + this->h > w.y && y < w.y + w.h)
            {
                if (vx > 0) x = w.x - this->w;
                if (vx < 0) x = w.x + w.w;
                vx = 0;
            }
        }

        // =======================
        // Screen boundaries
        // =======================
        if (y > SCREEN_H - h) {
            y = SCREEN_H - h;
            vy = 0;
            onGround = true;
        }

        if (x < 0) x = 0;
        if (x >= SCREEN_W - w) x = SCREEN_W - w;
    }

    void update(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {
        applyPhysics(platforms, walls);
    }
};

class Cliff : public Entity {
    public:
    Cliff(int x_, int y_)
        : Entity(x_, y_, cliffSprite) {}
};

class Flame : public Entity {
public:
    Flame(int x_, int y_, bool facingRight)
        : Entity(x_, y_, facingRight ? flameRight : flameLeft) {}

    bool intersects(const Entity& other) const {
        return !(x + w <= other.x || x >= other.x + other.w ||
                 y + h <= other.y || y >= other.y + other.h);
    }
};

class Player : public Entity {
public:
    Controls controls;
    bool flamethrowerOn = false;
    Flame* flame = nullptr;
    float flamethrowerCooldown = 0.0f; // seconds
    float change = 0.4f;
    float health = 100;      // max health = 100
    float maxHealth = 100;

    Player(int x_, int y_, Controls c) : Entity(x_, y_, playerSprite), controls(c) {}

    void attack() {
        bool keyPressed = GetAsyncKeyState(controls.attack) & 0x8000;

        if (keyPressed && flamethrowerCooldown <= 0.0f) {
            flamethrowerOn = true;

            if (!flame) {
                flame = new Flame(x + (facingRight ? w : -6), y, facingRight);
            } else {
                flame->x = x + (facingRight ? w : -6);
                flame->y = y;
                flame->facingRight = facingRight;
                flame->sprite = facingRight ? flameRight : flameLeft;
            }

            flamethrowerCooldown = 0.2f; // flamethrower ticks every 0.2s
        } else if (!keyPressed && flamethrowerOn) {
            flamethrowerOn = false;
            if (flame) {
                delete flame;
                flame = nullptr;
            }
        }
    }

    void handleInput() {
        vx = 0;

        // Move Left
        if (GetAsyncKeyState(controls.moveLeft) & 0x8000) {
            vx = -3;
            facingRight = false;
        }

        // Move Right
        if (GetAsyncKeyState(controls.moveRight) & 0x8000) {
            vx = 3;
            facingRight = true;
        }

        // Jump
        if (GetAsyncKeyState(controls.jump) & 0x8000) {
            if (onGround) vy = -9;
        }

        // Shoot
        if (GetAsyncKeyState(controls.attack) & 0x8000) {
            attack();
        }
    }

    void update(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {
        flamethrowerCooldown -= change;
        if (flamethrowerCooldown < 0.0f)
            flamethrowerCooldown = 0.0f;
        handleInput();
        attack();
        sprite = facingRight ? playerSprite : playerSpriteOtherWay;
        applyPhysics(platforms, walls);
    }
};

enum BossState {
    // skills
    FIREBREATH,
    SPAWNITONYASSPROJECTILE,
    BRUTALCOUNTER,
    REALLYSTRONGASSPUNCH,
    NOLIZARDSALLOWED,
    MORPH,
    FROSTBREATH,
    AVALANCHE,
    SPAWNITONYOASSPROJECTILEV2,
    FROSTPUNCH,
    FISSURE,
    CANTESCAPETHISHAMMER,
    THEWHEELOFMISFORTUNE,
    NEVERGONNALETYOUDOWN,
    CASTTHEDIEOFFATE,
    WHATSINTHECARDS,
    // actions
    RETREAT,
    CHASE,
    DASH,
    IDLE,

};

class Boss : public Entity {
    public:
    // Boss properties
    BossState state = IDLE;
    float health = 1000;
    float cooldown = 0; // Timer before performing another action
    float stateTimer = 0.0f; // counts down attack duration
    bool invulnerable = false; 


    
    Boss(int x_, int y_) : Entity(x_, y_, bossSprite) {
        w = (int)bossSprite[0].size();
        h = (int)bossSprite.size();
    }
    
    void takeDamage(float dmg) {
        if (!invulnerable)
            health -= dmg;
        if (health < 0) health = 0;
    }

    // Conditions
    void updateAI(const Player& player) {
        int playerCenter = player.x + player.w / 2;
        int bossCenter = x + w / 2;
        float distance = abs(playerCenter - bossCenter);

        facingRight = playerCenter > bossCenter;

        // Priority-based behavior
        if (distance <= 2) {
            state = BRUTALCOUNTER;
        } else if (distance < 10) {
            state = RETREAT;
        } else if (distance > 2 && distance <= 4) {
            state = FIREBREATH;
        } else {
            state = CHASE;  // move toward player if too far
        }
    }

    // Boss state cases
    void executeAction(const Player& player, const std::vector<Wall>& walls) {
        if(cooldown > 0) {
            cooldown--;
            return;
        }
        
        switch (state) { // return to idle state after each to determine next state
            case FIREBREATH:
 
                break;
            case SPAWNITONYASSPROJECTILE:

                break;
            case BRUTALCOUNTER:
                invulnerable = true;      
                stateTimer = 50;      
                // Move toward player rapidly
                vx = (player.x + player.w/2 > x + w/2) ? 4 : -4;
                break;
            case REALLYSTRONGASSPUNCH:

                break;
            case NOLIZARDSALLOWED:
  
                break;
            case MORPH:

                break;


            case FROSTBREATH:
 
                break;
            case AVALANCHE:
  
                break;
            case SPAWNITONYOASSPROJECTILEV2:

                break;
            case FROSTPUNCH:
  
                break;
            case FISSURE:

                break;
            case CANTESCAPETHISHAMMER:
 
                break;
            case THEWHEELOFMISFORTUNE:

                break;
            case NEVERGONNALETYOUDOWN:

                break;
            case CASTTHEDIEOFFATE:

                break;
            case WHATSINTHECARDS:

                break;
            case RETREAT:
                vx = (player.x + player.w/2 < x + w/2) ? 1 : -1;
                break;
            case CHASE:
                vx = (player.x + player.w / 2 > x + w / 2) ? 1 : -1;
                break;
            case IDLE:
                vx = 0;
                break;
        }
    }

    void update(const std::vector<Platform>& platforms, const std::vector<Wall>& walls, const Player& player) {
            updateAI(player);
            executeAction(player, walls); 
            applyPhysics(platforms, walls);

            if (cooldown > 0)
                cooldown -= 0.1f;
        
        if (stateTimer > 0) {
            stateTimer--;
            if (stateTimer <= 0) {
                invulnerable = false;
                state = IDLE; // reset to normal
            }
        }
    }
};

void showCursor(bool visible) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    if (GetConsoleCursorInfo(out, &ci)) {
        ci.bVisible = visible ? TRUE : FALSE;
        SetConsoleCursorInfo(out, &ci);
    }
}

void resizer() {
static int frameCount = 0;
static int lastW = SCREEN_W, lastH = SCREEN_H;

    frameCount++;

    // Every 20 frames (adjust as needed)
    if (frameCount % 20 == 0) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            int winW = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            int winH = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

            if (winW != lastW || winH != lastH) {
                lastW = winW;
                lastH = winH;
                SCREEN_W = winW;
                SCREEN_H = winH;
                buffer.assign(SCREEN_W * SCREEN_H, ' ');
            }
        }
    }
}

void damageTester() {

}

void drawBossHealthBar(const Boss& boss) {
    int barWidth = boss.w;  // same width as boss sprite
    int filled = (int)((boss.health / 1000.0f) * barWidth); // assuming max health = 1000

    int barX = (int)boss.x;
    int barY = (int)boss.y - 1; // one row above boss

    if (barY < 0) return; // don't draw off-screen

    for (int i = 0; i < barWidth; i++) {
        char c = (i < filled) ? '#' : '-';
        drawToBuffer(barX + i, barY, c);
    }
}

void drawHealthBar(const Player& player) {
    int barWidth = player.w; // same width as player sprite
    int filled = (int)((player.health / player.maxHealth) * barWidth);
    int barX = (int)player.x;
    int barY = (int)player.y - 1;

    if (barY < 0) return;

    for (int i = 0; i < barWidth; i++) {
        char c = (i < filled) ? '#' : '-';
        drawToBuffer(barX + i, barY, c);
    }
}


int main() {
    std::this_thread::sleep_for(std::chrono::seconds(4));
    srand((unsigned int)time(nullptr));
    getConsoleSizeAndFixBuffer();
    showCursor(false);

    std::cout << "=== Control Setup ===\n";
    Controls userControls = queryUserControls();
    std::cout << "\nControls set! Starting game...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    clearBuffer();
    render();



    Player player(10, SCREEN_H - 5, userControls);

    Boss boss(SCREEN_W / 2, SCREEN_H - 10);


    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();
    int frames = 0;
    float fps = 0.0f;

    // Platforms scattered from bottom to top
    std::vector<Platform> platforms = {
        {0, SCREEN_H, SCREEN_W, 1},
        {0, 1, SCREEN_W, 1},
        {SCREEN_W / 5, (SCREEN_H * 4) / 5, 15, 1},
        {(SCREEN_W * 4) / 5, (SCREEN_H * 4) / 5, 15, 1},
        {SCREEN_W / 3, SCREEN_H / 2, SCREEN_W / 3, 2},
    };

    std::vector<Wall> walls = {
        {0, 1, 1, SCREEN_H - 1},
        {SCREEN_W - 1, 1, 1, SCREEN_H - 1},
    };

    std::vector<Cliff> cliffs = {
        {0, SCREEN_H - 10},      // left wall
        {SCREEN_W - 6, SCREEN_H - 12} // right wall
    };


    runPackage();

    while (true) {
        resizer();
        clearBuffer();

        // Update
        player.update(platforms, walls);
        boss.update(platforms, walls, player);

        // Draw
        for (auto& p : platforms) p.draw();
        for (auto& wall : walls) wall.draw();
        for (auto& cl : cliffs) cl.draw();
        player.draw();
        drawHealthBar(player);

        boss.draw();
        drawBossHealthBar(boss); 


        if (player.flamethrowerOn && player.flame) {
            player.flame->draw();

            if (player.flame->intersects(boss)) {
                boss.takeDamage(1); // apply damage per tick
            }
        }
  
        if (!(player.x + player.w <= boss.x || player.x >= boss.x + boss.w ||
            player.y + player.h <= boss.y || player.y >= boss.y + boss.h)) {
            player.health -= 1.1f; // damage per frame of contact
            if (player.health < 0) player.health = 0;
        }


        if (boss.health <= 0) {
            clearBuffer();
            
            // Display victory message
            runEndPackage();
            render();
            
            std::this_thread::sleep_for(std::chrono::seconds(3));
            break;
        }

        if (player.health <= 0) {
            clearBuffer();

            runEndPackage();
            render();

            std::this_thread::sleep_for(std::chrono::seconds(3));
            break;
        }

        // FPS display
        std::string fpsText = "FPS: " + std::to_string((int)fps);
        for (size_t i = 0; i < fpsText.size() && i < (size_t)SCREEN_W; i++)
            drawToBuffer(i, 0, fpsText[i]);

        render();

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