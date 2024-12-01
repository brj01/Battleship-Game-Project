#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define SIZE 10
#define MAX_SHIPS 5

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

// Bot places its ships randomly
void botPlaceShips(Bot *bot) {
    int shipSizes[] = {5, 4, 3, 3, 2};
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
    char coordinates[3];
    bool success = false;

    // Use radar if available
    if (bot->radarSweeps > 0) {
        int row = rand() % SIZE;
        int col = rand() % SIZE;
        sprintf(coordinates, "%c%d", 'A' + col, row + 1);
        printf("Bot uses radar at %s\n", coordinates);

        if (opponentGrid[row][col]) {
            printf("Ship detected at %s! Focusing fire.\n", coordinates);
            opponentHits[row][col] = true;
            success = true;
        }
    }

    // Target surrounding cells
    if (!success) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (opponentHits[i][j] && opponentGrid[i][j]) {
                    if (i > 0 && !opponentHits[i - 1][j]) {
                        opponentHits[i - 1][j] = true;
                        printf("Bot fires at %c%d: %s\n", 'A' + j, i, opponentGrid[i - 1][j] ? "Hit!" : "Miss.");
                        return;
                    }
                    if (i < SIZE - 1 && !opponentHits[i + 1][j]) {
                        opponentHits[i + 1][j] = true;
                        printf("Bot fires at %c%d: %s\n", 'A' + j, i + 2, opponentGrid[i + 1][j] ? "Hit!" : "Miss.");
                        return;
                    }
                    if (j > 0 && !opponentHits[i][j - 1]) {
                        opponentHits[i][j - 1] = true;
                        printf("Bot fires at %c%d: %s\n", 'A' + j - 1, i + 1, opponentGrid[i][j - 1] ? "Hit!" : "Miss.");
                        return;
                    }
                    if (j < SIZE - 1 && !opponentHits[i][j + 1]) {
                        opponentHits[i][j + 1] = true;
                        printf("Bot fires at %c%d: %s\n", 'A' + j + 1, i + 1, opponentGrid[i][j + 1] ? "Hit!" : "Miss.");
                        return;
                    }
                }
            }
        }
    }

    // Fire randomly if no strategy applies
    if (!success) {
        easyDifficultyFire(bot, opponentHits, opponentGrid);
    }
}

int main() {
    srand(time(NULL));

    // Initialize bot and opponent
    Bot bot = {"AI", {}, {}, 3, -1, -1, false, 0};
    bool opponentGrid[SIZE][SIZE] = {false};
    bool opponentHits[SIZE][SIZE] = {false};

    // Place bot's ships
    initializeGrid(bot.grid);
    botPlaceShips(&bot);

    // Simulate firing in different difficulties
    printf("Easy difficulty:\n");
    easyDifficultyFire(&bot, opponentHits, opponentGrid);

    printf("\nMedium difficulty:\n");
    mediumDifficultyFire(&bot, opponentHits, opponentGrid);

    return 0;
}
