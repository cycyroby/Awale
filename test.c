#include <stdio.h>

#include "awale.c"

void gameInitTest() {
    printf("-----Game Initialization test-----\n");
    printf("4 should be printed 12 times : \n");

    int * holes;
    int numberOfHoles = 12;
    holes = malloc(sizeof(int)*numberOfHoles);
    initGame(holes, numberOfHoles);
    for (int i = 0; i<numberOfHoles; i++) {
        printf("%d,", holes[i]);
    }
    printf("\n");

    printf("-----end of game Initialization test-----\n");
}

int main(){
    gameInitTest();
}