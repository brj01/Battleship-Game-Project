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

// Place ship
void placeShip(bool grid[SIZE][SIZE], int row, int col, int size, bool isVertical) {
    for (int i = 0; i < size; i++) {
        if (isVertical) grid[row + i][col] = true;
        else grid[row][col + i] = true;
    }
}

// Random ship placement for bot
void botPlaceShips(Bot *bot) {
    for (int i = 0; i < MAX_SHIPS; i++) {
        int size = shipSizes[i];
        while (true) {
            int row = rand() % SIZE;
            int col = rand() % SIZE;
            bool isVertical = rand() % 2;
            if (canPlaceShip(bot->grid, row, col, size, isVertical)) {
                placeShip(bot->grid, row, col, size, isVertical);
                break;
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
    if (bot->difficulty == 1) { // Easy: Random fire
        do {
            row = rand() % SIZE;
            col = rand() % SIZE;
        } while (opponent->hits[row][col]);
    } else if (bot->difficulty == 2 || bot->difficulty == 3) { // Medium/Hard: Target logic
        if (bot->lastHitSuccess) {
            // Target adjacent tiles
            if (bot->lastHitRow > 0 && !opponent->hits[bot->lastHitRow - 1][bot->lastHitCol]) {
                row = bot->lastHitRow - 1;
                col = bot->lastHitCol;
            } else if (bot->lastHitRow < SIZE - 1 && !opponent->hits[bot->lastHitRow + 1][bot->lastHitCol]) {
                row = bot->lastHitRow + 1;
                col = bot->lastHitCol;
            } else if (bot->lastHitCol > 0 && !opponent->hits[bot->lastHitRow][bot->lastHitCol - 1]) {
                row = bot->lastHitRow;
                col = bot->lastHitCol - 1;
            } else if (bot->lastHitCol < SIZE - 1 && !opponent->hits[bot->lastHitRow][bot->lastHitCol + 1]) {
                row = bot->lastHitRow;
                col = bot->lastHitCol + 1;
            } else {
                row = rand() % SIZE;
                col = rand() % SIZE;
            }
        } else { // Random fire if no successful hit
            do {
                row = rand() % SIZE;
                col = rand() % SIZE;
            } while (opponent->hits[row][col]);
        }
    }

    char result[20];
    bot->lastHitSuccess = fire(opponent, row, col, result);
    bot->lastHitRow = row;
    bot->lastHitCol = col;
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
    bot.difficulty = 2; // Change for difficulty selection (1 = Easy, 2 = Medium, 3 = Hard)

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
                if (fire(&bot, row, col, result)) bot.shipsSunk++;
                printf("%s\n", result);
            } else {
                printf("Invalid coordinates! Try again.\n");
            }
        } else { // Bot's turn
            botFire(&bot, &player);
            if (bot.lastHitSuccess) player.shipsSunk++;
        }

        currentPlayerIndex = 1 - currentPlayerIndex; // Switch turns
    }

    printf("%s wins!\n", player.shipsSunk == MAX_SHIPS ? bot.name : player.name);
    return 0;
}
