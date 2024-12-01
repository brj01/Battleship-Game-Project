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
    int smokeScreens; // Tracks available smoke screens
} Player;

// Ship sizes and names
const int shipSizes[] = {5, 4, 3, 3, 2};
const char *shipNames[] = {"Carrier", "Battleship", "Destroyer 1", "Destroyer 2", "Submarine"};

// Grid Initialization
void initializeGrid(bool grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = false;
        }
    }
}

// Display Grid
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

// Parse Coordinates
bool parseCoordinates(const char *input, int *row, int *col) {
    if (strlen(input) < 2 || !isalpha(input[0]) || !isdigit(input[1])) {
        return false;
    }
    *col = toupper(input[0]) - 'A';
    *row = atoi(input + 1) - 1;

    return (*row >= 0 && *row < SIZE && *col >= 0 && *col < SIZE);
}

// Validate Ship Placement
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

// Place Ship
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

// Gameplay: Fire
bool fire(const char *coordinates, bool grid[SIZE][SIZE], bool hits[SIZE][SIZE], char *result) {
    int row, col;
    if (!parseCoordinates(coordinates, &row, &col)) {
        strcpy(result, "Invalid coordinates.");
        return false;
    }

    if (grid[row][col]) {
        hits[row][col] = true;
        strcpy(result, "Hit!");
        return true;
    } else {
        hits[row][col] = true;
        strcpy(result, "Miss.");
        return false;
    }
}

// Torpedo: Targets an entire row or column
void torpedo(Player *opponent, char *result) {
    char type[10];
    printf("Do you want to target a row or column? (row/column): ");
    scanf("%9s", type);

    if (strcmp(type, "row") == 0) {
        int row;
        printf("Enter the row number (1-10): ");
        scanf("%d", &row);
        row -= 1;

        if (row < 0 || row >= SIZE) {
            strcpy(result, "Invalid row number.");
            return;
        }

        for (int col = 0; col < SIZE; col++) {
            if (opponent->grid[row][col]) {
                opponent->hits[row][col] = true;
                opponent->grid[row][col] = false;
            }
        }
        strcpy(result, "Torpedo fired at row!");
    } else if (strcmp(type, "column") == 0) {
        char colChar;
        printf("Enter the column letter (A-J): ");
        scanf(" %c", &colChar);
        int col = toupper(colChar) - 'A';

        if (col < 0 || col >= SIZE) {
            strcpy(result, "Invalid column letter.");
            return;
        }

        for (int row = 0; row < SIZE; row++) {
            if (opponent->grid[row][col]) {
                opponent->hits[row][col] = true;
                opponent->grid[row][col] = false;
            }
        }
        strcpy(result, "Torpedo fired at column!");
    } else {
        strcpy(result, "Invalid choice. Use 'row' or 'column'.");
    }
}

// Radar Sweep
void radarSweep(Player *opponent, const char *coordinates, char *result) {
    int row, col;
    if (!parseCoordinates(coordinates, &row, &col)) {
        strcpy(result, "Invalid coordinates for radar sweep.");
        return;
    }

    sprintf(result, "Radar sweep at %c%d: %s", 'A' + col, row + 1,
            opponent->grid[row][col] ? "Ship detected!" : "No ship detected.");
}

// Smoke Screen
void smokeScreen(Player *player, const char *coordinates, char *result) {
    int row, col;
    if (!parseCoordinates(coordinates, &row, &col)) {
        strcpy(result, "Invalid coordinates for smoke screen.");
        return;
    }

    sprintf(result, "Smoke screen deployed at %c%d. Area obscured.", 'A' + col, row + 1);
}

// Check if a ship is sunk
bool isShipSunk(bool grid[SIZE][SIZE], bool hits[SIZE][SIZE], int row, int col, int size, bool isVertical) {
    for (int i = 0; i < size; i++) {
        int r = isVertical ? row + i : row;
        int c = isVertical ? col : col + i;

        if (!hits[r][c]) {
            return false;
        }
    }
    return true;
}

// Main Function
int main() {
    Player player1, player2;
    Player *players[2] = {&player1, &player2};
    int currentPlayerIndex = 0;

    printf("Enter name for Player 1: ");
    scanf("%49s", player1.name);
    printf("Enter name for Player 2: ");
    scanf("%49s", player2.name);

    initializeGrid(player1.grid);
    initializeGrid(player1.hits);
    initializeGrid(player2.grid);
    initializeGrid(player2.hits);

    player1.shipsSunk = player2.shipsSunk = 0;

    // Game setup: Ship placement
    for (int i = 0; i < 2; i++) {
        Player *player = players[i];
        printf("%s, place your ships:\n", player->name);
        for (int j = 0; j < MAX_SHIPS; j++) {
            char coordinate[4];
            char orientation[10];
            char error[50];
            while (true) {
                displayGrid(player->grid, "Your Ships", true);
                printf("Enter coordinate for your %s (size %d): ", shipNames[j], shipSizes[j]);
                scanf("%3s", coordinate);

                int row, col;
                if (!parseCoordinates(coordinate, &row, &col)) {
                    printf("Invalid coordinate. Try again.\n");
                    continue;
                }

                printf("Enter orientation (vertical or anything else for horizontal): ");
                scanf("%9s", orientation);

                bool isVertical = strcmp(orientation, "vertical") == 0;

                if (canPlaceShip(player->grid, row, col, shipSizes[j], isVertical, error)) {
                    placeShip(player->grid, row, col, shipSizes[j], isVertical);
                    break;
                } else {
                    printf("Error: %s\n", error);
                }
            }
        }
    }

    // Gameplay loop
    while (true) {
        Player *currentPlayer = players[currentPlayerIndex];
        Player *opponent = players[1 - currentPlayerIndex];

        displayGrid(opponent->hits, "Opponent's Grid", true);
        printf("%s's turn. Choose an action (Fire, Torpedo, Radar, Smoke): ", currentPlayer->name);

        char command[50];
        scanf("%s", command);

        if (strcmp(command, "Fire") == 0) {
            char coordinates[4];
            printf("Enter coordinates (e.g., B3): ");
            scanf("%3s", coordinates);

            char result[20];
            fire(coordinates, opponent->grid, opponent->hits, result);
            printf("%s\n", result);
        } else if (strcmp(command, "Torpedo") == 0) {
            char result[50];
            torpedo(opponent, result);
            printf("%s\n", result);
        } else if (strcmp(command, "Radar") == 0) {
            char coordinates[4], result[50];
            printf("Enter coordinates for radar sweep: ");
            scanf("%3s", coordinates);
            radarSweep(opponent, coordinates, result);
            printf("%s\n", result);
        } else if (strcmp(command, "Smoke") == 0) {
            char coordinates[4], result[50];
            printf("Enter coordinates for smoke screen: ");
            scanf("%3s", coordinates);
            smokeScreen(currentPlayer, coordinates, result);
            printf("%s\n", result);
        } else {
            printf("Unknown command. Try again.\n");
        }

        currentPlayerIndex = 1 - currentPlayerIndex;
    }

    return 0;
}
