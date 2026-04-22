#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <iostream>
using namespace std;

// 獲取鍵盤輸入，返回方向或動作代碼
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
                return 0; // 上
            case 'a':
            case 75:
                return 1; // 左
            case 's':
            case 80:
                return 2; // 下
            case 'd':
            case 77:
                return 3; // 右
            case ' ':
                return 4; // 放置炸彈
            case 'q':
                return -1; // 退出
            default:
                return -2; // 無效鍵
        }
    }
    return -2; // 無按鍵
}

// 設置隨機位置
void setRandomPos(int &x, int &y) {
    x = rand() % 26;
    y = rand() % 13;
}

// 更新時鐘，延遲並返回經過時間
double updateClock(int delta = 150){
    Sleep(delta);
    double timePassed = delta / 1000.0;
    return timePassed;
}

// 遊戲準備畫面
void gameReady() {
    cout << "Welcome to Bomberman!\n";
    cout << "Use WASD or arrow keys to move, SPACE to place bomb, Q to quit.\n";
    cout << "Press any key to start...\n";
    _getch();
}

// 遊戲倒計時等待
void gameWait() {
    for (int i = 3; i > 0; i--) {
         cout << "\033[3;1H";   // 游標移動到第3行第1列
        cout << "Starting in " << i << "...\n";
        Sleep(1000);
    }
}

// 生成敵人
void generateEnemy(int numEnemy, string baseMap[])
{
    int x = 1, y = 1;
    int distX = 0, distY = 0;
    for (int i = 0; i < numEnemy; i++)
    {
        distX = abs(x - 1);
        distY = abs(y - 1);
        while ((distX < 4 && distY < 4) || baseMap[y][x] != ' ') // 確保不靠近玩家起始點且位置為空
        {
            setRandomPos(x, y);
            distX = abs(x - 1);
            distY = abs(y - 1);
        }
        baseMap[y][x] = 'E'; // 放置敵人
    }
}

// 生成牆壁
void generateWall(int numWall, string baseMap[])
{
    int x = 1, y = 1;
    for (int i = 0; i < numWall; i++)
    {
        while ((x <= 2 && y <= 2) || baseMap[y][x] != ' ') // 確保不靠近起始區域且位置為空
        {
            setRandomPos(x, y);
        }
        baseMap[y][x] = '%'; // 放置牆壁
    }
}