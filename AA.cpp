// bullethell_console.cpp
#include <bits/stdc++.h>
#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
#else
  #include <termios.h>
  #include <unistd.h>
  #include <sys/select.h>
#endif

using namespace std;
using clock_t = chrono::steady_clock;

const int W = 60;
const int H = 30;
const double FRAME_DT = 1.0 / 30.0; // 30 FPS
const int MAX_BULLETS = 1200;

struct Bullet {
    bool active = false;
    double x=0, y=0;
    double vx=0, vy=0;
    char ch='*';
    void reset(double _x,double _y,double _vx,double _vy,char _ch='*'){
        active=true; x=_x; y=_y; vx=_vx; vy=_vy; ch=_ch;
    }
    void update(double dt){ x += vx*dt; y += vy*dt; if(x< -2 || x>W+2 || y< -2 || y>H+2) active=false; }
};

struct Player {
    double x = W/2.0, y = H-3;
    int hp = 3;
};

static vector<Bullet> bullets;
static Player player;

#ifdef _WIN32
bool kbhit_nonblock(){ return _kbhit(); }
int getch_nonblock(){ if(_kbhit()) return _getch(); return -1; }
void set_conio_terminal_mode(){}
void clear_console(){ cout << "\x1b[2J\x1b[H"; }
#else
struct TermiosSaver {
    termios oldt;
    bool saved=false;
    void save(){
        if(saved) return;
        tcgetattr(STDIN_FILENO, &oldt);
        termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        newt.c_cc[VMIN] = 0;
        newt.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        saved = true;
    }
    void restore(){
        if(!saved) return;
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        saved=false;
    }
    ~TermiosSaver(){ restore(); }
} termSaver;
bool kbhit_nonblock(){
    fd_set set;
    struct timeval tv;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    return select(STDIN_FILENO+1, &set, NULL, NULL, &tv) > 0;
}
int getch_nonblock(){
    char c = 0;
    if(read(STDIN_FILENO, &c, 1) == 1) return (int)c;
    return -1;
}
void clear_console(){ cout << "\x1b[2J\x1b[H"; }
#endif

Bullet* spawn_bullet(){
    for(auto &b : bullets) if(!b.active) return &b;
    return nullptr;
}

void init(){
    bullets.resize(MAX_BULLETS);
#ifndef _WIN32
    termSaver.save();
#endif
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
}

void cleanup(){
#ifndef _WIN32
    termSaver.restore();
#endif
}

void render_frame(vector<string> &screen){
    // clear buffer
    for(auto &row : screen) row.assign(W, ' ');
    // draw bullets
    for(auto &b : bullets) if(b.active){
        int ix = (int)round(b.x);
        int iy = (int)round(b.y);
        if(ix>=0 && ix<W && iy>=0 && iy<H) screen[iy][ix] = b.ch;
    }
    // draw player
    int px = (int)round(player.x);
    int py = (int)round(player.y);
    if(px>=0 && px<W && py>=0 && py<H) screen[py][px] = 'A';
    // HUD
    stringstream ss; ss << " HP: " << player.hp << "  Bullets: ";
    int active = 0; for(auto &b: bullets) if(b.active) ++active;
    ss << active << " (q to quit)";
    string info = ss.str();
    for(int i=0;i<(int)info.size() && i<W;i++) screen[0][i]=info[i];

    // output whole buffer
    cout << "\x1b[H"; // move cursor to home
    for(auto &row : screen) {
        cout << row << '\n';
    }
    cout.flush();
}

void update(double dt, double time){
    // move bullets
    for(auto &b : bullets) if(b.active) b.update(dt);

    // collision: bullets to player (simple)
    for(auto &b : bullets) if(b.active){
        double dx = b.x - player.x;
        double dy = b.y - player.y;
        if(fabs(dx) < 0.6 && fabs(dy) < 0.6){
            b.active = false;
            player.hp -= 1;
        }
    }

    // spawn patterns: spiral + waves
    static double last_spawn = 0;
    if(time - last_spawn > 0.12){ // spawn frequently for "bullet hell"
        last_spawn = time;
        // spawn a few bullets in a rotating spiral from top center
        static double angle_offset = 0;
        angle_offset += 0.12;
        int waveCount = 12;
        for(int i=0;i<waveCount;i++){
            double a = angle_offset + (2*M_PI/waveCount)*i;
            double speed = 8.0 + (i%3);
            Bullet* b = spawn_bullet();
            if(b) b->reset(W/2.0, 2.0, cos(a)*speed, sin(a)*speed, '*');
        }
        // occasional aimed bullets
        if(((int)(time*10))%10 == 0){
            double dx = player.x - W/2.0;
            double dy = player.y - 1.0;
            double a = atan2(dy, dx);
            Bullet* b = spawn_bullet();
            if(b) b->reset(W/2.0, 1.5, cos(a)*6.5, sin(a)*6.5, 'o');
        }
    }

    // simple player boundaries
    if(player.x < 1) player.x = 1;
    if(player.x > W-2) player.x = W-2;
    if(player.y < 1) player.y = 1;
    if(player.y > H-2) player.y = H-2;
}

int main(){
    init();
    clear_console();

    vector<string> screen(H, string(W, ' '));
    auto t0 = clock_t::now();
    double accumulator = 0.0;
    double lastTime = 0.0;
    bool running = true;

    while(running && player.hp > 0){
        auto now = clock_t::now();
        double elapsed = chrono::duration<double>(now - t0).count();
        t0 = now;
        accumulator += elapsed;

        // input (nonblocking)
        int c = -1;
        while(kbhit_nonblock()){
            c = getch_nonblock();
            if(c==-1) break;
            if(c==27){ // ESC / possible arrow sequences
                // ignore special sequences for simplicity
            } else if(c=='q' || c=='Q'){
                running = false; break;
            } else if(c=='w' || c=='W' || c==65){ player.y -= 1; }
            else if(c=='s' || c=='S' || c==66){ player.y += 1; }
            else if(c=='a' || c=='A' || c==68){ player.x -= 1; }
            else if(c=='d' || c=='D' || c==67){ player.x += 1; }
        }

        // fixed-step updates
        static double gameTime = 0.0;
        while(accumulator >= FRAME_DT){
            update(FRAME_DT, gameTime);
            gameTime += FRAME_DT;
            accumulator -= FRAME_DT;
        }

        render_frame(screen);

        // frame cap sleep: small sleep to reduce busy loop
        this_thread::sleep_for(chrono::milliseconds(4));
    }

    clear_console();
    if(player.hp <= 0){
        cout << "Game Over — you were overwhelmed! (press Enter)\n";
    } else {
        cout << "Quit. (press Enter)\n";
    }
    cin.get();
    cleanup();
    return 0;
}
