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

bool fire(const char *coordinates, bool grid[SIZE][SIZE]) {
    int col = coordinates[0] - 'A';
    int row = atoi(coordinates + 1) - 1;

    if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
        return grid[row][col];
    } else {
        printf("Invalid coordinates. Please try again.\n");
        return false;
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

void displayGrid(bool grid[SIZE][SIZE], const char *type) {
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", i + 1);  // Row numbering starts at 1
        for (int j = 0; j < SIZE; j++) {
            if (strcmp(type, "HITS") == 0) {
                printf("%c ", grid[i][j] ? '~' : 'X');
            } else if (strcmp(type, "SHIPS") == 0) {
                printf("%c ", grid[i][j] ? 'S' : '.');
            }
        }
        printf("\n");
    }
}

void radarSweep(Player *player, const char *coordinates) {
    int col = coordinates[0] - 'A';
    int row = atoi(coordinates + 1) - 1;

    if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
        if (player->radarSweeps > 0) {
            player->radarSweeps--;
            printf("Radar Sweep at %s: %s\n", coordinates, player->grid[row][col] ? "Ship detected!" : "No ship detected.");
        } else {
            printf("No radar sweeps left.\n");
        }
    } else {
        printf("Invalid coordinates for radar sweep.\n");
    }
}

bool smoke(Player *player, const char *coordinates) {
    if (player->smokeScreens <= 0) {
        printf("No smoke screens left.\n");
        return false;
    }

    int row = atoi(coordinates + 1) - 1;
    int col = coordinates[0] - 'A';

    if (row >= 0 && row + 1 < SIZE && col >= 0 && col + 1 < SIZE) {
        player->smokeScreens--;
        printf("Smoke screen deployed at %s\n", coordinates);
        return true;
    } else {
        printf("The 2x2 smoke screen is out of bounds.\n");
        return false;
    }
}

void clearScreen() {
    for (int i = 0; i < 30; i++) printf("\n");
}

int main() {
    Player player1, player2;
    Player *players[2] = {&player1, &player2};
    int currentPlayerIndex = 0;
    bool gameRunning = true;

    // Player setup
    printf("Enter name for Player 1: ");
    scanf("%s", player1.name);
    printf("Enter name for Player 2: ");
    scanf("%s", player2.name);

    initializeGrid(player1.grid);
    initializeGrid(player1.hits);
    initializeGrid(player2.grid);
    initializeGrid(player2.hits);

    player1.shipsSunk = player2.shipsSunk = 0;
    player1.radarSweeps = player2.radarSweeps = 3;
    player1.smokeScreens = player2.smokeScreens = 3;

    // Game setup: place ships
    for (int i = 0; i < 2; i++) {
        Player *player = players[i];
        printf("%s, place your ships:\n", player->name);
        for (int size = 5; size > 1; size--) {
            char orientation[10];
            char coordinate[3];
            while (true) {
                displayGrid(player->grid, "SHIPS");
                printf("Enter coordinate for a %d-cell ship (e.g., B3): ", size);
                scanf("%s", coordinate);
                printf("Vertical or horizontal? ");
                scanf("%s", orientation);

                int row = atoi(coordinate + 1) - 1;
                int col = coordinate[0] - 'A';

                if (canPlaceShip(player->grid, row, col, size, orientation)) {
                    placeShip(player->grid, row, col, size, orientation);
                    break;
                } else {
                    printf("Invalid position. Try again.\n");
                }
            }
        }
        clearScreen();
    }

    // Game loop
    while (gameRunning) {
        Player *currentPlayer = players[currentPlayerIndex];
        Player *opponent = players[1 - currentPlayerIndex];

        displayGrid(opponent->hits, "HITS");
        printf("%s's turn. Choose an action (Fire, Radar, Smoke): ", currentPlayer->name);

        char command[50];
        scanf("%s", command);

        if (strcmp(command, "Fire") == 0) {
            char coordinates[3];
            printf("Enter coordinates (e.g., B3): ");
            scanf("%s", coordinates);

            if (fire(coordinates, opponent->grid)) {
                currentPlayer->hits[coordinates[1] - '1'][coordinates[0] - 'A'] = true;
                printf("Hit!\n");
                opponent->shipsSunk++;
                if (opponent->shipsSunk >= MAX_SHIPS) {
                    printf("%s wins!\n", currentPlayer->name);
                    gameRunning = false;
                }
            } else {
                printf("Miss.\n");
            }
        } else if (strcmp(command, "Radar") == 0) {
            char coordinates[3];
            printf("Enter coordinates for radar sweep: ");
            scanf("%s", coordinates);
            radarSweep(currentPlayer, coordinates);
        } else if (strcmp(command, "Smoke") == 0) {
            char coordinates[3];
            printf("Enter coordinates for smoke screen: ");
            scanf("%s", coordinates);
            smoke(currentPlayer, coordinates);
        } else {
            printf("Unknown command. Try again.\n");
        }

        currentPlayerIndex = 1 - currentPlayerIndex;
    }

    return 0;
}
