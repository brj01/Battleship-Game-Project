#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define SIZE 10
#define MAX_SHIPS 5

typedef struct {
    char name[50];
    bool grid[SIZE][SIZE];
    bool hits[SIZE][SIZE];
    int shipsSunk;
    int smokeScreens;
    int torpedoAvailable;
    int artilleryAvailable;
} Player;

typedef struct {
    char name[50];
    bool grid[SIZE][SIZE];
    bool hits[SIZE][SIZE];
    int shipsSunk;
    int difficulty; // 1: Easy, 2: Medium, 3: Hard
    int lastHitRow;
    int lastHitCol;
    bool lastHitSuccess;
} Bot;

const int shipSizes[] = {5, 4, 3, 3, 2};
const char *shipNames[] = {"Carrier", "Battleship", "Destroyer 1", "Destroyer 2", "Submarine"};

// Initialize grid
void initializeGrid(bool grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = false;
        }
    }
}

// Display grid
void displayGrid(bool grid[SIZE][SIZE], const char *label, bool showMisses) {
    printf("\n%s:\n", label);
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", i + 1);
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j])
                printf("~ ");
            else
                printf("%c ", showMisses ? 'X' : '.');
        }
        printf("\n");
    }
}

// Parse coordinates
bool parseCoordinates(const char *input, int *row, int *col) {
    if (strlen(input) < 2 || !isalpha(input[0]) || !isdigit(input[1])) {
        return false;
    }
    *col = toupper(input[0]) - 'A';
    *row = atoi(input + 1) - 1;
    return (*row >= 0 && *row < SIZE && *col >= 0 && *col < SIZE);
}

// Ship placement validation
bool canPlaceShip(bool grid[SIZE][SIZE], int row, int col, int size, bool isVertical) {
    if (isVertical) {
        if (row + size > SIZE) return false;
        for (int i = 0; i < size; i++) {
            if (grid[row + i][col]) return false;
        }
    } else {
        if (col + size > SIZE) return false;
        for (int i = 0; i < size; i++) {
            if (grid[row][col + i]) return false;
        }
    }
    return true;
}

void placeShip(bool grid[SIZE][SIZE], int row, int col, int size, bool isVertical) {
    for (int i = 0; i < size; i++) {
        if (isVertical) grid[row + i][col] = true;
        else grid[row][col + i] = true;
    }
}


bool checkShipSunk(bool grid[SIZE][SIZE], bool hits[SIZE][SIZE], int row, int col) {

    if (!grid[row][col]) {
        return false;
    }

   
    int startCol = col, endCol = col;
    while (startCol > 0 && grid[row][startCol - 1]) {
        startCol--;
    }
    while (endCol < SIZE - 1 && grid[row][endCol + 1]) {
        endCol++;
    }

    bool isHorizontalSunk = true;
    for (int c = startCol; c <= endCol; c++) {
        if (!hits[row][c]) {
            isHorizontalSunk = false;
            break;
        }
    }


    int startRow = row, endRow = row;
    while (startRow > 0 && grid[startRow - 1][col]) {
        startRow--;
    }
    while (endRow < SIZE - 1 && grid[endRow + 1][col]) {
        endRow++;
    }

    bool isVerticalSunk = true;
    for (int r = startRow; r <= endRow; r++) {
        if (!hits[r][col]) {
            isVerticalSunk = false;
            break;
        }
    }


    return isHorizontalSunk || isVerticalSunk;
}

void artillery(Player *opponent, int centerRow, int centerCol) {
    printf("Artillery strike at %c%d:\n", 'A' + centerCol, centerRow + 1);
    for (int i = centerRow - 1; i <= centerRow + 1; i++) {
        for (int j = centerCol - 1; j <= centerCol + 1; j++) {
            if (i >= 0 && i < SIZE && j >= 0 && j < SIZE) {
                opponent->hits[i][j] = true;
                opponent->grid[i][j] = false;
            }
        }
    }
    printf("Artillery strike completed.\n");
}


void torpedo(Player *opponent, char *direction, int index) {
    printf("Torpedo strike on %s %d:\n", strcmp(direction, "row") == 0 ? "row" : "column", index + 1);
    if (strcmp(direction, "row") == 0) {
        for (int col = 0; col < SIZE; col++) {
            opponent->hits[index][col] = true;
            opponent->grid[index][col] = false;
        }
    } else if (strcmp(direction, "column") == 0) {
        for (int row = 0; row < SIZE; row++) {
            opponent->hits[row][index] = true;
            opponent->grid[row][index] = false;
        }
    }
    printf("Torpedo strike completed.\n");
}

// Radar Sweep: Detect ships in a 3x3 area
void radarSweep(Player *opponent, int centerRow, int centerCol) {
    printf("Radar sweep at %c%d:\n", 'A' + centerCol, centerRow + 1);
    for (int i = centerRow - 1; i <= centerRow + 1; i++) {
        for (int j = centerCol - 1; j <= centerCol + 1; j++) {
            if (i >= 0 && i < SIZE && j >= 0 && j < SIZE) {
                if (opponent->grid[i][j]) {
                    printf("Ship detected at %c%d\n", 'A' + j, i + 1);
                }
            }
        }
    }
}

// Fire
bool fire(Player *opponent, int row, int col, char *result) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
        strcpy(result, "Out of bounds!");
        return false;
    }
    if (opponent->grid[row][col]) {
        opponent->hits[row][col] = true;
        opponent->grid[row][col] = false;
        strcpy(result, "Hit!");
        return true;
    } else {
        opponent->hits[row][col] = true;
        strcpy(result, "Miss.");
        return false;
    }
}

// Bot fires based on difficulty
void botFire(Bot *bot, Player *opponent) {
    int row, col;
    do {
        row = rand() % SIZE;
        col = rand() % SIZE;
    } while (opponent->hits[row][col]);

    char result[20];
    fire(opponent, row, col, result);
    printf("Bot fires at %c%d: %s\n", 'A' + col, row + 1, result);
}

// Main function
int main() {
    srand(time(NULL));

    Player player;
    Bot bot;
    int currentPlayerIndex = 0;

    printf("Enter your name: ");
    scanf("%49s", player.name);
    strcpy(bot.name, "AI");

    initializeGrid(player.grid);
    initializeGrid(player.hits);
    initializeGrid(bot.grid);
    initializeGrid(bot.hits);

    player.shipsSunk = bot.shipsSunk = 0;
    player.smokeScreens = 3;
    player.torpedoAvailable = 1;
    player.artilleryAvailable = 1;

    printf("Place your ships:\n");
    for (int i = 0; i < MAX_SHIPS; i++) {
        char coordinate[4];
        char orientation[10];
        while (true) {
            displayGrid(player.grid, "Your Ships", false);
            printf("Enter coordinate for your %s (size %d): ", shipNames[i], shipSizes[i]);
            scanf("%3s", coordinate);

            int row, col;
            if (!parseCoordinates(coordinate, &row, &col)) {
                printf("Invalid coordinates! Try again.\n");
                continue;
            }

            printf("Enter orientation (vertical or horizontal): ");
            scanf("%9s", orientation);
            bool isVertical = strcmp(orientation, "vertical") == 0;

            if (canPlaceShip(player.grid, row, col, shipSizes[i], isVertical)) {
                placeShip(player.grid, row, col, shipSizes[i], isVertical);
                break;
            } else {
                printf("Invalid placement! Try again.\n");
            }
        }
    }

    printf("Bot is placing ships...\n");
    botPlaceShips(&bot);

    // Gameplay loop
    while (player.shipsSunk < MAX_SHIPS && bot.shipsSunk < MAX_SHIPS) {
        if (currentPlayerIndex == 0) { // Player's turn
            displayGrid(bot.hits, "Opponent's Grid", true);
            char coordinates[4];
            printf("Your turn! Enter coordinates to fire: ");
            scanf("%3s", coordinates);

            int row, col;
            char result[20];
            if (parseCoordinates(coordinates, &row, &col)) {
                if (fire(&bot, row, col, result)) {
                    if (checkShipSunk(&bot, row, col, shipSizes[i])) {
                        bot.shipsSunk++;
                        printf("You sunk the opponent's ship!\n");
                    }
                    printf("%s\n", result);
                }
            } else {
                printf("Invalid coordinates! Try again.\n");
            }
        } else { // Bot's turn
            botFire(&bot, &player);
            if (bot.lastHitSuccess) {
                if (checkShipSunk(&player, bot.lastHitRow, bot.lastHitCol, shipSizes[i])) {
                    player.shipsSunk++;
                    printf("The bot sunk one of your ships!\n");
                }
            }
        }

        currentPlayerIndex = 1 - currentPlayerIndex; // Switch turns
    }

    printf("%s wins!\n", player.shipsSunk == MAX_SHIPS ? bot.name : player.name);
    return 0;
}
