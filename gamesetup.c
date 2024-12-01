#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Display grid for hard mode (only hits are shown)
void displaygridhard(bool grid[10][10]) {
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", i);
        for (int j = 0; j < 10; j++) {
            if (grid[i][j]) printf("~ ");
            else printf("X ");
        }
        printf("\n");
    }
}

// Display grid for easy mode (hits and misses are shown)
void displaygrideasy(bool gridhits[10][10], bool shipslocation[10][10]) {
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", i);
        for (int j = 0; j < 10; j++) {
            if (gridhits[i][j]) printf("~ ");
            else if (shipslocation[i][j]) printf("M ");
            else printf("X ");
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

// Function to add ships to the grid
void addships(bool grid[10][10]) {
    char read[3];
    char orientation[10];
    int coordinate2;
    char coordinate;
    
    for (int i = 5; i > 1; i--) {  // Ship sizes from 5 down to 2
        while (1) {  // Loop until a valid position is entered
            printf("Enter coordinate of %d-cell ship (e.g., B3): ", i);
            scanf("%s", read);
            coordinate = read[0];
            coordinate2 = atoi(&read[1]) - 1;  // Convert to zero-based index

            printf("Vertical or horizontal? ");
            scanf("%s", orientation);

            // Check if placement is possible
            if (canPlaceShip(grid, coordinate2, coordinate - 'A', i, orientation)) {
                placeShip(grid, coordinate2, coordinate - 'A', i, orientation);
                displaygridhard(grid);  // Show the grid for confirmation after each ship
                break;  // Exit loop for current ship size
            } else {
                printf("Does not fit or overlaps with another ship. Choose another coordinate.\n");
            }
        }
    }
}

// Clears the screen (simulate by printing newlines)
void clearscreen() {
    for (int i = 0; i < 30; i++) printf("\n"); }

int main() {
    bool grid1[10][10] = {false};
    bool gridhits[10][10] = {false};
    bool shipslocation[10][10] = {false};

    addships(grid1);  // Test ship placement
    displaygridhard(grid1);  // Final display for confirmation
    
    return 0;
}
