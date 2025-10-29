#define NOMINMAX
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <chrono>
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


//=======================================

std::vector<std::string> playerSprite = {
    " ____ ",
    "||  -|",
    "|_||_|",
};

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

struct Entity {
    int x, y; // position
    int vx = 0, vy = 0; // velocity
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
        vy += 1; // gravity
        if (vy > 3) vy = 3;

        int nextX = x + vx;
        int nextY = y + vy;
        onGround = false;

        // horizontal movement + wall collisions
        x = nextX;
        for (auto& wall : walls) {
            if (nextX + this->w > wall.x && nextX < wall.x + wall.w &&
                y + this->h > wall.y && y < wall.y + wall.h) {
            if (vx > 0) x = w.x - this->w;      // hitting wall from the left
            if (vx < 0) x = w.x + w.w;          // hitting wall from the right
            }
        }

        // vertical movement + wall collisions
        y = nextY;
        for (auto& wall : walls) {
            if (x + w > wall.x && x < wall.x + wall.w &&
                nextY + h > wall.y && nextY < wall.y + wall.h) {
            if (vy > 0) { 
                y = wall.y - h; 
                onGround = true; 
            } else if (vy < 0) {
                y = wall.y + wall.h; 
            }
            vy = 0;
    }
}

        // platform collisions (from above)
        for (auto& p : platforms) {
            if (!drop) {
            if (y + h > p.y && y + h <= p.y + 1 && x + w > p.x && x < p.x + p.w && vy >= 0) {
                y = p.y - h;
                vy = 0;
                onGround = true;
                drop = false;
            }
        
        }
    }

        // screen bounds
        if (y > SCREEN_H - h) { y = SCREEN_H - h; vy = 0; onGround = true; }
        if (x < 0) x = 0;
        if (x > SCREEN_W - w) x = SCREEN_W - w;

        vx = 0;
        }

    void update(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {
        applyPhysics(platforms, walls);
    }
};


class Player : public Entity {
    public:
    // Player properties


    Player(int x_, int y_) : Entity(x_, y_, playerSprite) {}

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


class Player2 : public Entity {
public:
    Player2(int x_, int y_) : Entity(x_, y_, playerSprite) {}

    void handleInput() {
        // Reset velocities every frame
        vx = 0;
        vy = 0;

        // Horizontal movement
        if (GetAsyncKeyState(VK_LEFT) & 0x8000)  vx = -2;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) vx = 2;

        // Vertical movement / jump / drop
        if (GetAsyncKeyState(VK_UP) & 0x8000) {
            // Only jump if on the ground
            if (onGround) vy = -4;
        }

        if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
            drop = true; // fall through platforms
        }
    }

    void update(const std::vector<Platform>& platforms, const std::vector<Wall>& walls) {
        handleInput();
        applyPhysics(platforms, walls);
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


int main() {
    std::this_thread::sleep_for(std::chrono::seconds(4))
    getConsoleSizeAndFixBuffer();
    showCursor(false);

    Player2 player(10, SCREEN_H - 5);

    // Platforms scattered from bottom to top
    std::vector<Platform> platforms = {
        {0, SCREEN_H - 2, 80},       // floor
        {5, SCREEN_H - 6, 15},
        {30, SCREEN_H - 10, 20},
        {10, SCREEN_H - 14, 10},
        {50, SCREEN_H - 18, 15},
        {20, SCREEN_H - 22, 10},     // near top
    };

    std::vector<Wall> walls; // empty for now

    while (true) {
        clearBuffer();

        // Update
        player.update(platforms, walls);

        // Draw
        for (auto& p : platforms) p.draw();
        player.draw();

        // Render buffer to console
        render();

        // Frame delay (~50 FPS)
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return 0;
}
