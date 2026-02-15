#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
using namespace std;

#define H 15
#define W 31
#define MAX_ENEMY 4
#define MAX_WALL 25

struct Bomb {
    int x, y;
    int timer;
    bool active;
};

struct Enemy {
    int x, y;
    bool alive;
};

struct Prize {
    int x, y;
    bool finded = false;
    bool used = false;
};

string baseMap[H] = {
    "###############################",
    "#B    #     #     #     #     #",
    "#  #     #     #     #     #  #",
    "#     #     #     #     #     #",
    "#  #     #     #     #     #  #",
    "#     #     #     #     #     #",
    "#  #     #     #     #     #  #",
    "#     #     #     #     #     #",
    "#  #     #     #     #     #  #",
    "#     #     #     #     #     #",
    "#  #     #     #     #     #  #",
    "#     #     #     #     #     #",
    "#  #     #     #     #     #  #",
    "#     #     #     #     #     #",
    "###############################"
};

string displayGrid[H];

int dx[4] = {0, 1, 0, -1};
int dy[4] = {1, 0, -1, 0};

int playerX, playerY;
bool playerAlive = true;

vector<Enemy> enemies;
Bomb bomb;
int bombRange = 1;
int enemySeeRange = 4;

bool inRange(int x, int y) {
    return x >= 0 && x < H && y >= 0 && y < W;
}

void setRandomPos(int &x, int &y) {
    x = rand() % H;
    y = rand() % W;
}

void generateEnemy(int numEnemy){
    int x=1, y=1;
    for(int i=0; i<numEnemy; i++){
        while((x==1&&y==1)||baseMap[x][y]!=' '){
            setRandomPos(x, y);
        }
        baseMap[x][y] = 'E';
    }
    
}

void generateBrokenWall(int numWall){
    int x=1, y=1;
    for(int i=0; i<numWall; i++){
        while((x==1&&y==1)||(x==2&&y==1)||(x==1&&y==2)||baseMap[x][y]!=' '){
            setRandomPos(x, y);
        }
        baseMap[x][y] = '%';
    }
}

void initGame() {
    bomb.active = false;
    enemies.clear();
    generateEnemy(MAX_ENEMY);
    generateBrokenWall(MAX_WALL);
    for (int i = 0; i < H; i++) {
        displayGrid[i] = baseMap[i];
        for (int j = 0; j < W; j++) {
            if (baseMap[i][j] == 'B') {
                playerX = i;
                playerY = j;
                displayGrid[i][j] = ' ';
            }
            if (baseMap[i][j] == 'E') {
                enemies.push_back({i, j, true});
                displayGrid[i][j] = ' ';
            }
        }
    }
}

void draw() {
    cout << "\033[H";
    for (int i = 0; i < H; i++) {
        cout << displayGrid[i] << endl;
    }
    cout << "Enemies left: ";
    int cnt = 0;
    for (auto &e : enemies) if (e.alive) cnt++;
    cout << cnt << endl;
}

void placeBomb() {
    if (!bomb.active) {
        bomb = {playerX, playerY, 6, true};
    }
}

void explode() {
    int radius = bombRange;
    vector<pair<int,int>> fire;

    fire.push_back({bomb.x, bomb.y});

    for (int d = 0; d < 4; d++) {
        for (int r = 1; r <= radius; r++) {
            int nx = bomb.x + dx[d]*r;
            int ny = bomb.y + dy[d]*r;
            if (!inRange(nx, ny)) break;
            if (displayGrid[nx][ny] == '#') break;

            fire.push_back({nx, ny});
            if (displayGrid[nx][ny] == '%') break;
        }
    }

    for (auto &p : fire) {
        int x = p.first;
        int y = p.second;

        if (x == playerX && y == playerY)
            playerAlive = false;

        for (auto &e : enemies) {
            if (e.alive && e.x == x && e.y == y)
                e.alive = false;
        }

        if (displayGrid[x][y] == '%'){
            baseMap[x][y] = ' ';
        }
        displayGrid[x][y] = '*';
    }

    draw();
    Sleep(300);

    for (auto &p : fire) {
        int x = p.first;
        int y = p.second;
        displayGrid[x][y] = ' ';
    }

    bomb.active = false;
}

void updateBomb() {
    if (bomb.active) {
        bomb.timer--;
        displayGrid[bomb.x][bomb.y] = 'o';
        if (bomb.timer <= 0)
            explode();
    }
}

void calculateBestMove(int &bestX, int &bestY, Enemy &e) {
    int bestDist = 1e9;
    for (int d = 0; d < 4; d++) {
        int nx = e.x + dx[d];
        int ny = e.y + dy[d];
        if (!inRange(nx, ny) || displayGrid[nx][ny] != ' ')
            continue;

        int dist = 0;
        int distX = abs(nx - playerX);
        int distY = abs(ny - playerY);
        dist = distX*distX + distY*distY;
        if (dist < bestDist) {
            bestDist = dist;
            bestX = nx;
            bestY = ny;
        }
    }
}

void moveEnemies() {
    for (auto &e : enemies) {
        if (!e.alive) continue;

        int nx = e.x, ny = e.y;

        int distX = abs(e.x - playerX);
        int distY = abs(e.y - playerY);
        if (distX*distX + distY*distY <= enemySeeRange*enemySeeRange) {
            calculateBestMove(nx, ny, e);
        }else{
            int dir = rand() % 4;
            nx = e.x + dx[dir];
            ny = e.y + dy[dir];
        }
        if (displayGrid[nx][ny] == ' ') {
            e.x = nx;
            e.y = ny;
        }

        if (e.x == playerX && e.y == playerY)
            playerAlive = false;
    }
}

void updateDisplay() {
    for (int i = 0; i < H; i++)
        displayGrid[i] = baseMap[i];

    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            if (displayGrid[i][j] == 'B' || displayGrid[i][j] == 'E')
                displayGrid[i][j] = ' ';

    if (bomb.active)
        displayGrid[bomb.x][bomb.y] = 'o';

    for (auto &e : enemies)
        if (e.alive)
            displayGrid[e.x][e.y] = 'E';

    if (playerAlive)
        displayGrid[playerX][playerY] = 'B';
}

void movePlayer(int d) {
    int dir = d;
    if (dir < 0 || dir > 3) return;

    int nx = playerX + dx[dir];
    int ny = playerY + dy[dir];

    if (displayGrid[nx][ny] == ' ')
    {
        playerX = nx;
        playerY = ny;
    }
}

int getKey() {
    if (_kbhit()) {
        char c  = tolower(_getch());
        int code = (int)c;
        switch(code){
            case 'w':
            case 72:
                return 3;
            case 'a':
            case 75:
                return 2;
            case 's':
            case 80:
                return 1;
            case 'd':
            case 77:
                return 0;
            case ' ':
                return 4;
            case 'q':
                return -1;
            default:
                return -2;
        }
    }
    return -2;
}

int main() {
    srand(time(0));
    system("cls");
    cout << "\033[?25l";

    initGame();

    bool running = true;

    while (running && playerAlive) {

        int key = getKey();
        if (key == -1) break;
        if (key == 4) placeBomb();
        movePlayer(key);

        updateBomb();
        moveEnemies();
        updateDisplay();
        draw();

        int aliveEnemies = 0;
        for (auto &e : enemies) if (e.alive) aliveEnemies++;

        if (aliveEnemies == 0) {
            cout << "YOU WIN!\n";
            break;
        }

        Sleep(150);
    }

    if (!playerAlive)
        cout << "GAME OVER\n";

    cout << "\033[?25h";
    return 0;
}
