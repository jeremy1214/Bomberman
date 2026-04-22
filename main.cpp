#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
#include "Bomberman.hpp"
using namespace std;

// 遊戲常數定義
int maxEnemy = 4; // 最大敵人數量
int maxEnemySeeRange = 4; // 敵人視野範圍

int maxWall = 40; // 最大牆壁數量

int bombTimer = 6; // 炸彈計時器初始值

int height = 13; // 地圖高度
int width = 26; // 地圖寬度

// 敵人位置和狀態
int enemyX[4], enemyY[4];
bool enemyAlive[4];

// 基礎地圖
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

// 顯示地圖
string displayGrid[13];

// 移動方向向量 (上, 左, 下, 右)
int dy[4] = {-1, 0, 1, 0};
int dx[4] = {0, -1, 0, 1};

// 玩家位置和狀態
int playerX, playerY;
bool playerAlive = true;

// 門的位置和狀態
int doorX, doorY;
bool doorFound = false;
bool doorActive = false;

// 炸彈位置和狀態
int bombX, bombY;
int bombTimerCounter = 0;
bool bombActive = false;
int bombRange = 1;

// 遊戲計時器
double timer = 100.0;

// 檢查座標是否在地圖範圍內
bool inRange(int x, int y)
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

// 計算絕對值
int abs(int x)
{
    return x < 0 ? -x : x;
}

// 檢查指定位置是否為門
bool isfoundDoor(int x, int y)
{
    return x == doorX && y == doorY;
}

// 計算敵人移動的最佳距離 (越小越好)
int calculateBestMove(int nx, int ny)
{
    if (nx == playerX && ny == playerY)
    {
        return 0; // 如果是玩家位置，返回0 (最佳)
    }
    if (/*question 1*/)
    {
        return -1; // 如果無效，返回-1
    }

    int dist = 0;
    int distX = nx - playerX;
    int distY = ny - playerY;
    dist = distX * distX + distY * distY; // 計算歐幾里得距離平方

    return dist;
}

// 生成門的位置
void generateDoor()
{
    int cnt = 0, pur = rand() % maxWall; // 隨機選擇一個牆壁位置
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (/*question 2*/)
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

// 初始化遊戲
void gameInit()
{
    bombActive = false;
    generateWall(maxWall, baseMap); // 生成牆壁
    generateEnemy(maxEnemy, baseMap); // 生成敵人
    generateDoor(); // 生成門
    int cntEnemy = 0;
    for (int i = 0; i < height; i++)
    {
        displayGrid[i] = baseMap[i]; // 複製地圖
        for (int j = 0; j < width; j++)
        {
            if (baseMap[i][j] == 'B') // 找到玩家初始位置
            {
                playerX = j;
                playerY = i;
                displayGrid[i][j] = ' '; // 清空顯示
            }
            if (baseMap[i][j] == 'E') // 找到敵人初始位置
            {
                enemyX[cntEnemy] = j;
                enemyY[cntEnemy] = i;
                enemyAlive[cntEnemy] = true;
                cntEnemy++;
                displayGrid[i][j] = ' '; // 清空顯示
            }
        }
    }
}

// 放置炸彈
void placeBomb()
{
    if (!bombActive) // 如果沒有活躍炸彈
    {
        bombX = playerX;
        bombY = playerY;
        bombTimerCounter = bombTimer; // 重置計時器
        bombActive = true;
    }
}

// 移動玩家
void movePlayer(int d)
{
    int dir = d;
    if (dir < 0 || dir > 3)
        return; // 無效方向

    int nx = playerX + dx[dir];
    int ny = playerY + dy[dir];

    if (displayGrid[ny][nx] == ' ') // 如果是空地
    {
        playerX = nx;
        playerY = ny;
    }

    if (displayGrid[ny][nx] == 'D' && doorActive) // 如果是門且門已激活
    {
        playerX = nx;
        playerY = ny;
    }
}

// 移動敵人
void moveEnemies()
{
    for (int i = 0; i < maxEnemy; i++)
    {
        if (!enemyAlive[i])
            continue; // 跳過死亡敵人
        int nx = enemyX[i], ny = enemyY[i];

        int distX = abs(enemyX[i] - playerX);
        int distY = abs(enemyY[i] - playerY);
        if (distX * distX + distY * distY <= maxEnemySeeRange * maxEnemySeeRange) // 如果玩家在視野內
        {
            int bestDist = 100000;
            for (int d = 0; d < 4; d++)
            {
                int tx = /*question 3*/ // 計算新位置
                int ty = /*question 3*/
                int dist = calculateBestMove(tx, ty);
                if (dist < bestDist && dist != -1)
                {
                    bestDist = dist;
                    nx = tx;
                    ny = ty;
                }
            }
        }
        else // 隨機移動
        {
            int dir = rand() % 4;
            nx = enemyX[i] + dx[dir];
            ny = enemyY[i] + dy[dir];
        }

        if (nx == playerX && ny == playerY)
            playerAlive = false; // 撞到玩家

        if (displayGrid[ny][nx] == ' ') // 如果是空地
        {
            enemyX[i] = nx;
            enemyY[i] = ny;
        }
    }
}

// 更新門狀態
void updateDoor()
{
    if (/*question 4*/) // 如果條件滿足
    {
        baseMap[doorY][doorX] = 'D'; // 顯示門
    }
}

// 更新顯示地圖
void updateDisplay()
{
    for (int i = 0; i < height; i++)
        displayGrid[i] = baseMap[i]; // 複製基礎地圖

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (displayGrid[i][j] == 'B' || displayGrid[i][j] == 'E')
                displayGrid[i][j] = ' '; // 清空玩家和敵人位置

    if (bombActive)
        displayGrid[bombY][bombX] = 'o'; // 顯示炸彈

    for (int i = 0; i < maxEnemy; i++)
    {
        if (enemyAlive[i])
            displayGrid[enemyY[i]][enemyX[i]] = 'E'; // 顯示敵人
    }

    if (playerAlive)
        displayGrid[playerY][playerX] = 'B'; // 顯示玩家
}

// 繪製遊戲畫面
void draw()
{
    cout << "\033[H"; // 移動游標到左上角
    for (int i = 0; i < height; i++)
    {
        cout << displayGrid[i] << endl; // 輸出地圖
    }
    cout << "Enemies left: ";
    int cnt = 0;
    for (int i = 0; i < maxEnemy; i++)
    {
        if (enemyAlive[i])
            cnt++; // 計算剩餘敵人數
    }
    cout << cnt << endl;
    cout << "Time left: " << timer << endl; // 顯示剩餘時間
}

// 炸彈爆炸
void explode()
{
    int radius = bombRange;
    int fire[1 + 4 * radius][2]; // 火焰位置陣列
    int fireCount = 0;

    fire[0][0] = bombX;
    fire[0][1] = bombY; // 炸彈中心

    for (int d = 0; d < 4; d++) // 四個方向
    {
        for (int r = 1; r <= radius; r++)
        {
            int nx = bombX + dx[d] * r;
            int ny = bombY + dy[d] * r;

            if (!inRange(nx, ny))
                break; // 出界
            if (displayGrid[ny][nx] == '#')
                break; // 撞牆

            fire[/*question 5*/][0] = nx;
            fire[/*question 5*/][1] = ny;
            fireCount++;
            if (displayGrid[ny][nx] == '%')
                break; // 撞軟牆
        }
    }

    for (int i = 0; i < fireCount + 1; i++)
    {
        int x = fire[i][0];
        int y = fire[i][1];
        if (x == playerX && y == playerY)
            playerAlive = false; // 玩家死亡

        for (int j = 0; j < maxEnemy; j++)
        {
            if (/*question 6*/) // 如果敵人在火焰範圍
                enemyAlive[j] = false; // 敵人死亡
        }

        if (displayGrid[y][x] == '%') // 如果是軟牆
        {
            baseMap[y][x] = ' '; // 摧毀牆壁
            if (isfoundDoor(x, y)) // 如果是門
            {
                doorFound = true; // 找到門
            }
        }
        displayGrid[y][x] = '*'; // 顯示火焰
    }

    draw(); // 繪製爆炸畫面
    timer -= updateClock(300); // 延遲

    for (int i = 0; i < fireCount + 1; i++)
    {
        int x = fire[i][0];
        int y = fire[i][1];
        /*question 7*/ // 清空火焰
    }

    bombActive = false; // 炸彈失效
}

// 更新炸彈狀態
void updateBomb()
{
    if (bombActive)
    {
        bombTimerCounter--; // 計時器減一
        displayGrid[bombY][bombX] = 'o'; // 顯示炸彈
        if (/*question 8*/) // 如果計時器到0
            explode(); // 爆炸
    }
}

// 主遊戲循環
void periodic()
{
    while (playerAlive)
    {

        int key = getKey(); // 獲取按鍵
        if (key == -1)
        {
            break; // 退出
        }
        if (/*question 9*/) // 如果按下放置空白鍵
        {
            placeBomb(); // 放置炸彈
        }

        movePlayer(key); // 移動玩家
        updateBomb(); // 更新炸彈
        moveEnemies(); // 移動敵人
        updateDoor(); // 更新門
        updateDisplay(); // 更新顯示
        draw(); // 繪製畫面
        timer -= updateClock(); // 更新時間

        int aliveEnemies = 0;

        for (int i = 0; i < maxEnemy; i++)
        {
            if (enemyAlive[i])
                aliveEnemies++; // 計算剩餘敵人
        }

        if (aliveEnemies == 0 && doorFound) // 如果敵人全滅且找到門
        {
            doorActive = true; // 激活門
        }

        if (/*question 10*/) // 如果玩家到達門，且門可以使用
        {
            cout << "YOU WIN!\n";
            return; // 勝利
        }

        if (timer <= 0) // 如果時間到
        {
            playerAlive = false; // 遊戲結束
        }
    }
}

// 主函數
int main()
{
    srand(time(0)); // 初始化隨機種子
    system("cls"); // 清屏
    cout << "\033[?25l"; // 隱藏游標

    gameReady(); // 遊戲準備
    gameWait(); // 等待開始
    system("cls"); // 清屏

    gameInit(); // 初始化遊戲

    periodic(); // 開始遊戲循環

    if (!playerAlive)
        cout << "GAME OVER\n"; // 遊戲結束

    cout << "\033[?25h"; // 顯示游標
    return 0;
}