#include "Bomberman.hpp"
using namespace std;

// Game constant definitions
int maxEnemy = 4; // Maximum number of enemies
int maxEnemySeeRange = 4; // Enemy vision range

int maxWall = 40; // Maximum number of walls

int bombTimer = 6; // Initial bomb timer value

int height = 13; // Map height
int width = 26; // Map width

// Enemy positions and states
int enemyX[4], enemyY[4];
bool enemyAlive[4];

// Base map
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

// Display map
string displayGrid[13];

// Movement direction vectors (up, left, down, right)
int dy[4] = {-1, 0, 1, 0};
int dx[4] = {0, -1, 0, 1};

// Player position and state
int playerX, playerY;
bool playerAlive = true;

// Door position and state
int doorX, doorY;
bool doorFound = false;
bool doorActive = false;

// Bomb position and state
int bombX, bombY;
int bombTimerCounter = 0;
bool bombActive = false;
int bombRange = 1;

// Game timer
double timer = 100.0;

// Enemy move timer (used to slow enemies)
int enemyMoveCounter = 0;
int enemyMoveInterval = 2; // Enemies move every 2 game cycles (adjust this value to change enemy speed)

// Check if coordinates are within the map range
bool inRange(int x, int y)
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

// Check if the specified location is the door
bool isfoundDoor(int x, int y)
{
    return x == doorX && y == doorY;
}

// Calculate the best move distance for the enemy (smaller is better)
int calculateBestMove(int nx, int ny)
{
    if (nx == playerX && ny == playerY)
    {
        return 0; // If it's the player's position, return 0 (best)
    }
    if (/*question 1*/)
    {
        return -1; // If invalid, return -1
    }

    int dist = 0;
    int distX = nx - playerX;
    int distY = ny - playerY;
    dist = distX * distX + distY * distY; // Calculate squared Euclidean distance

    return dist;
}

// Generate door position
void generateDoor()
{
    int cnt = 0, pur = rand() % maxWall; // Randomly select a wall position
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

// Initialize game
void gameInit()
{
    bombActive = false;
    generateWall(maxWall, baseMap); // Generate walls
    generateEnemy(maxEnemy, baseMap); // Generate enemies
    generateDoor(); // Generate door
    int cntEnemy = 0;
    for (int i = 0; i < height; i++)
    {
        displayGrid[i] = baseMap[i]; // Copy map
        for (int j = 0; j < width; j++)
        {
            if (baseMap[i][j] == 'B') // Find player's initial position
            {
                playerX = /*question 3*/;
                playerY = /*question 3*/;
                displayGrid[i][j] = ' '; // Clear display
            }
            if (baseMap[i][j] == 'E') // Find enemy's initial position
            {
                enemyX[cntEnemy] = /*question 3*/;
                enemyY[cntEnemy] = /*question 3*/;
                enemyAlive[cntEnemy] = true;
                cntEnemy++;
                displayGrid[i][j] = ' '; // Clear display
            }
        }
    }
}

// Place bomb
void placeBomb()
{
    if (!bombActive) // If there is no active bomb
    {
        bombX = playerX;
        bombY = playerY;
        bombTimerCounter = bombTimer; // Reset timer
        bombActive = true;
    }
}

// Move player
void movePlayer(int d)
{
    int dir = d;
    if (dir < 0 || dir > 3)
        return; // Invalid direction

    int nx = playerX + dx[dir];
    int ny = playerY + dy[dir];

    if (displayGrid[ny][nx] == ' ') // If it's empty space
    {
        playerX = nx;
        playerY = ny;
    }

    if (displayGrid[ny][nx] == 'D' && doorActive) // If it's the door and it's active
    {
        playerX = nx;
        playerY = ny;
    }
}

// Move enemies
void moveEnemies()
{
    for (int i = 0; i < maxEnemy; i++)
    {
        if (!enemyAlive[i])
            continue; // Skip dead enemies
        int nx = enemyX[i], ny = enemyY[i];

        int distX = abs(enemyX[i] - playerX);
        int distY = abs(enemyY[i] - playerY);
        if (distX * distX + distY * distY <= maxEnemySeeRange * maxEnemySeeRange) // If the player is within sight
        {
            int bestDist = INT_MAX;
            for (int d = 0; d < 4; d++)
            {
                int tx = /*question 4*/;
                int ty = /*question 4*/;
                int dist = calculateBestMove(tx, ty);
                if (dist < bestDist && dist != -1)
                {
                    bestDist = dist;
                    nx = tx;
                    ny = ty;
                }
            }
        }
        else // Random movement
        {
            int dir = rand() % 4;
            nx = enemyX[i] + dx[dir];
            ny = enemyY[i] + dy[dir];
        }

        if (nx == playerX && ny == playerY)
            playerAlive = false; // Collide with player

        if (displayGrid[ny][nx] == ' ') // If it's empty space
        {
            enemyX[i] = nx;
            enemyY[i] = ny;
        }
    }
}

// Update door state
void updateDoor()
{
    if (/*question 5*/) // If condition is met
    {
        baseMap[doorY][doorX] = 'D'; // Display door
    }
}

// Update display map
void updateDisplay()
{
    for (int i = 0; i < height; i++)
        displayGrid[i] = baseMap[i]; // Copy Base map

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (displayGrid[i][j] == 'B' || displayGrid[i][j] == 'E')
                displayGrid[i][j] = ' '; // Clear player and enemy positions

    if (bombActive)
        displayGrid[bombY][bombX] = 'o'; // Display bomb

    for (int i = 0; i < maxEnemy; i++)
    {
        if (enemyAlive[i])
            displayGrid[enemyY[i]][enemyX[i]] = 'E'; // Display enemies
    }

    if (playerAlive)
        displayGrid[playerY][playerX] = 'B'; // Display player
}


// Draw game screen
void draw()
{
    cout << "\033[H"; // move cursor to top-left
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printColoredChar(displayGrid[i][j]);
        }
        cout << endl; 
    }
    cout << "Enemies left: ";
    int cnt = 0;
    for (int i = 0; i < maxEnemy; i++)
    {
        if (enemyAlive[i])
            cnt++; // Count alive enemies
    }
    cout << cnt << endl;
    cout << "Time left: " << timer << endl; // Display remaining time
}

// Bomb explosion logic
void explode()
{
    int radius = bombRange;
    int fire[1 + 4 * radius][2]; // Flame positions (center + 4 directions * radius)
    int fireCount = 0;

    fire[0][0] = bombX;
    fire[0][1] = bombY; // Bomb position
    fireCount++;

    for (int d = 0; d < 4; d++) // Four directions
    {
        for (int r = 1; r <= radius; r++)
        {
            int nx = bombX + dx[d] * r;
            int ny = bombY + dy[d] * r;

            if (!inRange(nx, ny))
                break; // Out of range
            if (displayGrid[ny][nx] == '#')
                break; // Hit a solid wall

            fire[fireCount][0] = nx;
            fire[fireCount][1] = ny;
            fireCount++;
            if (displayGrid[ny][nx] == '%')
                break; // Hit a breakable wall
        }
    }

    for (int i = 0; i < fireCount; i++)
    {
        int x = fire[i][0];
        int y = fire[i][1];
        if (x == playerX && y == playerY)
            playerAlive = false; // Player dies if in flame range

        for (int j = 0; j < maxEnemy; j++)
        {
            if (/*question 6*/) // If Enemy is in flame range
                enemyAlive[j] = false; // Enemy dies
        }

        if (displayGrid[y][x] == '%') // If it's a breakable wall
        {
            baseMap[y][x] = ' '; // Destroy the wall
            if (isfoundDoor(x, y)) // If it's a door
            {
                doorFound = true; // Found the door
            }
        }
        displayGrid[y][x] = '*'; // Display flame
    }

    draw(); // Draw explosion screen
    timer -= updateClock(300); // Delay for explosion effect

    for (int i = 0; i < fireCount; i++)
    {
        int x = fire[i][0];
        int y = fire[i][1];
        /*question 7*/ // Clear flame
    }

    bombActive = false; // Bomb is no longer active
}

// Update bomb state
void updateBomb()
{
    if (bombActive)
    {
        bombTimerCounter--; // Decrease bomb timer
        displayGrid[bombY][bombX] = 'o'; // Display bomb
        if (/*question 8*/) // If timer runs out 
            explode(); 
    }
}

// Main game loop
void periodic()
{
    while (playerAlive)
    {

        int key = getKey(); // Get player input
        if (key == -1)
        {
            break; // Quit
        }
        if (/*question 9*/) // If Down key is pressed
        {
            placeBomb(); // Place bomb
        }

        movePlayer(key); // Move player
        updateBomb(); // Update bomb state

        // Every enemyMoveInterval game cycles, move enemies once
        enemyMoveCounter++;
        if (enemyMoveCounter >= enemyMoveInterval) {
            moveEnemies(); // Move enemies
            enemyMoveCounter = 0;
        }
        
        updateDoor(); // Update door
        updateDisplay(); // Update display
        draw(); // Draw screen
        timer -= updateClock(); // Update timer

        int aliveEnemies = 0;

        for (int i = 0; i < maxEnemy; i++)
        {
            if (enemyAlive[i])
                aliveEnemies++; // Calculate remaining enemies
        }

        if (aliveEnemies == 0 && doorFound) // If all enemies are eliminated and the door is found
        {
            doorActive = true; // Activate the door
        }

        if (/*question 10*/) // If the player reaches the door and it's active
        {
            cout << "YOU WIN!\n";
            return; 
        }

        if (timer <= 0) // If time runs out
        {
            playerAlive = false; // Game over
        }
    }
}

// Main function
int main()
{
    // Initialize console
    windowSetup();
    
    gameReady(); // Game ready
    gameWait(); // Game wait
    cleanup(); // Cleanup screen

    gameInit(); // Initialize game

    periodic(); // Start game loop

    if (!playerAlive)
        cout << "GAME OVER\n"; // Game over

    showCursor(); // Show cursor
    system("pause"); // Wait for user input before closing
    return 0;
}