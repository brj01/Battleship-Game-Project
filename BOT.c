#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define SIZE 10
#define MAX_SHIPS 5

typedef struct {
    char name[50];
    bool grid[SIZE][SIZE];
    bool hits[SIZE][SIZE];
    int shipsSunk;
    int radarSweeps;
    int smokeScreens;
} Player;

typedef struct {
    char name[50];
    bool grid[SIZE][SIZE];
    bool hits[SIZE][SIZE];
    int radarSweeps;
    int lastHitRow;
    int lastHitCol;
    bool lastHitSuccess;
    int shipsSunk;
} Bot;

const int shipSizes[] = {5, 4, 3, 3, 2};
const char *shipNames[] = {"Carrier", "Battleship", "Destroyer 1", "Destroyer 2", "Submarine"};

// Initialize the grid
void initializeGrid(bool grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = false;
        }
    }
}

// Check if a ship can be placed
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

// Place a ship on the grid
void placeShip(bool grid[SIZE][SIZE], int row, int col, int size, bool isVertical) {
    if (isVertical) {
        for (int i = 0; i < size; i++) {
            grid[row + i][col] = true;
        }
    } else {
        for (int i = 0; i < size; i++) {
            grid[row][col + i] = true;
        }
    }
}

// Display the grid
void displayGrid(bool grid[SIZE][SIZE], const char *label) {
    printf("\n%s:\n", label);
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", i + 1);
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", grid[i][j] ? '~' : 'X');
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

// Simulate bot firing randomly
void easyDifficultyFire(Bot *bot, bool opponentHits[SIZE][SIZE], bool opponentGrid[SIZE][SIZE]) {
    int row, col;
    do {
        row = rand() % SIZE;
        col = rand() % SIZE;
    } while (opponentHits[row][col]);

    opponentHits[row][col] = true;
    printf("Bot fires at %c%d: %s\n", 'A' + col, row + 1, opponentGrid[row][col] ? "Hit!" : "Miss.");
}

// Simulate bot firing with a strategy
void mediumDifficultyFire(Bot *bot, bool opponentHits[SIZE][SIZE], bool opponentGrid[SIZE][SIZE]) {
    // TODO: Implement strategy for medium difficulty
    easyDifficultyFire(bot, opponentHits, opponentGrid); // Placeholder
}

// Simulate bot firing with a hard difficulty strategy
void hardDifficultyFire(Bot *bot, bool opponentHits[SIZE][SIZE], bool opponentGrid[SIZE][SIZE]) {
    // TODO: Implement strategy for hard difficulty
    easyDifficultyFire(bot, opponentHits, opponentGrid); // Placeholder
}

// Bot places its ships randomly
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

// Player places ships
void playerPlaceShips(Player *player) {
    printf("%s, place your ships:\n", player->name);
    for (int i = 0; i < MAX_SHIPS; i++) {
        char coordinates[3];
        char orientation[10];
        while (true) {
            displayGrid(player->grid, "Your Ships");
            printf("Enter the coordinate for %s (size %d): ", shipNames[i], shipSizes[i]);
            scanf("%s", coordinates);

            int row, col;
            if (!parseCoordinates(coordinates, &row, &col)) {
                printf("Invalid coordinates. Try again.\n");
                continue;
            }

            printf("Enter orientation (vertical/horizontal): ");
            scanf("%s", orientation);
            bool isVertical = strcmp(orientation, "vertical") == 0;

            if (canPlaceShip(player->grid, row, col, shipSizes[i], isVertical)) {
                placeShip(player->grid, row, col, shipSizes[i], isVertical);
                break;
            } else {
                printf("Cannot place ship here. Try again.\n");
            }
        }
    }
}

// Check if all ships are sunk
bool allShipsSunk(bool grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j]) return false;
        }
    }
    return true;
}

// Main function
int main() {
    srand(time(NULL));

    Player player;
    Bot bot = {"AI", {}, {}, 0, -1, -1, false, 0};

    printf("Enter your name: ");
    scanf("%s", player.name);

    initializeGrid(player.grid);
    initializeGrid(player.hits);
    initializeGrid(bot.grid);
    initializeGrid(bot.hits);

    playerPlaceShips(&player);
    botPlaceShips(&bot);

    int difficulty;
    printf("Choose difficulty (1 = Easy, 2 = Medium, 3 = Hard): ");
    scanf("%d", &difficulty);

    while (true) {
        // Player's turn
        displayGrid(bot.hits, "Opponent's Grid");
        printf("Your turn. Enter coordinates to fire: ");
        char coordinates[3];
        scanf("%s", coordinates);

        int row, col;
        if (!parseCoordinates(coordinates, &row, &col) || player.hits[row][col]) {
            printf("Invalid move. Try again.\n");
            continue;
        }

        player.hits[row][col] = true;
        if (bot.grid[row][col]) {
            bot.grid[row][col] = false;
            printf("Hit!\n");
        } else {
            printf("Miss.\n");
        }

        if (allShipsSunk(bot.grid)) {
            printf("You win!\n");
            break;
        }

        // Bot's turn
        printf("Bot's turn...\n");
        if (difficulty == 1) {
            easyDifficultyFire(&bot, player.hits, player.grid);
        } else if (difficulty == 2) {
            mediumDifficultyFire(&bot, player.hits, player.grid);
        } else if (difficulty == 3) {
            hardDifficultyFire(&bot, player.hits, player.grid);
        }

        if (allShipsSunk(player.grid)) {
            printf("Bot wins!\n");
            break;
        }
    }

    return 0;
}
