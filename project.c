#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define SIZE 10
#define MAX_SHIPS 5

typedef struct {
    char name[50];
    bool grid[SIZE][SIZE];
    bool hits[SIZE][SIZE];
    int shipsSunk;
} Player;

void initializeGrid(bool grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = false;
        }
    }
}

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

bool parseCoordinates(const char *input, int *row, int *col) {
    if (strlen(input) < 2 || !isalpha(input[0]) || !isdigit(input[1])) {
        return false; // Invalid format
    }
    *col = toupper(input[0]) - 'A'; // Convert column letter to uppercase
    *row = atoi(input + 1) - 1;     // Extract the numeric row and adjust to zero-based index

    if (*row < 0 || *row >= SIZE || *col < 0 || *col >= SIZE) {
        return false; // Out of bounds
    }
    return true;
}

bool canPlaceShip(bool grid[SIZE][SIZE], int row, int col, int size, bool isVertical, char *error) {
    if (isVertical) {
        if (row + size > SIZE) {
            strcpy(error, "Ship goes out of bounds vertically.");
            return false;
        }
        for (int i = 0; i < size; i++) {
            if (grid[row + i][col]) {
                strcpy(error, "Ship overlaps with an existing ship.");
                return false;
            }
        }
    } else {
        if (col + size > SIZE) {
            strcpy(error, "Ship goes out of bounds horizontally.");
            return false;
        }
        for (int i = 0; i < size; i++) {
            if (grid[row][col + i]) {
                strcpy(error, "Ship overlaps with an existing ship.");
                return false;
            }
        }
    }
    return true;
}

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

void clearScreen() {
    printf("Pretend the screen is cleared\n");
}

bool fire(const char *coordinates, bool grid[SIZE][SIZE]) {
    int col = toupper(coordinates[0]) - 'A';
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
    int col = toupper(coordinates[0]) - 'A';
    int row = atoi(coordinates + 1) - 1;

    if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
        printf("Radar sweep at %s: %s\n", coordinates, player->grid[row][col] ? "Ship detected!" : "No ship detected.");
    } else {
        printf("Invalid coordinates for radar sweep.\n");
    }
}

void smoke(Player *player, const char *coordinates) {
    int col = toupper(coordinates[0]) - 'A';
    int row = atoi(coordinates + 1) - 1;

    if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
        printf("Smoke screen deployed at %s.\n", coordinates);
    } else {
        printf("Invalid coordinates for smoke screen.\n");
    }
}

int main() {
    Player player1, player2;
    Player *players[2] = {&player1, &player2};
    int currentPlayerIndex = 0;

    // Player setup
    printf("Enter name for Player 1: ");
    scanf("%49s", player1.name);
    printf("Enter name for Player 2: ");
    scanf("%49s", player2.name);

    initializeGrid(player1.grid);
    initializeGrid(player1.hits);
    initializeGrid(player2.grid);
    initializeGrid(player2.hits);

    player1.shipsSunk = player2.shipsSunk = 0;

    // Game setup: place ships
    for (int i = 0; i < 2; i++) {
        Player *player = players[i];
        printf("%s, place your ships:\n", player->name);
        for (int size = 5; size > 1; size--) {
            char coordinate[4];
            char orientation[10];
            char error[50]; // Buffer to store error messages
            while (true) {
                displayGrid(player->grid, "Your Ships");
                printf("Enter coordinate for a %d-cell ship (e.g., B3): ", size);
                scanf("%3s", coordinate);

                int row, col;
                if (!parseCoordinates(coordinate, &row, &col)) {
                    printf("Invalid coordinate. Try again.\n");
                    continue;
                }

                printf("Enter orientation (vertical or anything else for horizontal): ");
                scanf("%9s", orientation);

                bool isVertical = strcmp(orientation, "vertical") == 0;

                if (canPlaceShip(player->grid, row, col, size, isVertical, error)) {
                    placeShip(player->grid, row, col, size, isVertical);
                    break;
                } else {
                    printf("Error: %s\n", error);
                }
            }
        }
        clearScreen();
    }

    // Game loop
    bool gameRunning = true;
    while (gameRunning) {
        Player *currentPlayer = players[currentPlayerIndex];
        Player *opponent = players[1 - currentPlayerIndex];

        displayGrid(opponent->hits, "HITS");
        printf("%s's turn. Choose an action (Fire, Radar, Smoke): ", currentPlayer->name);

        char command[50];
        scanf("%s", command);

        if (strcmp(command, "Fire") == 0) {
            char coordinates[4];
            printf("Enter coordinates (e.g., B3): ");
            scanf("%3s", coordinates);

            if (fire(coordinates, opponent->grid)) {
                int row = coordinates[1] - '1';
                int col = toupper(coordinates[0]) - 'A';
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
        } else if (strcmp(command, "Radar") == 0) {
            char coordinates[4];
            printf("Enter coordinates for radar sweep: ");
            scanf("%3s", coordinates);
            radarSweep(currentPlayer, coordinates);
        } else if (strcmp(command, "Smoke") == 0) {
            char coordinates[4];
            printf("Enter coordinates for smoke screen: ");
            scanf("%3s", coordinates);
            smoke(currentPlayer, coordinates);
        } else {
            printf("Unknown command. Try again.\n");
        }

        currentPlayerIndex = 1 - currentPlayerIndex; // Switch turns
    }

    return 0;
}
