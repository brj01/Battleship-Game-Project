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
} Player;

void initializeGrid(bool grid[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            grid[i][j] = false; 
        }
    }
}

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
    for
    
     (int i = 0; i < 30; i++) printf("\n");
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
bool smoke(int smokeCounter, bool smokeScreen[10][10]) {
    if (smokeCounter <= 0) {
        printf("you cant currently use this function\n");
        return false; }
    
    char coordinate[3];

    printf("enter the top left coordinate of the 2x2 area: ");
    scanf("%s\n", *coordinate);

    int row = atoi(coordinate + 1) - 1;
    int col = coordinate[0] - 'A';

    if (row + 1 < 10 && col + 1 < 10) {
        smokeScreen[row][col] = true;
        smokeScreen[row + 1][col] = true;
        smokeScreen[row][col + 1] = true;
        smokeScreen[row + 1][col + 1] = true;

        return true; }
    else {
        printf("the coordinates of the 2x2 smoke screen are out of bounds");
        return false; }
}

bool torpedo(bool sankShipLastTurn, bool array [10][10], bool hit [10][10]) {
    if(!sankShipLastTurn) {
        printf("you havent unlocked this feature yet");
        return false; }
    
    int rowOrCol; //0 for row, 1 for col
    bool chosen[10]; //row or column chosen
    printf("do you want a row or column? ");
    scanf("%d\n");

    if (rowOrCol == 0) {
        int row;
        printf("what row do you want to torpedo? ");
        scanf("%d\n", row);

        if(row >=0 && row < 10) {
            for(int j = 0; j < 10; j++) {
                chosen[j] = fire(array[row][j]);
                hit[row][j] = true; }

            for (int i = 0; i < chosen; i++) {
                if (chosen[i]) return true;
                else continue; }
            return false;
        }
        else printf("row out of bound and not on grid");
        return false;
    }
    else {
        char column[2];
        printf("what column do you want to torpedo? ");
        scanf("%s\n", *column);

        int col = column[0] - 'A';

        if(col >=0 && col < 10) {
            for(int i = 0; i < 10; i++) {
                chosen[i] = fire(array[i][col]); 
                hit[i][col] = true; }

            for (int i = 0; i < chosen; i++) {
                if (chosen[i]) return true;
                else continue; }
            return false;
        }
        else printf("column out of bound and not on grid");
        return false;
    }
}
bool artillery(bool sankShipLastTurn, bool array [10][10]) {
    if (!sankShipLastTurn) {
        printf("you cant currently use this function\n");
        return false; }
    
    char coordinate[3];

    printf("enter the top left coordinate of the 2x2 area: ");
    scanf("%s ", *coordinate);

    int row = atoi(coordinate + 1) - 1;
    int col = coordinate[0] - 'A';

    
    bool b1 = fire(array,array[row][col]);

    if(row + 1 < 10 && col + 1 < 10) {
        bool b2 = fire(array[row + 1][col], array);
        bool b3 = fire(array[row][col + 1], array);
        bool b4 = fire(array[row + 1][col + 1]); 
        
        if (b1 | b2 | b3 | b4) {
            return true; }

    else return false; }    
}

int main() {
    Player player1, player2;
    bool gameRunning = true;
    int currentPlayerIndex;
    Player* players[2];
    
    
    printf("Enter name for Player 1: ");
    scanf("%s", player1.name);
    printf("Enter name for Player 2: ");
    scanf("%s", player2.name);


    initializeGrid(player1.grid);
    initializeGrid(player2.grid);
    player1.radarSweeps = 3;
    player2.radarSweeps = 3;
    player1.smokeScreens = 0;
    player2.smokeScreens = 0;
    player1.shipsSunk = 0;
    player2.shipsSunk = 0;


    srand(time(NULL));
    currentPlayerIndex = rand() % 2;
    players[0] = &player1;
    players[1] = &player2;

 
    for (int i = 0; i < 2; i++) {
        Player* player = players[i];
        printf("%s, place your ships:\n", player->name);

        for (int size = 5; size > 1; size--) { 
            char orientation[10];
            char coordinate[3];
            while (true) {
                displayGrid(player->grid, false);
                printf("Enter coordinate for a %d-cell ship (e.g., B3): ", size);
                scanf("%s", coordinate);
                printf("Vertical or horizontal? ");
                scanf("%s", orientation);
                int row = coordinate[1] - '1';
                int col = coordinate[0] - 'A';

                if (canPlaceShip(player->grid, row, col, size, orientation)) {
                    placeShip(player->grid, row, col, size, orientation);
                    break; 
                } else {
                    printf("Invalid position. Try again.\n");
                }
            }
        }
        printf("All ships placed for %s.\n", player->name);
    }


    while (gameRunning) {
        Player* currentPlayer = players[currentPlayerIndex];
        Player* opponent = players[1 - currentPlayerIndex];

        displayGrid(opponent->hits, true); 
        printf("%s's turn. Available moves: Fire, Radar, Smoke, Artillery, Torpedo\n", currentPlayer->name);

        char command[50];
        scanf(" %[^\n]", command); 

        if (strcmp(command, "Fire") == 0) {
            char coordinates[3];
            printf("Enter coordinates (e.g., B3): ");
            scanf("%s", coordinates);
            fire(coordinates, opponent);
        } else if (strcmp(command, "Radar") == 0) {
            char coordinates[3];
            printf("Enter coordinates for radar sweep: ");
            scanf("%s", coordinates);
            radarSweep(opponent, coordinates);
        } else if (strcmp(command, "Smoke") == 0) {
            char coordinates[3];
            printf("Enter the top left coordinate of the 2x2 area: ");
            scanf("%s", coordinates);
            smoke(currentPlayer, coordinates);
        } else if (strcmp(command, "Artillery") == 0) {
            char coordinates[3];
            printf("Enter the top left coordinate of the 2x2 area: ");
            scanf("%s", coordinates);
            artillery(opponent, coordinates);
        } else if (strcmp(command, "Torpedo") == 0) {
            int rowOrCol;
            printf("Do you want to target a row (0) or a column (1)? ");
            scanf("%d", &rowOrCol);
            torpedo(opponent, rowOrCol);
        }

        if (opponent->shipsSunk >= MAX_SHIPS) {
            printf("%s wins!\n", currentPlayer->name);
            gameRunning = false;
        }

        currentPlayerIndex = 1 - currentPlayerIndex; // Switch players
    }

    return 0;
}
