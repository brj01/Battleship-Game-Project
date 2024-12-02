#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define SIZE 10
#define MAX_SHIPS 5

typedef struct {
    char name[50];
    bool grid[SIZE][SIZE];
    bool hits[SIZE][SIZE];
    int shipsSunk;
    int smokeScreens;
    int torpedoAvailable;
    int artilleryAvailable;
    int radarSweepsRemaining;
} Player;

typedef struct {
    char name[50];
    bool grid[SIZE][SIZE];
    bool hits[SIZE][SIZE];
    int shipsSunk;
    int smokeScreens;
    int torpedoAvailable;
    int artilleryAvailable;
    int radarSweepsRemaining;
    int difficulty; // 1: Easy, 2: Medium, 3: Hard
    int lastHitRow;
    int lastHitCol;
    bool lastHitSuccess;
} Bot;

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

void placeShip(bool grid[SIZE][SIZE], int row, int col, int size, bool isVertical) {
    for (int i = 0; i < size; i++) {
        if (isVertical)
            grid[row + i][col] = true;
        else
            grid[row][col + i] = true;
    }
}

bool checkShipSunk(bool grid[SIZE][SIZE], bool hits[SIZE][SIZE], int row, int col) {
    if (!grid[row][col]) return false;

    // Check horizontally
    int startCol = col, endCol = col;
    while (startCol > 0 && grid[row][startCol - 1]) startCol--;
    while (endCol < SIZE - 1 && grid[row][endCol + 1]) endCol++;

    bool isHorizontalSunk = true;
    for (int c = startCol; c <= endCol; c++) {
        if (!hits[row][c]) {
            isHorizontalSunk = false;
            break;
        }
    }

    // Check vertically
    int startRow = row, endRow = row;
    while (startRow > 0 && grid[startRow - 1][col]) startRow--;
    while (endRow < SIZE - 1 && grid[endRow + 1][col]) endRow++;

    bool isVerticalSunk = true;
    for (int r = startRow; r <= endRow; r++) {
        if (!hits[r][col]) {
            isVerticalSunk = false;
            break;
        }
    }

    return isHorizontalSunk || isVerticalSunk;
}

void artillery(Player *opponent, int centerRow, int centerCol) {
    printf("Artillery strike at %c%d:\n", 'A' + centerCol, centerRow + 1);
    for (int i = centerRow - 1; i <= centerRow + 1; i++) {
        for (int j = centerCol - 1; j <= centerCol + 1; j++) {
            if (i >= 0 && i < SIZE && j >= 0 && j < SIZE) {
                opponent->hits[i][j] = true;
                opponent->grid[i][j] = false;
            }
        }
    }
    printf("Artillery strike completed.\n");
}

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
    for (int i = centerRow - 1; i <= centerRow + 1; i++) {
        for (int j = centerCol - 1; j <= centerCol + 1; j++) {
            if (i >= 0 && i < SIZE && j >= 0 && j < SIZE) {
                if (opponent->grid[i][j]) {
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

// Bot fires based on difficulty
void easyBotFire(Bot *bot, Player *opponent) {
    int row, col;
    do {
        row = rand() % SIZE;
        col = rand() % SIZE;
    } while (opponent->hits[row][col]);

    char result[20];
    fire(opponent, row, col, result);
    printf("Bot fires at %c%d: %s\n", 'A' + col, row + 1, result);
}

void mediumBotFire(Bot *bot, Player *opponent) {
    int row, col;

    if (bot->lastHitSuccess) {
        if (bot->lastHitRow > 0 && !opponent->hits[bot->lastHitRow - 1][bot->lastHitCol]) {
            row = bot->lastHitRow - 1;
            col = bot->lastHitCol;
        } else if (bot->lastHitRow < SIZE - 1 && !opponent->hits[bot->lastHitRow + 1][bot->lastHitCol]) {
            row = bot->lastHitRow + 1;
            col = bot->lastHitCol;
        } else if (bot->lastHitCol > 0 && !opponent->hits[bot->lastHitRow][bot->lastHitCol - 1]) {
            row = bot->lastHitRow;
            col = bot->lastHitCol - 1;
        } else if (bot->lastHitCol < SIZE - 1 && !opponent->hits[bot->lastHitRow][bot->lastHitCol + 1]) {
            row = bot->lastHitRow;
            col = bot->lastHitCol + 1;
        } else {
            do {
                row = rand() % SIZE;
                col = rand() % SIZE;
            } while (opponent->hits[row][col]);
        }
    } else {
        do {
            row = rand() % SIZE;
            col = rand() % SIZE;
        } while (opponent->hits[row][col]);
    }

    char result[20];
    bot->lastHitSuccess = fire(opponent, row, col, result);
    bot->lastHitRow = row;
    bot->lastHitCol = col;
    printf("Bot fires at %c%d: %s\n", 'A' + col, row + 1, result);
}

void hardBotFire(Bot *bot, Player *opponent) {
    int row, col;
    if (bot->torpedoAvailable) {
        printf("Bot uses torpedo!\n");
        int leastHits = SIZE;
        int targetIndex = 0;
        char direction[10];

        for (int i = 0; i < SIZE; i++) {
            int rowHits = 0, colHits = 0;
            for (int j = 0; j < SIZE; j++) {
                if (opponent->hits[i][j])
                    rowHits++;
                if (opponent->hits[j][i])
                    colHits++;
            }

            if (rowHits < leastHits) {
                leastHits = rowHits;
                targetIndex = i;
                strcpy(direction, "row");
            }

            if (colHits < leastHits) {
                leastHits = colHits;
                targetIndex = i;
                strcpy(direction, "column");
            }
        }

        torpedo(opponent, direction, targetIndex);
        bot->torpedoAvailable = 0;
    } else if (bot->lastHitSuccess) {
        mediumBotFire(bot, opponent);
    } else {
        do {
            row = rand() % SIZE;
            col = rand() % SIZE;
        } while (opponent->hits[row][col]);

        char result[20];
        fire(opponent, row, col, result);
        printf("Bot fires at %c%d: %s\n", 'A' + col, row + 1, result);
    }
}

// Bot places its ships
void botPlaceShips(Bot *bot) {
    for (int i = 0; i < MAX_SHIPS; i++) {
        bool placed = false;
        while (!placed) {
            int row = rand() % SIZE;
            int col = rand() % SIZE;
            bool isVertical = rand() % 2;

            if (canPlaceShip(bot->grid, row, col, shipSizes[i], isVertical)) {
                placeShip(bot->grid, row, col, shipSizes[i], isVertical);
                placed = true;
            }
        }
    }
}

int main() {
    srand(time(NULL));

    Player player;
    Bot bot;

    char wantMissiles[10]; // Correctly declare as an array
    bool showMissiles = false;

    printf("Do you want to display missiles? (Y/N) ");
    scanf("%9s", wantMissiles);

    if (wantMissiles[0] == 'Y' || wantMissiles[0] == 'y') { // Access first character properly
        showMissiles = true;
    }

    printf("Enter your name: ");
    scanf("%49s", player.name);

    printf("Select bot difficulty (1 = Easy, 2 = Medium, 3 = Hard): ");
    scanf("%d", &bot.difficulty);

    initializeGrid(player.grid);
    initializeGrid(player.hits);
    initializeGrid(bot.grid);
    initializeGrid(bot.hits);

    player.shipsSunk = bot.shipsSunk = 0;
    player.smokeScreens = player.torpedoAvailable = player.artilleryAvailable = 0;
    player.radarSweepsRemaining = 3;

    bot.smokeScreens = bot.torpedoAvailable = bot.artilleryAvailable = 0;
    bot.radarSweepsRemaining = 3;

    printf("Place your ships:\n");
    for (int i = 0; i < MAX_SHIPS; i++) {
        char coordinate[4];
        char orientation[10];
        while (true) {
            displayGrid(player.grid, "Your Ships", showMissiles);
            printf("Enter coordinate for your %s (size %d): ", shipNames[i], shipSizes[i]);
            scanf("%3s", coordinate);

            int row, col;
            if (!parseCoordinates(coordinate, &row, &col)) {
                printf("Invalid coordinates! Try again.\n");
                continue;
            }

            printf("Enter orientation (vertical or horizontal): ");
            scanf("%9s", orientation);
            bool isVertical = strcmp(orientation, "vertical") == 0;

            if (canPlaceShip(player.grid, row, col, shipSizes[i], isVertical)) {
                placeShip(player.grid, row, col, shipSizes[i], isVertical);
                break;
            } else {
                printf("Invalid placement! Try again.\n");
            }
        }
    }

    printf("Bot is placing ships...\n");
    botPlaceShips(&bot);

    int currentPlayerIndex = 0;

    while (player.shipsSunk < MAX_SHIPS && bot.shipsSunk < MAX_SHIPS) {
        if (currentPlayerIndex == 0) { // Player's turn
            displayGrid(bot.hits, "Opponent's Grid", true);
            char command[20];
            printf("Your turn! Choose action (Fire, Radar, Smoke, Torpedo, Artillery): ");
            scanf("%s", command);

            if (strcmp(command, "Fire") == 0) {
                char coordinates[4];
                printf("Enter coordinates to fire: ");
                scanf("%3s", coordinates);

                int row, col;
                char result[20];
                if (parseCoordinates(coordinates, &row, &col)) {
                    Player botAsPlayer = *(Player *)&bot; // Cast Bot to Player for fire compatibility
                    if (fire(&botAsPlayer, row, col, result)) {
                        if (checkShipSunk(bot.grid, bot.hits, row, col)) {
                            bot.shipsSunk++;
                            printf("You sunk the opponent's ship!\n");
                        }
                    }
                    printf("%s\n", result);
                } else {
                    printf("Invalid coordinates! Try again.\n");
                }
            } else if (strcmp(command, "Radar") == 0 && player.radarSweepsRemaining > 0) {
                char coordinates[4];
                printf("Enter coordinates for radar sweep: ");
                scanf("%3s", coordinates);

                int row, col;
                if (parseCoordinates(coordinates, &row, &col)) {
                    Player botAsPlayer = *(Player *)&bot; // Cast Bot to Player for radarSweep compatibility
                    radarSweep(&botAsPlayer, row, col);
                    player.radarSweepsRemaining--;
                } else {
                    printf("Invalid coordinates! Try again.\n");
                }
            } else if (strcmp(command, "Smoke") == 0 && player.smokeScreens > 0) {
                printf("Smoke screen deployed (dummy functionality).\n");
                player.smokeScreens--;
            } else if (strcmp(command, "Torpedo") == 0 && player.torpedoAvailable > 0) {
                printf("Torpedo fired (dummy functionality).\n");
                player.torpedoAvailable--;
            } else if (strcmp(command, "Artillery") == 0 && player.artilleryAvailable > 0) {
                printf("Artillery strike initiated (dummy functionality).\n");
                player.artilleryAvailable--;
            } else {
                printf("Invalid action or unavailable resource! Try again.\n");
            }
        } else { // Bot's turn
            switch (bot.difficulty) {
                case 1:
                    easyBotFire(&bot, &player);
                    break;
                case 2:
                    mediumBotFire(&bot, &player);
                    break;
                case 3:
                    hardBotFire(&bot, &player);
                    break;
            }
        }

        currentPlayerIndex = 1 - currentPlayerIndex; // Switch turns
    }

    printf("%s wins!\n", player.shipsSunk == MAX_SHIPS ? bot.name : player.name);

    return 0;
}
