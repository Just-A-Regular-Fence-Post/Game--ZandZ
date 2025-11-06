#define NOMINMAX
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>
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

void getConsoleSizeAndFixBuffer2() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int WIDTH = 80, HEIGHT = 25;

    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        WIDTH = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        HEIGHT = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }

    SCREEN_W = WIDTH;
    SCREEN_H = HEIGHT;
    buffer.assign(WIDTH * HEIGHT, ' ');
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

void render2() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hOut, pos);
    for (int y = 0; y < SCREEN_H; y++) {
        for (int x = 0; x < SCREEN_W; x++)
            std::cout << buffer[y * SCREEN_W + x];
        std::cout << '\n';
    }
}

//=======================================

std::vector<std::string> playerSprite = {
    " ____ ",
    "||  -|",
    "|_||_|",
};

std::vector<std::string> playerSpriteOtherWay {
    " ____ ",
    "|-  ||",
    "|_||_|"
};

std::vector<std::string> bossSprite = {
    " /\\_/\\ ",
    "( o.o )",
    " > ^ < "
};

struct Wall {
    int x, y, w, h;
    Wall(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_) {}
    void draw() const {
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
                drawToBuffer(x + j, y + i, '|');
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

struct Controls {
    int moveLeft;
    int moveRight;
    int jump;
    int drop;
};

Controls queryUserControls() {
    Controls c;

    auto askKey = [](const std::string& action) {
        std::cout << "Press a key for " << action << ": ";
        int key = _getch();
        std::cout << (key == 224 ? "(Arrow key)" : std::string(1, (char)key)) << "\n";
        if (key == 224) key = 224 * 256 + _getch(); // handle arrow keys
        return key;
    };

    c.moveLeft  = askKey("Move Left");
    c.moveRight = askKey("Move Right");
    c.jump      = askKey("Jump");
    c.drop      = askKey("Drop/Down");

    return c;
}

struct Entity {
    int x, y; // position
    int vx = 0;
    float vy{}; // velocity
    int w = 1, h = 1; // width and height
    bool onGround = false; // is the entity on the ground
    bool facingRight = true; // direction the entity is facing
    bool drop = false; // drop through platform

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
            for (int j = 0; j < w; ++j)
                if (sprite[i][j] != ' ')
                    drawToBuffer(x + j, y + i, sprite[i][j]);
    }

   void applyPhysics(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {
    vy += 2; // gravity
    if (vy > 2.9) vy = 2.9; // cap fall speed

    onGround = false;

    // -----------------------------
    //  Vertical movement first
    // -----------------------------
    y += vy;
    for (auto& p : platforms) {
        bool overlappingVertically = (y + h > p.y && y + h < p.y + 3);
        bool overlappingHorizontally = (x + w > p.x && x < p.x + p.w);

        if (overlappingHorizontally) {
            if (overlappingVertically && vy >= 0 && !drop) {
                // Land normally
                y = p.y - h;
                vy = 0;
                onGround = true;
            }
        }

// If the player is now below the platform stop drop
if (y > p.y) 
    drop = false;
}


    for (auto& wall : walls) {
        if (x + w > wall.x && x < wall.x + wall.w &&
            y + h > wall.y && y < wall.y + wall.h) {
            if (vy > 0) { y = wall.y - h; onGround = true; }
            else if (vy < 0) y = wall.y + wall.h;
            vy = 0;
        }
    }

    // -----------------------------
    //  Horizontal movement
    // -----------------------------
    x += vx;
    for (auto& wall : walls) {
        if (x + w > wall.x && x < wall.x + wall.w &&
            y + h > wall.y && y < wall.y + wall.h) {
            if (vx > 0) x = wall.x - w;
            if (vx < 0) x = wall.x + wall.w;
            vx = 0;
        }
    }

    // -----------------------------
    //  Screen bounds
    // -----------------------------
    if (y > SCREEN_H - h) { y = SCREEN_H - h; vy = 0; onGround = true; }
    if (x < 0) x = 0;
    if (x > SCREEN_W - w) x = SCREEN_W - w;
}


    void update(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {
        applyPhysics(platforms, walls);
    }
};

struct Projectile {
    int x, y, vx, w, h;
    bool active;

    Projectile(int x_, int y_, int vx_)
        : x(x_), y(y_), vx(vx_), w(2), h(1), active(true) {}

    void update() {
        x += vx;
        if (x < 0 || x >= SCREEN_W) active = false;
    }

    void draw() const {
        if (active) {
            
            drawToBuffer(x, y, '-');
            //drawToBuffer(x + 1, y, '-');
        }
    }

    bool collidesWith(const Entity& e) const {
        return active &&
               x < e.x + e.w &&
               x + w > e.x &&
               y < e.y + e.h &&
               y + h > e.y;
    }
};

class Player2 : public Entity {
    public:
    // Player properties

    Player2(int x_, int y_) : Entity(x_, y_, playerSprite) {}

    void handleInput() {
        if (!_kbhit()) return;
        int key = _getch();
            // Process key input
        if (key == 224) {
            key = _getch();
            if (key == 75) vx = -2;  // left arrow
            else if (key == 77)  vx = 2; // right arrow
            else if (key == 72)  vy = -2; // up arrow
            else if (key == 80) drop = true;
        }else {
            // other keys
        }
    }

    void update(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {
        handleInput();
        applyPhysics(platforms, walls);
    }

};

class Player : public Entity {
public:
    Player(int x_, int y_) : Entity(x_, y_, playerSprite) {}

    std::vector<Projectile> projectiles;
    float attackCooldown = 0.0f;

    void attack() {
        if (attackCooldown <= 0.0f) {
            int projX = facingRight ? x + w : x - 2;
            int projY = y + h / 2;
            int projVx = facingRight ? 3 : -3;
            projectiles.emplace_back(projX, projY, projVx);
            attackCooldown = 0.05; // seconds between attacks
        }
    }



    void handleInput() {
        // Reset velocities every frame
        vx = 0;

        // Horizontal movement
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            vx = -3;
            facingRight = false;
        }

        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            vx = 3;
            facingRight = true;
        }

        // Vertical movement / jump / drop
        if (GetAsyncKeyState(VK_UP) & 0x8000) {
            // Only jump if on the ground
            if (onGround) vy = -9;
        }

        if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
            drop = true; // fall through platforms
            onGround = false;
            vy = 1;
        }
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            attack();
        }
    }

    void update(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {
        attackCooldown = std::max(0.0f, attackCooldown - 0.033f); 
        handleInput();
        sprite = facingRight ? playerSprite : playerSpriteOtherWay;
        applyPhysics(platforms, walls);

        for (auto& proj : projectiles) proj.update();
        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
                                        [](auto& p){ return !p.active; }),
                        projectiles.end());

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

    
    Boss(int x_, int y_) : Entity(x_, y_, bossSprite) {
        w = (int)bossSprite[0].size();
        h = (int)bossSprite.size();
    }
    
    void takeDamage(float dmg) {
        health -= dmg;
        if (health < 0) health = 0;
    }

    // Conditions
    void updateAI(const Player& player) {
        int playerCenter = player.x + player.w / 2;
        int bossCenter = x + w / 2;
        float distance = abs(playerCenter - bossCenter);
        facingRight = playerCenter > bossCenter;



        if(distance < 10) {
            state = RETREAT;
        }
        else if(distance >= 2 && distance <= 4) {
            state = FIREBREATH;
        }
        else if(distance >= 1 && distance < 2) {
            state = REALLYSTRONGASSPUNCH;
        }
        else if(distance < 1 && cooldown == 0) {
            state = BRUTALCOUNTER;
        }
        else {
            state = IDLE;
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
                // Implement FIREBREATH behavior
                break;
            case SPAWNITONYASSPROJECTILE:
                // Implement SPAWNITONYASSPROJECTILE behavior
                break;
            case BRUTALCOUNTER:
                // Implement BRUTALCOUNTER behavior
                break;
            case REALLYSTRONGASSPUNCH:
                // Implement REALLYSTRONGASSPUNCH behavior
                break;
            case NOLIZARDSALLOWED:
                // Implement NOLIZARDSALLOWED behavior
                break;
            case MORPH:
                // Implement MORPH behavior
                break;


            case FROSTBREATH:
                // Implement FROSTBREATH behavior
                break;
            case AVALANCHE:
                // Implement AVALANCHE behavior
                break;
            case SPAWNITONYOASSPROJECTILEV2:
                // Implement SPAWNITONYOASSPROJECTILEV2 behavior
                break;
            case FROSTPUNCH:
                // Implement FROSTPUNCH behavior
                break;
            case FISSURE:
                // Implement FISSURE behavior
                break;
            case CANTESCAPETHISHAMMER:
                // Implement CANTESCAPETHISHAMMER behavior
                break;
            case THEWHEELOFMISFORTUNE:
                // Implement THEWHEELOFMISFORTUNE behavior
                break;
            case NEVERGONNALETYOUDOWN:
                // Implement NEVERGONNALETYOUDOWN behavior
                break;
            case CASTTHEDIEOFFATE:
                // Implement CASTTHEDIEOFFATE behavior
                break;
            case WHATSINTHECARDS:
                // Implement WHATSINTHECARDS behavior
                break;
            case RETREAT:
                vx = (player.x + player.w/2 < x + w/2) ? 1 : -1;
                break;
            case IDLE:
                if (rand() % 60 == 0) vx = (rand() % 2 ? 1 : -1);
                break;
        }
    }

    void update(const std::vector<Platform>& platforms,
            const std::vector<Wall>& walls,
            const Player& player) 
        {
            updateAI(player);
            executeAction(player, walls); 
            applyPhysics(platforms, walls);

            if (cooldown > 0)
                cooldown -= 0.1f;
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
int frameCount = 0;
int lastW = SCREEN_W, lastH = SCREEN_H;

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


int main() {
    std::this_thread::sleep_for(std::chrono::seconds(4));
    srand((unsigned int)time(nullptr));
    getConsoleSizeAndFixBuffer();
    showCursor(false);

    Player player(10, SCREEN_H - 5);

    Boss boss(SCREEN_W / 2, SCREEN_H - 10);


    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();
    int frames = 0;
    float fps = 0.0f;

    // Platforms scattered from bottom to top
    std::vector<Platform> platforms = {
        {SCREEN_W / 3, SCREEN_H - 2, SCREEN_W / 3},     
        {(SCREEN_W / 3) + 3, SCREEN_H - 6, 15},
        {((SCREEN_W / 3) * 2) - 3, SCREEN_H - 6, 15},
        {SCREEN_W / 2, SCREEN_H - 10, 20},
        {10, SCREEN_H - 14, 10},
        {30, SCREEN_H - 18, 15},
        {20, SCREEN_H - 22, 10},     

        {0, SCREEN_H, SCREEN_W},
        {0, 3, SCREEN_W},
        {SCREEN_W / 5, (SCREEN_H * 4) / 5, 15},
        {(SCREEN_W * 4) / 5, (SCREEN_H * 4) / 5, 15},
    };

    std::vector<Wall> walls = {
        {SCREEN_W / 3, SCREEN_H - 20, 2, 30},
        {(SCREEN_W * 2) / 3, SCREEN_H - 20, 2, 30},
    };

    while (true) {
        resizer();
        clearBuffer();

        // Update
        player.update(platforms, walls);
        boss.update(platforms, walls, player);

        for (auto& proj : player.projectiles) {
        if (proj.collidesWith(boss)) {
            boss.takeDamage(10);
            proj.active = false;
        }
    }


        // Draw
        for (auto& p : platforms) p.draw();
        for (auto& wall : walls) wall.draw();
        for (auto& proj : player.projectiles) proj.draw();
        player.draw();
        boss.draw();

        std::string fpsText = "FPS: " + std::to_string((int)fps);
        for (size_t i = 0; i < fpsText.size() && i < (size_t)SCREEN_W; ++i)
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

    return 0;
}