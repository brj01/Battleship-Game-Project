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
}bool fire(const char *coordinates, bool array[10][10]) {
    int column = coordinates[0] - 'A'; 
    int row = atoi(coordinates + 1) - 1; 

    if (row >= 0 && row < 10 && column >= 0 && column < 10) {
        if (array[row][column]) {
            return true;
        } else {
           return false;
        }
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

bool artillery(bool sankShipLastTurn, bool array [10][10]) {
    if (!sankShipLastTurn) {
        printf("you cant currently use this function\n");
        return false; }
    
    char coordinate[3];

    printf("enter the top left coordinate of the 2x2 area: ");
    scanf("%s ", *coordinate);

    int row = atoi(coordinate + 1) - 1;
    int col = coordinate[0] - 'A';

    
    bool b1 = fire(array[row][col], array);

    if(row + 1 < 10 && col + 1 < 10) {
        bool b2 = fire(array[row + 1][col], array);
        bool b3 = fire(array[row][col + 1], array);
        bool b4 = fire(array[row + 1][col + 1]); 
        
        if (b1 | b2 | b3 | b4) {
            return true; }

    else return false; }    
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
    player1.smokeScreens = 3;  
    player2.smokeScreens = 3; 
    player1.shipsSunk = 0;
    player2.shipsSunk = 0;

 
  
    players[0] = &player1;
    players[1] = &player2;

    for (int i = 0; i < 2; i++) {
        Player* player = players[i];
        printf("%s, place your ships:\n", player->name);
        
        for (int size = 5; size > 1; size--) {
            char orientation[10];
            char coordinate[3];
            while (true) {
                displayGridHard(player->grid);
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
                    printf("Invalid position.\n");
                }
            }
        }
        printf("All ships placed for %s.\n", player->name);
             clearscreen();
    }

    while (gameRunning) {
        Player* currentPlayer = players[currentPlayerIndex];
        Player* opponent = players[1 - currentPlayerIndex];

        displayGridEasy(opponent->hits, opponent->grid);
        printf("%s's turn. Fire, Radar, Smoke, Artillery, Torpedo\n", currentPlayer->name);
        
        char command[50];
        scanf(" %[^\n]", command); 

        if (strcmp(command, "Fire") == 0) {
            char coordinates[3];
            printf("Enter coordinates (e.g., B3): ");
            scanf("%s", coordinates);
               if (fire(coordinates, opponent->grid)) { 
            currentPlayer->shipsSunk++;
            printf("Hit! %s has sunk a ship!\n", opponent->name);
           
            checkShipsSunk(opponent); 
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
            printf("Enter the top left coordinate of the 2x2 area: ");
            scanf("%s", coordinates);
            smoke(currentPlayer, coordinates);
       
        } else if (strcmp(command, "Artillery") == 0) {
            char coordinates[3];
            printf("Enter the top left coordinate of the 2x2 area: ");
            scanf("%s", coordinates);
            artillery(currentPlayer, coordinates);
        } else if (strcmp(command, "Torpedo") == 0) {
            torpedo(currentPlayer, opponent->grid, opponent->hits);
        }

        if (opponent->shipsSunk >= MAX_SHIPS) {
            printf("%s wins!\n", currentPlayer->name);
            gameRunning = false;
        }

        currentPlayerIndex = 1 - currentPlayerIndex; 
             clearscreen();

checkShipsSunk(opponent);

if (opponent->shipsSunk >= MAX_SHIPS) {
    printf("%s wins!\n", currentPlayer->name);
    gameRunning = false;
}
    }
    return 0; }

