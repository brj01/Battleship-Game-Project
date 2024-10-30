#include <stdio.h>
#include <stdbool.h>

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
        else printf("column out of bound and not on grid");
        return false;
    }
}
