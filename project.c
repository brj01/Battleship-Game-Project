#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define SIZE 10
#define MAX_SHIPS 5
#define MAX_RADAR_SWEEPS 3

typedef struct {
    char name[50];
    bool grid[SIZE][SIZE];
    bool hits[SIZE][SIZE];
    bool smoke[SIZE][SIZE];
    int shipsSunk;
    int smokeScreens;
    bool torpedoAvailable;
    bool artilleryAvailable;
    int radarSweepsRemaining;
} Player;

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

// Check if a ship is sunk
bool checkShipSunk(bool grid[SIZE][SIZE], bool hits[SIZE][SIZE], int row, int col) {
    if (!grid[row][col]) {
        return false;
    }

    // Check horizontally
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

    // Check vertically
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

// Artillery: Hits a 3x3 area
void artillery(Player *opponent, int centerRow, int centerCol) {
    printf("Artillery strike at %c%d:\n", 'A' + centerCol, centerRow + 1);
    for (int i = centerRow ; i <= centerRow + 1; i++) {
        for (int j = centerCol ; j <= centerCol + 1; j++) {
            if (i >= 0 && i < SIZE && j >= 0 && j < SIZE) {
                opponent->hits[i][j] = true;
                opponent->grid[i][j] = false;
            }
        }
    }
    printf("Artillery strike completed.\n");
}

// Torpedo: Targets an entire row or column
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

// Radar Sweep
void radarSweep(Player *opponent, int centerRow, int centerCol) {
    printf("Radar sweep at %c%d:\n", 'A' + centerCol, centerRow + 1);
    for (int i = centerRow ; i <= centerRow + 1; i++) {
        for (int j = centerCol ; j <= centerCol + 1; j++) {
            if (i >= 0 && i < SIZE && j >= 0 && j < SIZE) {
                if (opponent->smoke[i][j]) {
                    printf("Area at %c%d is obscured by smoke.\n", 'A' + j, i + 1);
                } else if (opponent->grid[i][j]) {
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

// Place smoke screen
void placeSmokeScreen(Player *player, int centerRow, int centerCol) {
    printf("Smoke screen placed at %c%d:\n", 'A' + centerCol, centerRow + 1);
    for (int i = centerRow ; i <= centerRow + 1; i++) {
        for (int j = centerCol ; j <= centerCol + 1; j++) {
            if (i >= 0 && i < SIZE && j >= 0 && j < SIZE) {
                player->smoke[i][j] = true;
            }
        }
    }
    printf("Smoke screen deployed.\n");
}

// Main function
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
    initializeGrid(player1.smoke);
    initializeGrid(player2.grid);
    initializeGrid(player2.hits);
    initializeGrid(player2.smoke);

    player1.shipsSunk = player2.shipsSunk = 0;
    player1.smokeScreens = player2.smokeScreens = 0;
    player1.torpedoAvailable = player2.torpedoAvailable = false;
    player1.artilleryAvailable = player2.artilleryAvailable = false;
    player1.radarSweepsRemaining = player2.radarSweepsRemaining = MAX_RADAR_SWEEPS;
  // Player ship placement
    for (int i = 0; i < 2; i++) {
        Player *player = players[i];
        printf("%s, place your ships:\n", player->name);
        for (int j = 0; j < MAX_SHIPS; j++) {
            char coordinate[4];
            char orientation[10];
            while (true) {
                displayGrid(player->grid, "Your Ships", false);
                printf("Enter coordinate for your %s (size %d): ", shipNames[j], shipSizes[j]);
                scanf("%3s", coordinate);

                int row, col;
                if (!parseCoordinates(coordinate, &row, &col)) {
                    printf("Invalid coordinates! Try again.\n");
                    continue;
                }

                printf("Enter orientation (vertical or horizontal): ");
                scanf("%9s", orientation);
                bool isVertical = strcmp(orientation, "vertical") == 0;

                if (canPlaceShip(player->grid, row, col, shipSizes[j], isVertical)) {
                    placeShip(player->grid, row, col, shipSizes[j], isVertical);
                    break;
                } else {
                    printf("Invalid placement! Try again.\n");
                }
            }
        }
    }

    // Gameplay loop
    while (player1.shipsSunk < MAX_SHIPS && player2.shipsSunk < MAX_SHIPS) {
        Player *currentPlayer = players[currentPlayerIndex];
        Player *opponent = players[1 - currentPlayerIndex];
    opponent->artilleryAvailable=false;
    opponent->torpedoAvailable=false;
        displayGrid(opponent->hits, "Opponent's Grid", true);
        printf("%s's turn! Choose an action (Fire, Artillery, Torpedo, Radar): ", currentPlayer->name);

        char action[20];
        scanf("%s", action);

        if (strcmp(action, "Fire") == 0) {
            char coordinates[4];
            printf("Enter coordinates to fire: ");
            scanf("%3s", coordinates);

            int row, col;
            char result[20];
            if (parseCoordinates(coordinates, &row, &col)) {
                if (fire(opponent, row, col, result)) {
                    if (checkShipSunk(opponent->grid, opponent->hits, row, col)) {
                        currentPlayer->shipsSunk++;
                        currentPlayer->torpedoAvailable = true;
                        currentPlayer->artilleryAvailable = true;
                        currentPlayer->smokeScreens++;
                        printf("You sunk a ship! Torpedo and Artillery are now available.\n");
                    }
                }
                printf("%s\n", result);
            } else {
                printf("Invalid coordinates! Try again.\n");
            }
        } else if (strcmp(action, "Artillery") == 0 && currentPlayer->artilleryAvailable) {
            char coordinates[4];
            printf("Enter center coordinates for artillery: ");
            scanf("%3s", coordinates);

            int row, col;
            if (parseCoordinates(coordinates, &row, &col)) {
                artillery(opponent, row, col);
                currentPlayer->artilleryAvailable = false;
            } else {
                printf("Invalid coordinates! Try again.\n");
            }
        } else if (strcmp(action, "Torpedo") == 0 && currentPlayer->torpedoAvailable) {
            char type[10];
            printf("Enter target type (row/column): ");
            scanf("%9s", type);

            int index;
            printf("Enter target index (1-10): ");
            scanf("%d", &index);
            index--;

            if ((strcmp(type, "row") == 0 && index >= 0 && index < SIZE) ||
                (strcmp(type, "column") == 0 && index >= 0 && index < SIZE)) {
                torpedo(opponent, type, index);
                currentPlayer->torpedoAvailable = false;
            } else {
                printf("Invalid target! Try again.\n");
            }
        } else if (strcmp(action, "Radar") == 0 && currentPlayer->smokeScreens > 0) {
            char coordinates[4];
            printf("Enter center coordinates for radar sweep: ");
            scanf("%3s", coordinates);

            int row, col;
            if (parseCoordinates(coordinates, &row, &col)) {
                radarSweep(opponent, row, col);
                currentPlayer->smokeScreens--;
            } else {
                printf("Invalid coordinates! Try again.\n");
            }
        } else {
            printf("Invalid action or unavailable resource. Try again.\n");
        }

        currentPlayerIndex = 1 - currentPlayerIndex;
    }

    printf("%s wins!\n", player1.shipsSunk == MAX_SHIPS ? player2.name : player1.name);
   
    return 0;
}
