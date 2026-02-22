#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
using namespace std;

#define Height 15
#define Width 31
#define MAX_ENEMY 4
#define MAX_WALL 50
#define MAX_ENEMY_SEE_RANGE 4
#define MAX_GAME_TIME 100.0
#define BOMB_TIMER 6

struct Bomb {
    int x, y;
    int timer;
    bool active;
};

struct Enemy {
    int x, y;
    bool alive;
};

string baseMap[Height] = {
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

string displayGrid[Height];

int dx[4] = {0, 1, 0, -1};
int dy[4] = {1, 0, -1, 0};

int playerX, playerY;
bool playerAlive = true;

int doorX, doorY;
bool doorFound = false;
bool doorActive = false;

vector<Enemy> enemies;
Bomb bomb;
int bombRange = 1;

double timer = MAX_GAME_TIME;

bool inRange(int x, int y) {
    return x >= 0 && x < Height && y >= 0 && y < Width;
}

void setRandomPos(int &x, int &y) {
    x = rand() % Height;
    y = rand() % Width;
}

bool isfoundDoor(int x, int y) {
    return x == doorX && y == doorY;
}

void updateClock(int delta = 150){
    Sleep(delta);
    double timePassed = delta / 1000.0;
    timer -= timePassed;
}

void calculateBestMove(int &bestX, int &bestY, Enemy e) {
    int bestDist = 1e9;
    for (int d = 0; d < 4; d++) {
        int nx = e.x + dx[d];
        int ny = e.y + dy[d];
        if (nx == playerX && ny == playerY){
            bestX = nx;
            bestY = ny;
            return;
        }
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

void generateEnemy(int numEnemy){
    int x=1, y=1;
    int distX = 0, distY = 0;
    for(int i=0; i<numEnemy; i++){
        distX = abs(x - 1);
        distY = abs(y - 1);
        while((distX<2||distY<2)||baseMap[x][y]!=' '){
            setRandomPos(x, y);
            distX = abs(x - 1);
            distY = abs(y - 1);
        }
        baseMap[x][y] = 'E';
    }
    
}

void generateWall(int numWall){
    int x=1, y=1;
    for(int i=0; i<numWall; i++){
        while((x==1&&y==1)||(x==2&&y==1)||(x==1&&y==2)||baseMap[x][y]!=' '){
            setRandomPos(x, y);
        }
        baseMap[x][y] = '%';
    }
}

void generateDoor(){
    int cnt = 0, pur = rand() % MAX_WALL;
    for(int i=0; i<Height; i++){
        for(int j=0; j<Width; j++){
            if(baseMap[i][j] == '%'){
                if(cnt == pur){
                    doorX = i;
                    doorY = j;
                    return;
                }
                cnt++;
            }
        }
    }
}

void initGame() {
    bomb.active = false;
    enemies.clear();
    generateWall(MAX_WALL);
    generateEnemy(MAX_ENEMY);
    generateDoor();
    for (int i = 0; i < Height; i++) {
        displayGrid[i] = baseMap[i];
        for (int j = 0; j < Width; j++) {
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

void placeBomb() {
    if (!bomb.active) {
        bomb = {playerX, playerY, BOMB_TIMER, true};
    }
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

    if (displayGrid[nx][ny] == 'D' && doorActive) {
        playerX = nx;
        playerY = ny;
    }
}

void moveEnemies() {
    for (auto &e : enemies) {
        if (!e.alive) continue;

        int nx = e.x, ny = e.y;

        int distX = abs(e.x - playerX);
        int distY = abs(e.y - playerY);
        if (distX*distX + distY*distY <= MAX_ENEMY_SEE_RANGE*MAX_ENEMY_SEE_RANGE) {
            calculateBestMove(nx, ny, e);
        }else{
            int dir = rand() % 4;
            nx = e.x + dx[dir];
            ny = e.y + dy[dir];
        }

        if (nx == playerX && ny == playerY)
            playerAlive = false;

        if (displayGrid[nx][ny] == ' ') {
            e.x = nx;
            e.y = ny;
        }

        
    }
}

void updateDoor(){
    if(doorFound && !doorActive){
        baseMap[doorX][doorY] = 'D';
    }
}

void updateDisplay() {
    for (int i = 0; i < Height; i++)
        displayGrid[i] = baseMap[i];

    for (int i = 0; i < Height; i++)
        for (int j = 0; j < Width; j++)
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

int getKey() {
    if (_kbhit()) {
        int ch = _getch();
        if (ch == 0 || ch == 0xE0) { // 方向鍵或功能鍵
            ch = _getch();
        }else{
            ch = int(tolower(ch));
        }
        switch(ch) {
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

void draw() {
    cout << "\033[H";
    for (int i = 0; i < Height; i++) {
        cout << displayGrid[i] << endl;
    }
    cout << "Enemies left: ";
    int cnt = 0;
    for (auto &e : enemies) if (e.alive) cnt++;
    cout << cnt << endl;
    cout << "Time left: " << timer << endl;
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
            if(isfoundDoor(x, y)){
                doorFound = true;
            }
        }
        displayGrid[x][y] = '*';
    }

    draw();
    updateClock(300); 

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

void gameReady() {
    cout << "Welcome to Bomberman!\n";
    cout << "Use WASD or arrow keys to move, SPACE to place bomb, Q to quit.\n";
    cout << "Press any key to start...\n";
    _getch();
}

void gameWait() {
    for (int i = 3; i > 0; i--) {
         cout << "\033[3;1H";   // 游標移動到第3行第1列
        cout << "Starting in " << i << "...\n";
        Sleep(1000);
    }
}

void periodic(){
    while (playerAlive) {

        int key = getKey();
        if (key == -1) break;
        if (key == 4) placeBomb();
        movePlayer(key);
        updateBomb();
        moveEnemies();
        updateDoor();
        updateDisplay();
        draw();
        updateClock();

        int aliveEnemies = 0;
        for (auto &e : enemies) if (e.alive) aliveEnemies++;

        if (aliveEnemies == 0 && doorFound) {
            doorActive = true;
        }

        if (doorActive && playerX == doorX && playerY == doorY) {
            cout << "YOU WIN!\n";
            return;
        }
        
        if(timer <= 0){
            playerAlive = false;
        }
    }
}

int main() {
    srand(time(0));
    system("cls");
    cout << "\033[?25l"; //終端機中隱藏游標

    gameReady();
    gameWait();
    system("cls");

    initGame();

    periodic();

    if (!playerAlive)
        cout << "GAME OVER\n";

    cout << "\033[?25h"; //終端機中顯示游標
    return 0;
}