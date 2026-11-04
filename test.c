#include <stdio.h>
#include <stdbool.h>

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
    printf("-----end of Game Initialization test-------------------------------------------------------------------------\n\n");
}

void gameShowTest() {
    printf("-----Game Show test-----\n");

    int player = 1;
    int myScore = 1;
    int opponentScore = 2;
    printf("All holes have 4 seeds, my score is %d, my opponents score is %d\nplayer 1 has 0 in first hole\nplayer 2 has 2 in his last hole\nplayer 1 has 1 in second hole\nplayer 2 has 3 in his second to last hole\n", myScore, opponentScore);

    int * holes;
    int numberOfHoles = 12;
    holes = malloc(sizeof(int)*numberOfHoles);
    initGame(holes, numberOfHoles);
    holes[0] = 0; // player 1 has 0 in first hole
    holes[1] = 1; // player 1 has 1 in second hole
    holes[6] = 2; // player 2 has 2 in his last hole
    holes[7] = 3; // player 2 has 3 in his second to last hole

    printf("I am player %d\n", player);
    showGame(holes, numberOfHoles, player, opponentScore, myScore);

    player = 2;
    printf("Same but I am player %d\n", player);
    showGame(holes, numberOfHoles, player, opponentScore, myScore);

    printf("\n");
    printf("-----end of Game Show test-------------------------------------------------------------------------\n\n");
}

void moveSeedTest() {
    printf("-----move Seed test-----\n");

    int * holes;
    int numberOfHoles = 12;
    int arrivalIndex;
    holes = malloc(sizeof(int)*numberOfHoles);
    initGame(holes, numberOfHoles);
    holes[2] = 13;

    showGame(holes, numberOfHoles, 1, 0, 0);
    printf("Move the seeds from my third hole\n");
    arrivalIndex = moveSeed(2, holes);
    printf("The arrival index should be 4\n");
    printf("The arrival index is %d\n", arrivalIndex);
    showGame(holes, numberOfHoles, 1, 0, 0);
    printf("Move the seeds from my third hole - it shouldn't change anything since there are no more seeds in that hole\n");
    arrivalIndex = moveSeed(2, holes);
    printf("The arrival index should be -1 because you can't play an empty hole\n");
    printf("The arrival index is %d\n", arrivalIndex);
    showGame(holes, numberOfHoles, 1, 0, 0);

    printf("-----end of move Seed test-------------------------------------------------------------------------\n\n");
}

void hasToBeFedTest() {
    printf("-----has to be fed test-----\n");

    printf("A player has to be fed if they don't have any sedds in their holes\n");
    printf("I am player 1\n");

    int * holes;
    int numberOfHoles = 12;
    holes = malloc(sizeof(int)*numberOfHoles);
    initGame(holes, numberOfHoles);
    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 0;
    showGame(holes, numberOfHoles, 1, 0, 0);

    bool hasTobeFed1;
    bool hasTobeFed2;

    hasTobeFed1 = hasToBeFed(1 ,holes);
    hasTobeFed2 = hasToBeFed(2 ,holes);

    if (hasTobeFed1) {
        printf("player 1 has to be fed : true\n");
    }
    else {
        printf("player 1 has to be fed : false\n");
    }
    if (hasTobeFed2) {
        printf("player 2 has to be fed : true\n");
    }
    else {
        printf("player 2 has to be fed : false\n");
    }
    
    printf("-----end of has to be fed test-------------------------------------------------------------------------\n\n");
}

void canFeedTest() {
    printf("-----can feed test-----\n");

    int numberOfHoles = 12;
    int * holes = malloc(sizeof(int)*numberOfHoles);

    int * possibleHoles = malloc(sizeof(int)*numberOfHoles);
    for (int i=0; i<numberOfHoles; i++){
        possibleHoles[i] = -1;
    }
    int myScore = 0;
    int opponentScore = 0;
    int playerNumber = 2;
    bool possible;

    printf("I am player %d\n\n", playerNumber);

    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 0;
    holes[6] = 12;
    holes[7] = 2;
    holes[8] = 1;
    holes[9] = 3;
    holes[10] = 2;
    holes[11] = 0;

    showGame(holes, numberOfHoles, playerNumber, opponentScore, myScore);

    possible = canFeed(1, holes, possibleHoles);
    
    printf("The possible hole indexes should be : 6, 9, 10\n");
    if(possible){
        printf("The possible hole indexes are :");
        for (int i=0; i<numberOfHoles; i++){
            if (possibleHoles[i] != -1){
                printf("%d, ", possibleHoles[i]);
            }
        }
        printf("\n");
    }
    else{
        printf("There are no possible holes\n");
    }

    printf("----------------------------\n");
    for (int i=0; i<numberOfHoles; i++){
        possibleHoles[i] = -1;
    }

    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 0;
    holes[6] = 0;
    holes[7] = 2;
    holes[8] = 0;
    holes[9] = 0;
    holes[10] = 1;
    holes[11] = 0;

    showGame(holes, numberOfHoles, playerNumber, opponentScore, myScore);

    possible = canFeed(1, holes, possibleHoles);
    
    printf("The possible hole indexes should be : none\n");
    if(possible){
        printf("The possible hole indexes are :");
        for (int i=0; i<numberOfHoles; i++){
            if (possibleHoles[i] != -1){
                printf("%d, ", possibleHoles[i]);
            }
        }
        printf("\n");
    }
    else{
        printf("There are no possible holes\n");
    }

    printf("----------------------------\n");
    for (int i=0; i<numberOfHoles; i++){
        possibleHoles[i] = -1;
    }

    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 1;
    holes[6] = 0;
    holes[7] = 0;
    holes[8] = 0;
    holes[9] = 0;
    holes[10] = 0;
    holes[11] = 0;

    showGame(holes, numberOfHoles, playerNumber, opponentScore, myScore);

    possible = canFeed(2, holes, possibleHoles);
    
    printf("The possible hole indexes should be : 5\n");
    if(possible){
        printf("The possible hole indexes are :");
        for (int i=0; i<numberOfHoles; i++){
            if (possibleHoles[i] != -1){
                printf("%d, ", possibleHoles[i]);
            }
        }
        printf("\n");
    }
    else{
        printf("There are no possible holes\n");
    }
    
    printf("-----end of can feed test-------------------------------------------------------------------------\n\n");
}

void gameFinishedTest() {
    printf("-----game Finished test-----\n");

    int * holes = malloc(sizeof(int)*12);
    int myScore;
    int opponentScore;

    myScore = 0;
    opponentScore = 0;
    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 0;
    holes[6] = 0;
    holes[7] = 2;
    holes[8] = 0;
    holes[9] = 0;
    holes[10] = 1;
    holes[11] = 0;

    printf("The game should be finished because of hunger\n");
    if (!isGameFinished(holes, myScore, opponentScore)){
        printf("The game is not done\n");
    }
    printf("\n");

    printf("----------------------------\n");
    myScore = 27;
    opponentScore = 0;
    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 0;
    holes[6] = 0;
    holes[7] = 2;
    holes[8] = 0;
    holes[9] = 0;
    holes[10] = 1;
    holes[11] = 0;

    printf("The game should be finished because of score and hunger\n");
    if (!isGameFinished(holes, myScore, opponentScore)){
        printf("The game is not done\n");
    }
    printf("\n");

    printf("----------------------------\n");
    myScore = 0;
    opponentScore = 0;
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
    holes[10] = 0;
    holes[11] = 1;

    printf("The game should not be finished\n");
    if (!isGameFinished(holes, myScore, opponentScore)){
        printf("The game is not done\n");
    }

    printf("-----end of game Finished test-------------------------------------------------------------------------\n\n");
}


void collectSeedTest() {
    printf("-----collect Seed test-----\n");

    int numberOfHoles = 12;
    int * holes = malloc(sizeof(int)*numberOfHoles);

    int myScore;
    int opponentScore;
    int playerNumber;

    myScore = 0;
    opponentScore = 0;
    playerNumber = 1;
    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 0;
    holes[6] = 2;
    holes[7] = 2;
    holes[8] = 2;
    holes[9] = 0;
    holes[10] = 0;
    holes[11] = 0;

    showGame(holes, numberOfHoles, playerNumber, opponentScore, myScore);

    myScore = collectSeed(playerNumber, holes, myScore, 8);

    printf("I arrived at index 8\n");
    printf("My new score should be : %d\n",6);
    showGame(holes, numberOfHoles, playerNumber, opponentScore, myScore);

    printf("----------------------------\n");
    myScore = 0;
    opponentScore = 0;
    playerNumber = 1;
    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 0;
    holes[6] = 2;
    holes[7] = 2;
    holes[8] = 4;
    holes[9] = 0;
    holes[10] = 0;
    holes[11] = 0;

    showGame(holes, numberOfHoles, playerNumber, opponentScore, myScore);

    myScore = collectSeed(playerNumber, holes, myScore, 8);

    printf("I arrived at index 8\n");
    printf("My new score should be : %d\n",0);
    showGame(holes, numberOfHoles, playerNumber, opponentScore, myScore);

    printf("----------------------------\n");
    myScore = 0;
    opponentScore = 0;
    playerNumber = 1;
    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 0;
    holes[5] = 0;
    holes[6] = 2;
    holes[7] = 4;
    holes[8] = 3;
    holes[9] = 0;
    holes[10] = 0;
    holes[11] = 0;

    showGame(holes, numberOfHoles, playerNumber, opponentScore, myScore);

    myScore = collectSeed(playerNumber, holes, myScore, 8);

    printf("I arrived at index 8\n");
    printf("My new score should be : %d\n",3);
    showGame(holes, numberOfHoles, playerNumber, opponentScore, myScore);

    printf("-----end of collect Seed test-------------------------------------------------------------------------\n\n");
}

int main(){
    gameInitTest();
    gameShowTest();
    moveSeedTest();
    hasToBeFedTest();
    canFeedTest();
    gameFinishedTest();
    collectSeedTest();
}