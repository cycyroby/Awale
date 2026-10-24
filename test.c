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
    printf("-----end of Game Initialization test-----\n\n");
}

void gameShowTest() {
    printf("-----Game Show test-----\n");

    int player = 1;
    int myScore = 1;
    int opponentScore = 2;
    printf("All holes have 4 seeds, I am player %d, my score is %d, my opponents score is %d\nplayer 1 has 0 in first hole\nplayer 2 has 2 in his last hole\nplayer 1 has 1 in second hole\nplayer 2 has 3 in his second to last hole\n", player, myScore, opponentScore);

    int * holes;
    int numberOfHoles = 12;
    holes = malloc(sizeof(int)*numberOfHoles);
    initGame(holes, numberOfHoles);
    holes[0] = 0; // player 1 has 0 in first hole
    holes[6] = 2; // player 2 has 2 in his last hole
    holes[1] = 1; // player 1 has 1 in second hole
    holes[7] = 3; // player 2 has 3 in his second to last hole
    showGame(holes, numberOfHoles, player, opponentScore, myScore);

    printf("\n");
    printf("-----end of Game Show test-----\n\n");
}

int main(){
    gameInitTest();

    
    /*int * holes = malloc(sizeof(int)*12);

    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 0;
    holes[6] = 0;
    holes[7] = 2;
    holes[8] = 1;
    holes[9] = 3;
    holes[10] = 2;
    holes[11] = 0;

    printf("%d\n", collectSeed(2, holes, 0, 10));

    //printf("%d\n", isGameFinished(1, holes));*/

    gameShowTest();
}