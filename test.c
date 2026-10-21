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
    /*
    int * holes = malloc(sizeof(int)*12);

    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 0;
    holes[6] = 0;
    holes[7] = 0;
    holes[8] = 0;
    holes[9] = 0;
    holes[10] = 2;
    holes[11] = 0;

    printf("%d\n", isGameFinished(1, holes));*/

}