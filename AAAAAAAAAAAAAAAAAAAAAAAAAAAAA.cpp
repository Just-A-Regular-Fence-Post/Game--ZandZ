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

// ---------------------- Global console / buffer ----------------------
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

    SHORT winW = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    SHORT winH = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

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
    for (int y = 0; y < SCREEN_H; ++y) {
        int rowStart = y * SCREEN_W;
        output.append(&buffer[rowStart], &buffer[rowStart] + SCREEN_W);
        output.push_back('\n');
    }

    DWORD written = 0;
    WriteConsoleA(hOut, output.c_str(), (DWORD)output.size(), &written, nullptr);
}


void renderSimple() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hOut, pos);
    for (int y = 0; y < SCREEN_H; y++) {
        for (int x = 0; x < SCREEN_W; x++)
            std::cout << buffer[y * SCREEN_W + x];
        std::cout << '\n';
    }
}

// ---------------------- ASCII helpers / art ----------------------
int getArtWidth(const std::vector<std::string>& art) {
    int maxW = 0;
    for (const auto& line : art) maxW = std::max(maxW, (int)line.size());
    return maxW;
}

void drawAsciiArt(int x, int y, const std::vector<std::string>& art) {
    for (int row = 0; row < (int)art.size(); ++row) {
        const std::string& line = art[row];
        for (int col = 0; col < (int)line.size(); ++col) {
            if (y + row >= 0 && y + row < SCREEN_H)
                drawToBuffer(x + col, y + row, line[col]);
        }
    }
}

// ---------------------- ASCII sprites ----------------------
std::vector<std::string> playerSprite = {
    " ____ ",
    "||  -|",
    "|_||_|",
};

std::vector<std::string> bossSprite = {
    " /\\_/\\ ",
    "( o.o )",
    " > ^ < "
};

std::vector<std::string> titleArt = {
    " Place Holder Title Cuz Im Lazy ",
};

// -------------------------- Animations --------------------------
void playStartAnimation() {
    clearBuffer();
    int targetY = SCREEN_H / 3;
    int startY = - (int)titleArt.size() - 1;
    int artW = getArtWidth(titleArt);
    int x = (SCREEN_W - artW) / 2;

    for (int y = startY; y <= targetY; ++y) {
        clearBuffer();
        drawAsciiArt(x, y, titleArt);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

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

void slideBlackBars(int speed = 33) {
    clearBuffer();
    int mid = SCREEN_H / 2;
    for (int i = 0; i <= mid; ++i) {
        clearBuffer();
        for (int y = 0; y <= i; ++y)
            for (int x = 0; x < SCREEN_W; ++x)
                drawToBuffer(x, y, (char)219);
        for (int y = SCREEN_H - 1; y >= SCREEN_H - 1 - i; --y)
            for (int x = 0; x < SCREEN_W; ++x)
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
        for (int y = 0; y <= i; ++y)
            for (int x = 0; x < SCREEN_W; ++x)
                drawToBuffer(x, y, (char)219);
        for (int y = SCREEN_H - 1; y >= SCREEN_H - 1 - i; --y)
            for (int x = 0; x < SCREEN_W; ++x)
                drawToBuffer(x, y, (char)219);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }
}

// ------------------------ World objects ------------------------
struct Wall {
    int x, y, w, h;
    Wall(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_) {}
    void draw() const {
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
                drawToBuffer(x + j, y + i, '#');
    }
};

struct Platform {
    int x, y, w;
    Platform(int x_, int y_, int w_) : x(x_), y(y_), w(w_) {}
    void draw() const {
        for (int i = 0; i < w; ++i)
            drawToBuffer(x + i, y, '=');
    }
};

// ------------------------ Utility collision ------------------------
bool intersectsRect(int x1, int y1, int w1, int h1,
                    int x2, int y2, int w2, int h2)
{
    return !(x1 + w1 <= x2 || x1 >= x2 + w2 || y1 + h1 <= y2 || y1 >= y2 + h2);
}

// ------------------------ Entities & projectiles ------------------------
struct Projectile {
    int x, y;
    int vx, vy;
    char sprite;
    bool active = true;

    Projectile(int x_, int y_, int vx_, int vy_, char s = '*')
        : x(x_), y(y_), vx(vx_), vy(vy_), sprite(s) {}

    void update(const std::vector<Wall>& walls) {
        if (!active) return;
        x += vx;
        y += vy;
        for (auto& w : walls) {
            if (x >= w.x && x < w.x + w.w && y >= w.y && y < w.y + w.h) {
                active = false;
                break;
            }
        }
        if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) active = false;
    }

    void draw() const {
        if (active) drawToBuffer(x, y, sprite);
    }
};

struct Entity {
    int x, y;
    int vx = 0, vy = 0;
    int w = 1, h = 1;
    bool onGround = false;
    std::vector<std::string> sprite;

    Entity(int x_, int y_, const std::vector<std::string>& art)
        : x(x_), y(y_), sprite(art)
    {
        h = (int)sprite.size();
        w = (h > 0) ? (int)sprite[0].size() : 1;
    }

    virtual ~Entity() = default;

    virtual void draw() const {
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
                if (sprite[i][j] != ' ')
                    drawToBuffer(x + j, y + i, sprite[i][j]);
    }

    // generic physics: handles walls and platforms (AABB)
    void applyPhysics(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {
        vy += 1; // gravity
        if (vy > 3) vy = 3;

        int nextX = x + vx;
        int nextY = y + vy;
        onGround = false;

        // horizontal movement + wall collisions
        x = nextX;
        for (auto& w : walls) {
            if (nextX + this->w > w.x && nextX < w.x + w.w &&
                y + this->h > w.y && y < w.y + w.h) {
            if (vx > 0) x = w.x - this->w;      // hitting wall from the left
            if (vx < 0) x = w.x + w.w;          // hitting wall from the right
            }
        }



        // vertical movement + wall collisions
        y = nextY;
        for (auto& w : walls) {
            if (x + w > w.x && x < w.x + w.w &&
                nextY + h > w.y && nextY < w.y + w.h) {
                if (vy > 0) { y = w.y - h; onGround = true; }
                else if (vy < 0) y = w.y + w.h;
                vy = 0;
            }
        }

        // platform collisions (from above)
        for (auto& p : platforms) {
            if (y + h > p.y && y + h <= p.y + 1 && x + w > p.x && x < p.x + p.w && vy >= 0) {
                y = p.y - h;
                vy = 0;
                onGround = true;
            }
        }

        // screen bounds
        if (y > SCREEN_H - h) { y = SCREEN_H - h; vy = 0; onGround = true; }
        if (x < 0) x = 0;
        if (x > SCREEN_W - w) x = SCREEN_W - w;

        vx = 0;
    }
};

// ------------------------ Player ------------------------
class Player : public Entity {
public:
    int health = 100;
    Player(int x_, int y_) : Entity(x_, y_, playerSprite) {}

    void handleInput() {
        if (!_kbhit()) return;
        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 75) vx = -1;
            if (key == 77) vx = 1;
            if (key == 72 && onGround) { vy = -4; onGround = false; }
        } else {
            // optional other keys
        }
    }

    void update(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {
        handleInput();
        applyPhysics((std::vector<Platform>&)platforms, (std::vector<Wall>&)walls);
    }
};

// ------------------------ Boss AI ------------------------
enum BossState {
    IDLE,
    CHARGING,
    LONG_RANGE_ATTACK,
    SHORT_RANGE_ATTACK,
    RETREAT,
    RAGE_MODE
};

class Boss : public Entity {
public:
    int health = 100;
    BossState state = IDLE;
    int attackTimer = 0;
    int cooldown = 0;
    std::vector<Projectile> projectiles;
    bool facingRight = true;

    Boss(int x_, int y_) : Entity(x_, y_, bossSprite) {
        w = (int)bossSprite[0].size();
        h = (int)bossSprite.size();
    }

    // keep boss on the nearest platform below its x range; if none, stand on floor
    void stayOnPlatform(const std::vector<Platform>& platforms) {
        int groundY = SCREEN_H; // bottom default
        for (const auto& p : platforms) {
            if (x + w > p.x && x < p.x + p.w) {
                // platform overlaps horizontally
                groundY = std::min(groundY, p.y);
            }
        }
        y = groundY - h;
        vy = 0;
        onGround = true;
    }

    void updateAI(const Player& player, const std::vector<Platform>& platforms) {
        int playerCenter = player.x + player.w / 2;
        int bossCenter = x + w / 2;
        int distance = abs(playerCenter - bossCenter);
        facingRight = playerCenter > bossCenter;

        // 5) Rage override
        if (health <= 30) {
            if (state != RAGE_MODE) {
                state = RAGE_MODE;
                attackTimer = 0;
                cooldown = 0;
            }
        }

        // if already in rage mode, skip normal state selection (rage handles its own timers)
        if (state == RAGE_MODE) return;

        // don't change state while charging or cooling
        if (attackTimer > 0 || cooldown > 0) return;

        // 3) maintain minimum distance logic: if too close -> RETREAT
        if (distance < 6) {
            state = RETREAT;
            return;
        }

        // 1) choose move by range
        if (distance < 8) {
            // close
            state = SHORT_RANGE_ATTACK;
        } else if (distance < 25) {
            // midrange
            state = LONG_RANGE_ATTACK;
        } else {
            state = IDLE;
        }
    }

    void executeAction(const Player& player, const std::vector<Wall>& walls) {
        // cooldown/attackTimer bookkeeping
        if (cooldown > 0) { --cooldown; return; }
        if (attackTimer > 0) { --attackTimer; 
            // if charging finishes now and state==CHARGING, fire
            if (attackTimer == 0 && state == CHARGING) {
                int dir = facingRight ? 1 : -1;
                // spawn a projectile at boss front, one tile above bottom of sprite
                int spawnX = facingRight ? x + w : x - 1;
                int spawnY = y + h / 2;
                projectiles.emplace_back(spawnX, spawnY, dir, 0, '*');
                cooldown = 30;
                state = IDLE;
            }
            return;
        }

        switch (state) {
        case IDLE:
            if (rand() % 60 == 0) vx = (rand() % 2 ? 1 : -1);
            break;

        case RETREAT:
            // move away from player and set short cooldown
            vx = (player.x + player.w/2 < x + w/2) ? 1 : -1;
            attackTimer = 8;
            cooldown = 18;
            state = IDLE; // retreat is a short action; revert to IDLE after action started
            break;

        case LONG_RANGE_ATTACK:
            // telegraph and charge to fire
            state = CHARGING;
            attackTimer = 16; // telegraph time
            break;

        case SHORT_RANGE_ATTACK:
            // lunge: quick horizontal movement then cooldown and fallback
            vx = (player.x + player.w/2 > x + w/2) ? 2 : -2;
            attackTimer = 6;
            cooldown = 30;
            // after lunge, boss will be put back onto platform in update() via stayOnPlatform
            state = IDLE;
            break;

        case RAGE_MODE:
            // aggressive pattern: rapid 3-shot burst then small cooldown
            for (int i = -1; i <= 1; ++i) {
                int dir = facingRight ? 1 : -1;
                int spawnX = facingRight ? x + w : x - 1;
                int spawnY = std::clamp(y + h/2 + i, 0, static_cast<int>(SCREEN_H - 1));
                projectiles.emplace_back(spawnX, spawnY, dir, 0, 'o');
            }
            cooldown = 12;
            break;

        default:
            break;
        }
    }

    void updateProjectiles(const std::vector<Wall>& walls) {
        for (auto& p : projectiles) p.update(walls);
        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
                                         [](const Projectile& p){ return !p.active; }),
                          projectiles.end());
    }

    void update(const std::vector<Platform>& platforms, const std::vector<Wall>& walls, const Player& player) {
        updateAI(player, platforms);
        executeAction(player, walls);

        // movement: boss doesn't use gravity (rule 2). we move horizontally then snap to platform
        x += vx;
        vx = 0;
        if (x < 0) x = 0;
        if (x > SCREEN_W - w) x = SCREEN_W - w;

        // snap to platform (so it never falls)
        stayOnPlatform(platforms);

        // update projectiles
        updateProjectiles(walls);
    }

    void drawAll() const {
        drawAsciiArt(x, y, sprite);
        for (const auto& p : projectiles) p.draw();
    }
};

// --------------------- Game Loop -----------------------
void showCursor(bool visible) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    if (GetConsoleCursorInfo(out, &ci)) {
        ci.bVisible = visible ? TRUE : FALSE;
        SetConsoleCursorInfo(out, &ci);
    }
}

void runRoom() {
    srand((unsigned)time(nullptr));
    getConsoleSizeAndFixBuffer();

    // hide cursor
    showCursor(false);

    Player player(SCREEN_W / 2, SCREEN_H - 6);
    Boss boss(SCREEN_W / 2 - 20, 5);

    std::vector<Platform> platforms = {
        {5, SCREEN_H - 2, 20},
        {35, SCREEN_H - 6, 25},
        {65, SCREEN_H - 10, 10}
    };

    std::vector<Wall> walls = {
        {0, 0, 2, SCREEN_H},               // left wall
        {SCREEN_W - 2, 0, 2, SCREEN_H}     // right wall
    };

    playStartAnimation();
    pressAnyKeyPrompt();
    slideBlackBars(20);
    slideBlackBarsOpen(20);

    // main loop
    while (true) {
        clearBuffer();

        // update
        player.update(platforms, walls);
        boss.update(platforms, walls, player);

        // projectiles collisions: boss -> player
        for (auto& proj : boss.projectiles) {
            if (!proj.active) continue;
            if (intersectsRect(proj.x, proj.y, 1, 1, player.x, player.y, player.w, player.h)) {
                proj.active = false;
                player.health -= 10;

                std::cout << "Player hit! HP: " << player.health << "\n";
            }
        }

        // draw world
        for (auto& p : platforms) p.draw();
        for (auto& w : walls) w.draw();

        // draw entities
        player.draw();
        boss.drawAll();

        // draw simple HUD (player HP)
        std::string hp = "HP: " + std::to_string(player.health);
        for (int i = 0; i < (int)hp.size(); ++i) drawToBuffer(1 + i, 1, hp[i]);

        render();

        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }

    // restore cursor (unreachable here but kept for completeness)
    showCursor(true);
}

// ------------------------ main ------------------------
int main() {
    // ensure console buffer matches
    getConsoleSizeAndFixBuffer();

    srand((unsigned)time(nullptr));
    runRoom();
    return 0;
}
