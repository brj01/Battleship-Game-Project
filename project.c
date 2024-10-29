#include <stdio.h>
#include <stdbool.h>

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



