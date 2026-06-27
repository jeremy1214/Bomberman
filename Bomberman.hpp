#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
using namespace std;

// Get keyboard input, return direction or action code
int getKey() {
    if (_kbhit()) {
        int ch = _getch();
        if (ch == 0 || ch == 0xE0) { // Arrow key or function key
            ch = _getch();
        }else{
            ch = int(tolower(ch));
        }
        switch(ch) {
            case 'w':
            case 72:
                return 0; // Up
            case 'a':
            case 75:
                return 1; // Left
            case 's':
            case 80:
                return 2; // Down
            case 'd':
            case 77:
                return 3; // Right
            case ' ':
                return 4; // Place bomb
            case 'q':
                return -1; // Quit
            default:
                return -2; // Invalid key
        }
    }
    return -2; // No key pressed
}

// Calculate absolute value
int abs(int x)
{
    return x < 0 ? -x : x;
}

// Set random position
void setRandomPos(int &x, int &y) {
    x = rand() % 26;
    y = rand() % 13;
}

// Update clock, delay and return elapsed time
double updateClock(int delta = 150){
    Sleep(delta);
    double timePassed = delta / 1000.0;
    return timePassed;
}

// Game ready screen
void gameReady() {
    cout << "Welcome to Bomberman!\n";
    cout << "Use WASD or arrow keys to move, SPACE to place bomb, Q to quit.\n";
    cout << "Press any key to start...\n";
    _getch();
}

// Game countdown wait
void gameWait() {
    for (int i = 3; i > 0; i--) {
         cout << "\033[3;1H";   // Move cursor to row 3, column 1
        cout << "Starting in " << i << "...\n";
        Sleep(1000);
    }
}

// Generate enemies
void generateEnemy(int numEnemy, string baseMap[])
{
    int x = 1, y = 1;
    int distX = 0, distY = 0;
    for (int i = 0; i < numEnemy; i++)
    {
        distX = abs(x - 1);
        distY = abs(y - 1);
        while ((distX < 4 && distY < 4) || baseMap[y][x] != ' ') // Ensure not near the player's starting area and position is empty
        {
            setRandomPos(x, y);
            distX = abs(x - 1);
            distY = abs(y - 1);
        }
        baseMap[y][x] = 'E'; // Place enemy
    }
}

// Generate walls
void generateWall(int numWall, string baseMap[])
{
    int x = 1, y = 1;
    for (int i = 0; i < numWall; i++)
    {
        while ((x <= 2 && y <= 2) || baseMap[y][x] != ' ') // Ensure not near the starting area and position is empty
        {
            setRandomPos(x, y);
        }
        baseMap[y][x] = '%'; // Place wall
    }
}

// Print colored character
void printColoredChar(char ch)
{
    string reset = "\033[0m";      // Reset color
    string blue = "\033[94m";      // Bright blue (solid wall)
    string magenta = "\033[95m";   // Bright magenta (breakable wall)
    string green = "\033[92m";     // Bright green (player)
    string red = "\033[91m";       // Bright red (enemy)
    string yellow = "\033[93m";    // Bright yellow (bomb)
    string cyan = "\033[96m";      // Bright cyan (door)
    string orange = "\033[38;5;208m"; // Orange (fire)
    
    switch(ch)
    {
        case '#':  // Solid wall
            cout << blue << ch << reset;
            break;
        case '%':  // Breakable wall
            cout << magenta << ch << reset;
            break;
        case 'B':  // Player
            cout << green << ch << reset;
            break;
        case 'E':  // Enemy
            cout << red << ch << reset;
            break;
        case 'o':  // Bomb
            cout << yellow << ch << reset;
            break;
        case 'D':  // Door
            cout << cyan << ch << reset;
            break;
        case '*':  // Flame
            cout << orange << ch << reset;
            break;
        default:   // Space and other characters
            cout << ch;
            break;
    }
}

// Initialize console window
void windowSetup() {
    // // Enable virtual terminal processing to support ANSI escape codes
    // HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    // DWORD dwMode = 0;
    // GetConsoleMode(hOut, &dwMode);
    // dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    // SetConsoleMode(hOut, dwMode);

    srand(time(0)); // Initialize random seed
    system("cls"); // Clear screen
    cout << "\033[?25l"; // Hide cursor
}

// Cleanup screen
void cleanup() {
    system("cls"); // Clear screen
}

// Show cursor
void showCursor() {
    cout << "\033[?25h"; // Show cursor
}
