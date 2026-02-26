#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
using namespace std;


int maxEnemy = 4;
int maxEnemySeeRange = 4;

int maxWall = 40;

double maxGameTime = 100.0;

int bombTimer = 6;

int height = 13;
int width = 26;

struct Enemy {
    int x, y;
    bool alive;
};

int enemyX[4], enemyY[4];
bool enemyAlive[4];

string baseMap[13] = {
    "##########################",
    "#B   #    #    #    #    #",
    "#  #   #    #    #    #  #",
    "#    #    #    #    #    #",
    "#  #   #    #    #    #  #",
    "#    #    #    #    #    #",
    "#  #   #    #    #    #  #",
    "#    #    #    #    #    #",
    "#  #   #    #    #    #  #",
    "#    #    #    #    #    #",
    "#  #   #    #    #    #  #",
    "#    #    #    #    #    #",
    "##########################"
};

string displayGrid[13];

int dy[4] = {-1, 0, 1, 0};
int dx[4] = {0, -1, 0, 1};

int playerX, playerY;
bool playerAlive = true;

int doorX, doorY;
bool doorFound = false;
bool doorActive = false;

vector<Enemy> enemies;

int bombX, bombY;
int bombTimerCounter = 0;
bool bombActive = false;
int bombRange = 1;

double timer = maxGameTime;

bool inRange(int x, int y) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void setRandomPos(int &x, int &y) {
    x = rand() % width;
    y = rand() % height;
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
        if (!inRange(nx, ny) || displayGrid[ny][nx] != ' ')
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
        while((distX<4&&distY<4)||baseMap[y][x]!=' '){
            setRandomPos(x, y);
            distX = abs(x - 1);
            distY = abs(y - 1);
        }
        baseMap[y][x] = 'E';
    }
    
}

void generateWall(int numWall){
    int x=1, y=1;
    for(int i=0; i<numWall; i++){
        while((x<=2&&y<=2)||baseMap[y][x]!=' '){
            setRandomPos(x, y);
        }
        baseMap[y][x] = '%';
    }
}

void generateDoor(){
    int cnt = 0, pur = rand() % maxWall;
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            if(baseMap[i][j] == '%'){
                if(cnt == pur){
                    doorX = j;
                    doorY = i;
                    return;
                }
                cnt++;
            }
        }
    }
}

void gameInit() {
    bombActive = false;
    enemies.clear();
    generateWall(maxWall);
    generateEnemy(maxEnemy);
    generateDoor();
    // int cntEnemy = 0;
    for (int i = 0; i < height; i++) {
        displayGrid[i] = baseMap[i];
        for (int j = 0; j < width; j++) {
            if (baseMap[i][j] == 'B') {
                playerX = j;
                playerY = i;
                displayGrid[i][j] = ' ';
            }
            if (baseMap[i][j] == 'E') {
                enemies.push_back({j, i, true});
                // enemyX[cntEnemy] = j;
                // enemyY[cntEnemy] = i;
                // enemyAlive[cntEnemy] = true;
                // cntEnemy++;
                displayGrid[i][j] = ' ';
            }
        }
    }
}

void placeBomb() {
    if(!bombActive){
        bombX = playerX;
        bombY = playerY;
        bombTimerCounter = bombTimer;
        bombActive = true;
    }
}

void movePlayer(int d) {
    int dir = d;
    if (dir < 0 || dir > 3) return;

    int nx = playerX + dx[dir];
    int ny = playerY + dy[dir];

    if (displayGrid[ny][nx] == ' ')
    {
        playerX = nx;
        playerY = ny;
    }

    if (displayGrid[ny][nx] == 'D' && doorActive) {
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
        if (distX*distX + distY*distY <= maxEnemySeeRange*maxEnemySeeRange) {
            calculateBestMove(nx, ny, e);
        }else{
            int dir = rand() % 4;
            nx = e.x + dx[dir];
            ny = e.y + dy[dir];
        }

        if (nx == playerX && ny == playerY)
            playerAlive = false;

        if (displayGrid[ny][nx] == ' ') {
            e.x = nx;
            e.y = ny;
        }
    }

    // for (int i = 0; i < maxEnemy; i++) {
    //     if (!enemyAlive[i]) continue;
    //    int nx = enemyX[i], ny = enemyY[i];

    //     int distX = abs(enemyX[i] - playerX);
    //     int distY = abs(enemyY[i] - playerY);
    //     if (distX*distX + distY*distY <= maxEnemySeeRange*maxEnemySeeRange) {
    //         calculateBestMove(nx, ny, i);
    //     }else{
    //         int dir = rand() % 4;
    //         nx = enemyX[i] + dx[dir];
    //         ny = enemyY[i] + dy[dir];
    //     }
    
    //     if (nx == playerX && ny == playerY)
    //         playerAlive = false;

    //     if (displayGrid[ny][nx] == ' ') {
    //         enemyX[i] = nx;
    //         enemyY[i] = ny;
    //     }
    // }
}

void updateDoor(){
    if(doorFound && !doorActive){
        baseMap[doorY][doorX] = 'D';
    }
}

void updateDisplay() {
    for (int i = 0; i < height; i++)
        displayGrid[i] = baseMap[i];

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (displayGrid[i][j] == 'B' || displayGrid[i][j] == 'E')
                displayGrid[i][j] = ' ';

    if (bombActive)
        displayGrid[bombY][bombX] = 'o';

    for (auto &e : enemies)
        if (e.alive)
            displayGrid[e.y][e.x] = 'E';

    // for (int i=0; i<maxEnemy; i++) {
    //     if (enemyAlive[i])
    //         displayGrid[enemyY[i]][enemyX[i]] = 'E';
    // }

    if (playerAlive)
        displayGrid[playerY][playerX] = 'B';
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
                return 0;
            case 'a':
            case 75:
                return 1;
            case 's':
            case 80:
                return 2;
            case 'd':
            case 77:
                return 3;
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
    for (int i = 0; i < height; i++) {
        cout << displayGrid[i] << endl;
    }
    cout << "Enemies left: ";
    int cnt = 0;
    for (auto &e : enemies) if (e.alive) cnt++;
    // for (int i=0; i<maxEnemy; i++) {
    //     if (enemyAlive[i]) cnt++;
    // }
    cout << cnt << endl;
    cout << "Time left: " << timer << endl;
}

void explode() {
    int radius = bombRange;
    vector<pair<int,int>> fire;

    fire.push_back({bombX, bombY});

    for (int d = 0; d < 4; d++) {
        for (int r = 1; r <= radius; r++) {
            int nx = bombX + dx[d]*r;
            int ny = bombY + dy[d]*r;
            
            if (!inRange(nx, ny)) break;
            if (displayGrid[ny][nx] == '#') break;

            fire.push_back({nx, ny});
            if (displayGrid[ny][nx] == '%') break;
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

        // for (int i=0; i<maxEnemy; i++) {
        //     if (enemyAlive[i] && enemyX[i] == x && enemyY[i] == y)
        //         enemyAlive[i] = false;
        // }

        if (displayGrid[y][x] == '%'){
            baseMap[y][x] = ' ';
            if(isfoundDoor(x, y)){
                doorFound = true;
            }
        }
        displayGrid[y][x] = '*';
    }

    draw();
    updateClock(300); 

    for (auto &p : fire) {
        int x = p.first;
        int y = p.second;
        displayGrid[y][x] = ' ';
    }

    bombActive = false;
}

void updateBomb() {
    if (bombActive) {
        bombTimerCounter--;
        displayGrid[bombY][bombX] = 'o';
        if (bombTimerCounter <= 0)
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

        // for (int i=0; i<maxEnemy; i++) {
        //     if (enemyAlive[i]) aliveEnemies++;
        // }

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

    gameInit();

    periodic();

    if (!playerAlive)
        cout << "GAME OVER\n";

    cout << "\033[?25h"; //終端機中顯示游標
    return 0;
}