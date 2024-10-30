#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 10
#define MAX_SHIPS 5

typedef struct {
    char name[50];
    bool grid[10][10];
    bool hits[10][10];
    int shipsSunk;
    int radarSweeps;
    int smokeScreens;
} 
Player;


void radarSweep(int counter, bool array[10][10], const char *coordinates) {
    int column = coordinates[0] - 'A';
    int row = atoi(coordinates + 1) - 1;


    if (counter <= 3) {
        bool t1 = array[row][column];

       
        if (row + 1 < 10 && column + 1 < 10 ) {
            bool t2 = array[row + 1][column + 1]  ;
            bool t3 = array[row][column + 1];
            bool t4 = array[row + 1][column]; 
        }
    }
}
void fire(const char *coordinates, bool array[10][10]) {
    int column = coordinates[0] - 'A'; 
    int row = atoi(coordinates + 1) - 1; 
    if (row >= 0 && row < 10 && column >= 0 && column < 10) {
        if (array[row][column]) {
            printf("hit\n");
        } else {
            printf("miss\n");
        }
    } else {
        printf("Invalid coordinates\n");
    }
}
void initializeGrid(bool grid[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            grid[i][j] = false; 
        }
    }
}

void displayGridHard(bool grid[10][10]) {
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", i);
        for (int j = 0; j < 10; j++) {
            printf("%c ", grid[i][j] ? '~' : 'X');
        }
        printf("\n");
    }
}

// Display grid for easy mode (hits and misses are shown)
void displayGridEasy(bool gridHits[10][10], bool shipsLocation[10][10]) {
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", i);
        for (int j = 0; j < 10; j++) {
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

// Check if a ship can be placed at the specified location and orientation
bool canPlaceShip(bool grid[10][10], int row, int col, int size, const char* orientation) {
    if (strcmp(orientation, "vertical") == 0) {
        if (row + size > 10) return false;
        for (int i = 0; i < size; i++) {
            if (grid[row + i][col]) return false;
        }
    } else if (strcmp(orientation, "horizontal") == 0) {
        if (col + size > 10) return false;
        for (int i = 0; i < size; i++) {
            if (grid[row][col + i]) return false;
        }
    }
    return true;
}

// Place the ship on the grid
void placeShip(bool grid[10][10], int row, int col, int size, const char* orientation) {
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

// Clears the screen (simulate by printing newlines)
void clearScreen() {
    for (int i = 0; i < 30; i++) printf("\n");
}

void fire(const char *coordinates, bool array[10][10]) {
    int column = coordinates[0] - 'A'; 
    int row = atoi(coordinates + 1) - 1; 
    if (row >= 0 && row < 10 && column >= 0 && column < 10) {
        if (array[row][column]) {
            printf("Hit\n");
            array[row][column] = false; // Mark hit for the opponent
        } else {
            printf("Miss\n");
        }
    } else {
        printf("Invalid coordinates\n");
    }
}

void radarSweep(Player* currentPlayer, const char *coordinates) {
    int column = coordinates[0] - 'A';
    int row = atoi(coordinates + 1) - 1;

    if (currentPlayer->radarSweeps > 0) {
        currentPlayer->radarSweeps--;
        printf("Radar Sweep at %s: %s\n", coordinates, currentPlayer->grid[row][column] ? "Ship detected!" : "No ship detected.");
    } else {
        printf("No radar sweeps left.\n");
    }
}

bool smoke(Player* currentPlayer, const char *coordinates) {
    if (currentPlayer->smokeScreens <= 0) {
        printf("You can't currently use this function\n");
        return false; 
    }
    
    int row = atoi(coordinates + 1) - 1;
    int col = coordinates[0] - 'A';

    if (row + 1 < 10 && col + 1 < 10) {
        currentPlayer->smokeScreens--;
        printf("Smoke screen deployed at %s\n", coordinates);
        return true;
    } else {
        printf("The coordinates of the 2x2 smoke screen are out of bounds\n");
        return false; 
    }
}

bool torpedo(Player* currentPlayer, bool array[10][10], bool hit[10][10]) {
    int rowOrCol;
    printf("Do you want a row (0) or column (1)? ");
    scanf("%d", &rowOrCol);

    if (rowOrCol == 0) {
        int row;
        printf("What row do you want to torpedo? ");
        scanf("%d", &row);

        if (row >= 0 && row < 10) {
            for (int j = 0; j < 10; j++) {
                fire(array[row][j]);
                hit[row][j] = true; 
            }
            return true;
        } else {
            printf("Row out of bounds\n");
            return false;
        }
    } else {
        char column[2];
        printf("What column do you want to torpedo? ");
        scanf("%s", column);

        int col = column[0] - 'A';

        if (col >= 0 && col < 10) {
            for (int i = 0; i < 10; i++) {
                fire(array[i][col]);
                hit[i][col] = true; 
            }
            return true;
        } else {
            printf("Column out of bounds\n");
            return false;
        }
    }
}

bool artillery(Player* currentPlayer, const char *coordinates) {
    int row = atoi(coordinates + 1) - 1;
    int col = coordinates[0] - 'A';

    if (row + 1 < 10 && col + 1 < 10) {
        fire(array, array[row][col]);
        fire(array, array[row + 1][col]);
        fire(array, array[row][col + 1]);
        fire(array, array[row + 1][col + 1]);
        return true;
    } else {
        printf("Coordinates are out of bounds\n");
        return false; 
    }
}

