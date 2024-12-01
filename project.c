#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

void initializeGrid(bool grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = false;
        }
    }
}

void displayGridHard(bool grid[SIZE][SIZE]) {
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", i + 1);
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", grid[i][j] ? '~' : 'X');
        }
        printf("\n");
    }
}

void displayGridEasy(bool gridHits[SIZE][SIZE], bool shipsLocation[SIZE][SIZE]) {
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", i + 1);
        for (int j = 0; j < SIZE; j++) {
            if (gridHits[i][j])
                printf("~ ");
            else if (shipsLocation[i][j])
                printf("M ");
            else
                printf("X ");
        }
        printf("\n");
    }
}

bool canPlaceShip(bool grid[SIZE][SIZE], int row, int col, int size, const char *orientation) {
    if (strcmp(orientation, "vertical") == 0) {
        if (row + size > SIZE) return false;
        for (int i = 0; i < size; i++) {
            if (grid[row + i][col]) return false;
        }
    } else if (strcmp(orientation, "horizontal") == 0) {
        if (col + size > SIZE) return false;
        for (int i = 0; i < size; i++) {
            if (grid[row][col + i]) return false;
        }
    }
    return true;
}

void placeShip(bool grid[SIZE][SIZE], int row, int col, int size, const char *orientation) {
    if (strcmp(orientation, "vertical") == 0) {
        for (int i = 0; i < size; i++) {
            grid[row + i][col] = true;
        }
    } else {
        for (int i = 0; i < size; i++) {
            grid[row][col + i] = true;
        }
    }
}

bool fire(const char *coordinates, bool grid[SIZE][SIZE]) {
    int col = coordinates[0] - 'A';
    int row = atoi(coordinates + 1) - 1;

    if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
        if (grid[row][col]) {
            grid[row][col] = false; // Mark as hit
            return true;
        }
    }
    return false;
}

void radarSweep(Player *player, const char *coordinates) {
    if (player->radarSweeps <= 0) {
        printf("No radar sweeps left.\n");
        return;
    }
    int col = coordinates[0] - 'A';
    int row = atoi(coordinates + 1) - 1;

    if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
        player->radarSweeps--;
        printf("Radar Sweep at %s: %s\n", coordinates, player->grid[row][col] ? "Ship detected!" : "No ship detected.");
    } else {
        printf("Invalid coordinates.\n");
    }
}

void smoke(Player *player, const char *coordinates) {
    if (player->smokeScreens <= 0) {
        printf("No smoke screens left.\n");
        return;
    }
    int col = coordinates[0] - 'A';
    int row = atoi(coordinates + 1) - 1;

    if (row >= 0 && row + 1 < SIZE && col >= 0 && col + 1 < SIZE) {
        player->smokeScreens--;
        printf("Smoke screen deployed at %s.\n", coordinates);
    } else {
        printf("Coordinates out of bounds for smoke screen.\n");
    }
}

int main() {
    Player player1, player2;
    Player *players[2] = {&player1, &player2};
    int currentPlayerIndex = 0;
    bool gameRunning = true;

    printf("Enter name for Player 1: ");
    scanf("%s", player1.name);
    printf("Enter name for Player 2: ");
    scanf("%s", player2.name);

    initializeGrid(player1.grid);
    initializeGrid(player2.grid);
    player1.radarSweeps = 3;
    player2.radarSweeps = 3;
    player1.smokeScreens = 3;
    player2.smokeScreens = 3;
    player1.shipsSunk = 0;
    player2.shipsSunk = 0;

    // Setup and gameplay logic omitted for brevity...
    return 0;
}
