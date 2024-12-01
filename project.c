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
        if (row + size > SIZE) return false; // Check if ship fits in the grid
        for (int i = 0; i < size; i++) {
            if (grid[row + i][col]) return false; // Ensure no conflict with existing ships
        }
    } else if (strcmp(orientation, "horizontal") == 0) {
        if (col + size > SIZE) return false; // Check if ship fits in the grid
        for (int i = 0; i < size; i++) {
            if (grid[row][col + i]) return false; // Ensure no conflict with existing ships
        }
    } else {
        return false; // Invalid orientation
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
void clearScreen() {
printf("pretend screen cleared");
}

int main() {
    Player player1, player2;
    Player *players[2] = {&player1, &player2};
    int currentPlayerIndex = 0;
    bool gameRunning = true;

    // Player setup
    printf("Enter name for Player 1: ");
    scanf("%49s", player1.name); // Limit input length to prevent buffer overflow
    printf("Enter name for Player 2: ");
    scanf("%49s", player2.name);

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
            char coordinate[4];
            while (true) {
                displayGridHard(player->grid);
                printf("Enter coordinate for a %d-cell ship (e.g., B3): ", size);
                scanf("%3s", coordinate); // Limit to 3 characters
                printf("Vertical or horizontal? ");
                scanf("%9s", orientation); // Limit to 9 characters

                int row = atoi(coordinate + 1) - 1;
                int col = coordinate[0] - 'A';

                if (row >= 0 && row < SIZE && col >= 0 && col < SIZE &&
                    canPlaceShip(player->grid, row, col, size, orientation)) {
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

        displayGridEasy(opponent->hits, opponent->grid);
        printf("%s's turn. Choose an action (Fire, Radar, Smoke): ", currentPlayer->name);

        char command[10];
        scanf("%9s", command); // Limit to 9 characters

        if (strcmp(command, "Fire") == 0) {
            char coordinates[4];
            printf("Enter coordinates (e.g., B3): ");
            scanf("%3s", coordinates); // Limit to 3 characters

            int row = atoi(coordinates + 1) - 1;
            int col = coordinates[0] - 'A';

            if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
                if (fire(coordinates, opponent->grid)) {
                    currentPlayer->hits[row][col] = true;
                    printf("Hit!\n");
                    opponent->shipsSunk++;
                    if (opponent->shipsSunk >= MAX_SHIPS) {
                        printf("%s wins!\n", currentPlayer->name);
                        gameRunning = false;
                    }
                } else {
                    printf("Miss.\n");
                }
            } else {
                printf("Invalid coordinates. Try again.\n");
            }
        } else if (strcmp(command, "Radar") == 0) {
            char coordinates[4];
            printf("Enter coordinates for radar sweep (e.g., B3): ");
            scanf("%3s", coordinates);
            radarSweep(currentPlayer, coordinates);
        } else if (strcmp(command, "Smoke") == 0) {
            char coordinates[4];
            printf("Enter coordinates for smoke screen (e.g., B3): ");
            scanf("%3s", coordinates);
            smoke(currentPlayer, coordinates);
        } else {
            printf("Unknown command. Try again.\n");
        }

        // Switch turn
        if (gameRunning) {
            currentPlayerIndex = 1 - currentPlayerIndex;
        }
    }

    return 0;
}

