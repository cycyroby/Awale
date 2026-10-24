#include <stdio.h>
#include <stdlib.h>

int moveSeed(int x, int *holes) {

    int y = holes[x];
    int i;

    for (i = x+1; i < x+y+1; i++){
        holes[i%12] = holes[i%12] + 1;
    }

    return (i-1)%12; //à tester

}


void initGame(int *holes, int numberOfHoles) {
    for (int i=0; i<numberOfHoles; i++) {
        holes[i] = 4;
    }
}


void printSeparationLine(int numberOfHoles, int holeWidth){
    // separation line
    for (int i=0; i<numberOfHoles/2; i++) {
        printf(" ");
        for (int j=0; j<holeWidth; j++) {
            printf("_");
        }
    }
    printf("\n");
}
void printHoles(int*holes, int numberOfHoles, int holeWidth, int index, char*scoreText, int score) {
    int next;
    if (index == 0) {
        next = 1;
    }
    else {
        next = -1;
    }
    for (int i=0; i<numberOfHoles/2; i++) {
        printf("|");
        int numberOfSeeds = holes[index + i*next];
        
        // the number of characters the number of seeds take
        int numberPlaces;
        if (numberOfSeeds <= 9) {
            numberPlaces = 1;
        }
        else if(numberOfSeeds >= 10) {
            numberPlaces = 2;
        }


        int numberSpacesAfter;
        if ((holeWidth - numberPlaces)%2 == 0) {
            numberSpacesAfter = 0;
        }
        else {
            numberSpacesAfter = 1;
        }

        // showing the hole
        for (int j=0; j<(holeWidth - numberPlaces)/2; j++) {
            printf("_");
        }
        printf("%d", numberOfSeeds);
        for (int j=0; j<(holeWidth - numberPlaces)/2 + numberSpacesAfter; j++) {
            printf("_");
        }
    }
    printf("|    %s : %d\n", scoreText, score);
}

void showGame(int *holes, int numberOfHoles, int player, int opponentScore, int myScore) {
    int holeWidth = 4;

    int iOpp;
    int iMine;
    // if I am player 1 the first 6 holes are mine if not the last 6 are
    if (player == 1) { 
        iOpp = 11;
        iMine = 0;
    }
    else if (player == 2) {
        iOpp = 0;
        iMine = 11;
    }

    // first line
    printSeparationLine(numberOfHoles, holeWidth);

    // show opponnent's holes
    printHoles(holes, numberOfHoles, holeWidth, iOpp, "Opponent's score", opponentScore);

    // second separation line
    printSeparationLine(numberOfHoles, holeWidth);

    // show my holes
    printHoles(holes, numberOfHoles, holeWidth, iMine, "My score", myScore);

    printf("\n");
}