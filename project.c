#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

bool parseCoordinates(const char *input, int *row, int *col) {
    if (strlen(input) < 2 || input[0] < 'A' || input[0] > 'J') {
        return false; // Invalid format
    }
    *col = input[0] - 'A'; // Extract column (A-J)
    *row = atoi(input + 1) - 1; // Extract row (1-10)

    if (*row < 0 || *row >= SIZE || *col < 0 || *col >= SIZE) {
        return false; // Out of bounds
    }
    return true;
}

bool validateOrientation(const char *orientation) {
    return strcmp(orientation, "vertical") == 0 || strcmp(orientation, "horizontal") == 0;
}

bool canPlaceShip(bool grid[SIZE][SIZE], int row, int col, int size, const char *orientation) {
    if (strcmp(orientation, "vertical") == 0) {
        if (row + size > SIZE) return false; // Ship goes out of bounds vertically
        for (int i = 0; i < size; i++) {
            if (grid[row + i][col]) return false; // Conflicts with existing ship
        }
    } else if (strcmp(orientation, "horizontal") == 0) {
        if (col + size > SIZE) return false; // Ship goes out of bounds horizontally
        for (int i = 0; i < size; i++) {
            if (grid[row][col + i]) return false; // Conflicts with existing ship
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

void clearScreen() {
    printf("Pretend the screen is cleared\n");
}

int main() {
    Player player1, player2;
    Player *players[2] = {&player1, &player2};
    int currentPlayerIndex = 0;

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

                int row, col;
                if (!parseCoordinates(coordinate, &row, &col)) {
                    printf("Invalid coordinate. Try again.\n");
                    continue;
                }

                printf("Vertical or horizontal? ");
                scanf("%9s", orientation); // Limit to 9 characters

                if (!validateOrientation(orientation)) {
                    printf("Invalid orientation. Use 'vertical' or 'horizontal'.\n");
                    continue;
                }

                if (canPlaceShip(player->grid, row, col, size, orientation)) {
                    placeShip(player->grid, row, col, size, orientation);
                    break;
                } else {
                    printf("Invalid position or conflicting with another ship. Try again.\n");
                }
            }
        }
        clearScreen();
    }

    return 0;
}
