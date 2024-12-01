int main() {
    Player player1, player2;
    Player *players[2] = {&player1, &player2};
    int currentPlayerIndex = 0;
    bool gameRunning = true;

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
    player1.radarSweeps = player2.radarSweeps = 3;
    player1.smokeScreens = player2.smokeScreens = 3;

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
                printf("Vertical or horizontal? ");
                scanf("%9s", orientation); // Limit to 9 characters

                int row = atoi(coordinate + 1) - 1;
                int col = coordinate[0] - 'A';

                if (row >= 0 && row < SIZE && col >= 0 && col < SIZE &&
                    canPlaceShip(player->grid, row, col, size, orientation)) {
                    placeShip(player->grid, row, col, size, orientation);
                    break;
                } else {
                    printf("Invalid position. Try again.\n");
                }
            }
        }
        clearScreen();
    }

    // Game loop
    while (gameRunning) {
        Player *currentPlayer = players[currentPlayerIndex];
        Player *opponent = players[1 - currentPlayerIndex];

        displayGridEasy(opponent->hits, opponent->grid);
        printf("%s's turn. Choose an action (Fire, Radar, Smoke): ", currentPlayer->name);

        char command[10];
        scanf("%9s", command); // Limit to 9 characters

        if (strcmp(command, "Fire") == 0) {
            char coordinates[4];
            printf("Enter coordinates (e.g., B3): ");
            scanf("%3s", coordinates); // Limit to 3 characters

            int row = atoi(coordinates + 1) - 1;
            int col = coordinates[0] - 'A';

            if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
                if (fire(coordinates, opponent->grid)) {
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
            } else {
                printf("Invalid coordinates. Try again.\n");
            }
        } else if (strcmp(command, "Radar") == 0) {
            char coordinates[4];
            printf("Enter coordinates for radar sweep (e.g., B3): ");
            scanf("%3s", coordinates);
            radarSweep(currentPlayer, coordinates);
        } else if (strcmp(command, "Smoke") == 0) {
            char coordinates[4];
            printf("Enter coordinates for smoke screen (e.g., B3): ");
            scanf("%3s", coordinates);
            smoke(currentPlayer, coordinates);
        } else {
            printf("Unknown command. Try again.\n");
        }

        // Switch turn
        if (gameRunning) {
            currentPlayerIndex = 1 - currentPlayerIndex;
        }
    }

    return 0;
}
