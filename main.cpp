#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
#include "Bomberman.hpp"
using namespace std;

int maxEnemy = 4;
int maxEnemySeeRange = 4;

int maxWall = 40;

double maxGameTime = 100.0;

int bombTimer = 6;

int height = 13;
int width = 26;

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
    "##########################"};

string displayGrid[13];

int dy[4] = {-1, 0, 1, 0};
int dx[4] = {0, -1, 0, 1};

int playerX, playerY;
bool playerAlive = true;

int doorX, doorY;
bool doorFound = false;
bool doorActive = false;

int bombX, bombY;
int bombTimerCounter = 0;
bool bombActive = false;
int bombRange = 1;

double timer = maxGameTime;

bool inRange(int x, int y)
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

int abs(int x)
{
    return x < 0 ? -x : x;
}

bool isfoundDoor(int x, int y)
{
    return x == doorX && y == doorY;
}

int calculateBestMove(int nx, int ny)
{
    if (nx == playerX && ny == playerY)
    {
        return 0;
    }
    if (!inRange(nx, ny) || displayGrid[ny][nx] != ' ')
    {
        return -1;
    }

    int dist = 0;
    int distX = abs(nx - playerX);
    int distY = abs(ny - playerY);
    dist = distX * distX + distY * distY;

    return dist;
}

void generateEnemy(int numEnemy)
{
    int x = 1, y = 1;
    int distX = 0, distY = 0;
    for (int i = 0; i < numEnemy; i++)
    {
        distX = abs(x - 1);
        distY = abs(y - 1);
        while ((distX < 4 && distY < 4) || baseMap[y][x] != ' ')
        {
            setRandomPos(x, y);
            distX = abs(x - 1);
            distY = abs(y - 1);
        }
        baseMap[y][x] = 'E';
    }
}

void generateWall(int numWall)
{
    int x = 1, y = 1;
    for (int i = 0; i < numWall; i++)
    {
        while ((x <= 2 && y <= 2) || baseMap[y][x] != ' ')
        {
            setRandomPos(x, y);
        }
        baseMap[y][x] = '%';
    }
}

void generateDoor()
{
    int cnt = 0, pur = rand() % maxWall;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (baseMap[i][j] == '%')
            {
                if (cnt == pur)
                {
                    doorX = j;
                    doorY = i;
                    return;
                }
                cnt++;
            }
        }
    }
}

void gameInit()
{
    bombActive = false;
    generateWall(maxWall);
    generateEnemy(maxEnemy);
    generateDoor();
    int cntEnemy = 0;
    for (int i = 0; i < height; i++)
    {
        displayGrid[i] = baseMap[i];
        for (int j = 0; j < width; j++)
        {
            if (baseMap[i][j] == 'B')
            {
                playerX = j;
                playerY = i;
                displayGrid[i][j] = ' ';
            }
            if (baseMap[i][j] == 'E')
            {
                enemyX[cntEnemy] = j;
                enemyY[cntEnemy] = i;
                enemyAlive[cntEnemy] = true;
                cntEnemy++;
                displayGrid[i][j] = ' ';
            }
        }
    }
}

void placeBomb()
{
    if (!bombActive)
    {
        bombX = playerX;
        bombY = playerY;
        bombTimerCounter = bombTimer;
        bombActive = true;
    }
}

void movePlayer(int d)
{
    int dir = d;
    if (dir < 0 || dir > 3)
        return;

    int nx = playerX + dx[dir];
    int ny = playerY + dy[dir];

    if (displayGrid[ny][nx] == ' ')
    {
        playerX = nx;
        playerY = ny;
    }

    if (displayGrid[ny][nx] == 'D' && doorActive)
    {
        playerX = nx;
        playerY = ny;
    }
}

void moveEnemies()
{
    for (int i = 0; i < maxEnemy; i++)
    {
        if (!enemyAlive[i])
            continue;
        int nx = enemyX[i], ny = enemyY[i];

        int distX = abs(enemyX[i] - playerX);
        int distY = abs(enemyY[i] - playerY);
        if (distX * distX + distY * distY <= maxEnemySeeRange * maxEnemySeeRange)
        {
            int bestDist = 100000;
            for(int d = 0; d < 4; d++)
            {
                int tx = enemyX[i] + dx[d];
                int ty = enemyY[i] + dy[d];
                int dist = calculateBestMove(tx, ty);
                if (dist < bestDist && dist != -1)
                {
                    bestDist = dist;
                    nx = tx;
                    ny = ty;
                }
            }
        }
        else
        {
            int dir = rand() % 4;
            nx = enemyX[i] + dx[dir];
            ny = enemyY[i] + dy[dir];
        }

        if (nx == playerX && ny == playerY)
            playerAlive = false;

        if (displayGrid[ny][nx] == ' ')
        {
            enemyX[i] = nx;
            enemyY[i] = ny;
        }
    }
}

void updateDoor()
{
    if (doorFound && !doorActive)
    {
        baseMap[doorY][doorX] = 'D';
    }
}

void updateDisplay()
{
    for (int i = 0; i < height; i++)
        displayGrid[i] = baseMap[i];

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (displayGrid[i][j] == 'B' || displayGrid[i][j] == 'E')
                displayGrid[i][j] = ' ';

    if (bombActive)
        displayGrid[bombY][bombX] = 'o';

    for (int i = 0; i < maxEnemy; i++)
    {
        if (enemyAlive[i])
            displayGrid[enemyY[i]][enemyX[i]] = 'E';
    }

    if (playerAlive)
        displayGrid[playerY][playerX] = 'B';
}

void draw()
{
    cout << "\033[H";
    for (int i = 0; i < height; i++)
    {
        cout << displayGrid[i] << endl;
    }
    cout << "Enemies left: ";
    int cnt = 0;
    for (int i = 0; i < maxEnemy; i++)
    {
        if (enemyAlive[i])
            cnt++;
    }
    cout << cnt << endl;
    cout << "Time left: " << timer << endl;
}

void explode()
{
    int radius = bombRange;
    int fire[5][2];
    int fireCount = 0;

    fire[0][0] = bombX;
    fire[0][1] = bombY;

    for (int d = 0; d < 4; d++)
    {
        for (int r = 1; r <= radius; r++)
        {
            int nx = bombX + dx[d] * r;
            int ny = bombY + dy[d] * r;

            if (!inRange(nx, ny))
                break;
            if (displayGrid[ny][nx] == '#')
                break;

            fire[fireCount + 1][0] = nx;
            fire[fireCount + 1][1] = ny;
            fireCount++;
            if (displayGrid[ny][nx] == '%')
                break;
        }
    }

    for (int i = 0; i < fireCount + 1; i++)
    {
        int x = fire[i][0];
        int y = fire[i][1];
        if (x == playerX && y == playerY)
            playerAlive = false;

        for (int i = 0; i < maxEnemy; i++)
        {
            if (enemyAlive[i] && enemyX[i] == x && enemyY[i] == y)
                enemyAlive[i] = false;
        }

        if (displayGrid[y][x] == '%')
        {
            baseMap[y][x] = ' ';
            if (isfoundDoor(x, y))
            {
                doorFound = true;
            }
        }
        displayGrid[y][x] = '*';
    }

    draw();
    timer -= updateClock(300);

    for (int i = 0; i < fireCount + 1; i++)
    {
        int x = fire[i][0];
        int y = fire[i][1];
        displayGrid[y][x] = ' ';
    }

    bombActive = false;
}

void updateBomb()
{
    if (bombActive)
    {
        bombTimerCounter--;
        displayGrid[bombY][bombX] = 'o';
        if (bombTimerCounter <= 0)
            explode();
    }
}

void periodic()
{
    while (playerAlive)
    {

        int key = getKey();
        if (key == -1)
            break;
        if (key == 4)
            placeBomb();
        movePlayer(key);
        updateBomb();
        moveEnemies();
        updateDoor();
        updateDisplay();
        draw();
        timer -= updateClock();

        int aliveEnemies = 0;

        for (int i = 0; i < maxEnemy; i++)
        {
            if (enemyAlive[i])
                aliveEnemies++;
        }

        if (aliveEnemies == 0 && doorFound)
        {
            doorActive = true;
        }

        if (doorActive && playerX == doorX && playerY == doorY)
        {
            cout << "YOU WIN!\n";
            return;
        }

        if (timer <= 0)
        {
            playerAlive = false;
        }
    }
}

int main()
{
    srand(time(0));
    system("cls");
    cout << "\033[?25l"; // 終端機中隱藏游標

    gameReady();
    gameWait();
    system("cls");

    gameInit();

    periodic();

    if (!playerAlive)
        cout << "GAME OVER\n";

    cout << "\033[?25h"; // 終端機中顯示游標
    return 0;
}