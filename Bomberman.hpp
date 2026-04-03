#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <iostream>
using namespace std;

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

void setRandomPos(int &x, int &y) {
    x = rand() % 26;
    y = rand() % 13;
}

double updateClock(int delta = 150){
    Sleep(delta);
    double timePassed = delta / 1000.0;
    return timePassed;
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