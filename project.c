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
    int smokeScreens; 
    bool smoke[SIZE][SIZE]; 
} Player;

const int shipSizes[] = {5, 4, 3, 3, 2};
const char *shipNames[] = {"Carrier", "Battleship", "Destroyer 1", "Destroyer 2", "Submarine"};

void initializeGrid(bool grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = false;
        }
    }
}

void initializeSmoke(bool grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = false;
        }
    }
}

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

bool parseCoordinates(const char *input, int *row, int *col) {
    if (strlen(input) < 2 || !isalpha(input[0]) || !isdigit(input[1])) {
        return false;
    }
    *col = toupper(input[0]) - 'A';
    *row = atoi(input + 1) - 1;

    return (*row >= 0 && *row < SIZE && *col >= 0 && *col < SIZE);
}

void radarSweep(Player *opponent, const char *coordinates, char *result) {
    int row, col;
    if (!parseCoordinates(coordinates, &row, &col)) {
        strcpy(result, "Invalid coordinates for radar sweep.");
        return;
    }

    if (opponent->smoke[row][col]) {
        strcpy(result, "Radar sweep blocked by smoke.");
    } else {
        sprintf(result, "Radar sweep at %c%d: %s", 'A' + col, row + 1,
                opponent->grid[row][col] ? "Ship detected!" : "No ship detected.");
    }
}

void artillery(Player *opponent, const char *coordinates, char *result) {
    int row, col;
    if (!parseCoordinates(coordinates, &row, &col)) {
        strcpy(result, "Invalid coordinates for artillery.");
        return;
    }

    int hits = 0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int r = row + i;
            int c = col + j;
            if (r >= 0 && r < SIZE && c >= 0 && c < SIZE) {
                if (opponent->grid[r][c] && !opponent->smoke[r][c]) {
                    opponent->grid[r][c] = false;
                    opponent->hits[r][c] = true;
                    hits++;
                }
            }
        }
    }
    sprintf(result, "Artillery fired at %c%d: %d hits!", 'A' + col, row + 1, hits);
}

void smokeScreen(Player *player, const char *coordinates, char *result) {
    int row, col;
    if (!parseCoordinates(coordinates, &row, &col)) {
        strcpy(result, "Invalid coordinates for smoke screen.");
        return;
    }

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int r = row + i;
            int c = col + j;
            if (r >= 0 && r < SIZE && c >= 0 && c < SIZE) {
                player->smoke[r][c] = true;
            }
        }
    }
    sprintf(result, "Smoke screen deployed at %c%d.", 'A' + col, row + 1);
}

int main() {
    Player player1, player2;
    Player *players[2] = {&player1, &player2};
    int currentPlayerIndex = 0;

    printf("Enter name for Player 1: ");
    scanf("%49s", player1.name);

    initializeGrid(player1.grid);
    initializeGrid(player1.hits);
    initializeSmoke(player1.smoke);

    initializeGrid(player2.grid);
    initializeGrid(player2.hits);
    initializeSmoke(player2.smoke);

    player1.shipsSunk = player2.shipsSunk = 0;

    printf("%s, place your ships:\n", player1.name);
    for (int i = 0; i < MAX_SHIPS; i++) {
        int row = i; 
        int col = i;
        placeShip(player1.grid, row, col, shipSizes[i], true);
    }

    while (true) {
        Player *currentPlayer = players[currentPlayerIndex];
        Player *opponent = players[1 - currentPlayerIndex];

        displayGrid(opponent->hits, "Opponent's Grid", true);
        printf("%s's turn. Choose an action (Fire, Torpedo, Radar, Smoke, Artillery): ", currentPlayer->name);

        char command[50];
        scanf("%s", command);

        if (strcmp(command, "Radar") == 0) {
            char coordinates[4], result[50];
            printf("Enter coordinates for radar sweep: ");
            scanf("%3s", coordinates);
            radarSweep(opponent, coordinates, result);
            printf("%s\n", result);
        } else if (strcmp(command, "Artillery") == 0) {
            char coordinates[4], result[50];
            printf("Enter coordinates for artillery: ");
            scanf("%3s", coordinates);
            artillery(opponent, coordinates, result);
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

        currentPlayerIndex = 1 - currentPlayerIndex; // Switch turns
    }

    return 0;
}
