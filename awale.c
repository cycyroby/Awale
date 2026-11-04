#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/*
initialise the game with 4 seeds in each hole
*/
void initGame(int *holes, int numberOfHoles) {
    for (int i=0; i<numberOfHoles; i++) {
        holes[i] = 4;
    }
}


/*
used in show game to make the separation line
*/
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

/*
Line number indicates if we're printing the row of holes of the opponent (lineNumber 1) or ours (lineNumber 2)
*/
void printIndexes(int numberOfHoles, int holeWidth, int index, int lineNumber) {
    printf("\n");
    int next;
    if (lineNumber == 2) {
        next = 1;
    }
    else if (lineNumber == 1){
        next = -1;
    }
    for (int i=0; i<numberOfHoles/2; i++) {
        
        printf(" ");
        
        // the number of characters the number of seeds take
        int numberPlaces;
        if ((index + i*next) <= 9) {
            numberPlaces = 1;
        }
        else if((index + i*next) >= 10) {
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
            printf(" ");
        }
        printf("%d", (index + i*next));
        for (int j=0; j<(holeWidth - numberPlaces)/2 + numberSpacesAfter; j++) {
            printf(" ");
        }
    }
    printf("     indexes\n");
}

/*
Line number indicates if we're printing the row of holes of the opponent (lineNumber 1) or ours (lineNumber 2)
*/
void printHoles(int*holes, int numberOfHoles, int holeWidth, int index, int lineNumber, char*scoreText, int score) {
    int next;
    if (lineNumber == 2) {
        next = 1;
    }
    else if (lineNumber == 1){
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

/* 
 Shows the game in ASCII art 
 parameter player : indicates which player I am 
  ___________ ___________ ___________ ___________ ___________ ___________ 
 |_holes[11]_|_holes[10]_|_holes[9]__|_holes[8]__|_holes[7]__|_holes[6]__| player 2
  ___________ ___________ ___________ ___________ ___________ ___________ 
 |_holes[0]__|_holes[1]__|_holes[2]__|_holes[3]__|_holes[4]__|_holes[5]__| player 1
 
 but my board is shown in front of me
 so when I am player 2 :
  ___________ ___________ ___________ ___________ ___________ ___________ 
 |_holes[5]__|_holes[4]__|_holes[3]__|_holes[2]__|_holes[1]__|_holes[0]__| player 1
  ___________ ___________ ___________ ___________ ___________ ___________ 
 |_holes[6]__|_holes[7]__|_holes[8]__|_holes[9]__|_holes[10]_|_holes[11]_| player 2
 */
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
        iOpp = 5;
        iMine = 6;
    }

    printIndexes(numberOfHoles, holeWidth, iOpp, 1);

    // first line
    printSeparationLine(numberOfHoles, holeWidth);

    // show opponnent's holes
    printHoles(holes, numberOfHoles, holeWidth, iOpp, 1, "Opponent's score", opponentScore);

    // second separation line
    printSeparationLine(numberOfHoles, holeWidth);

    // show my holes
    printHoles(holes, numberOfHoles, holeWidth, iMine, 2, "My score", myScore);

    printIndexes(numberOfHoles, holeWidth, iMine, 2);

    printf("\n");
}

/*
we choose the index hole so it moves the seeds in that hole in the counterclockwise direction
*/
int moveSeed(int index, int *holes) {

    int y = holes[index];
    if (y != 0) {
        int i;
        int endIndex = index+y+1;

        for (i = index+1; i < endIndex; i++){
            if (i%12 != index){
                holes[i%12] = holes[i%12] + 1;
                holes[index] -= 1;
            }
            else{
                endIndex++;
            }
        }
        // arrival index
        return (endIndex-1)%12;
    }
    else {
        return -1;
    }
}

/*
checks if all the holes of the player given are at 0 seeds (= it needs to be fed)
and then returns true or false accordingly
*/
bool hasToBeFed(int player, int * holes) {
    bool losing = true;
    if (player == 1) {
        for (int i = 0; i < 6; i++) {
            if (holes[i] != 0) {
               losing = false; 
            }
        }
    }

    else {
        for (int i = 6; i < 12; i++) {
            if (holes[i] != 0) {
                losing = false;
            }
        }
    }
    return losing; 
}


/*
a losing player given as input (= a player that has to be fed) checks which holes can feed it from its opponent's holes
*/
bool canFeed(int player, int * holes, int* possibleHoles) {
    int numberOfHoles = 0;
    int * holesThatCanFeed = malloc((sizeof(int)*12));
    int arrivingHole;

    if (player == 1) {
        for (int i = 6; i < 12; i++) {
            holesThatCanFeed[i] = 0;
            // holes[i]/12 because when we do a complete round the starting hole is jumped
            arrivingHole = (i + holes[i] + holes[i]/12);
            // if the arriving hole is on the other's holes or passes through them
            // expl : on 6 there are 6 seeds (6+6 = 12) %12 = 0
            //        -> it arrives until the 1st hole of the hungry player
            // expl : on 6 there are 5 seeds (6+5 = 11) %12 = 11
            //        -> it arrives until the last of the non hungry player - the hungry player is still hungry
            // expl : on 6 there are 16 seeds (6+16 + 1since16>12 = 23) %12 = 11
            //        -> it also arrives on the last of the non hungry but before it passed through all the hungry's holes
            if (12 <= arrivingHole) {
                numberOfHoles += 1;
                holesThatCanFeed[i] = 1;
            }
        }
    }
    else {
        for (int i = 0; i < 6; i++) {
            holesThatCanFeed[i] = 0;
            arrivingHole = (i + holes[i] + holes[i]/12);
            if (6 <= arrivingHole) {
                numberOfHoles += 1;
                holesThatCanFeed[i] = 1;
            }
        }
    }

    if(numberOfHoles != 0) {
        int j = 0;
        for (int k = 0; k < 12; k++) {
            if (holesThatCanFeed[k] == 1) {
                possibleHoles[j] = k;
                j++;
            }
        }
        return true;
    }
    else {
        return false;
    }

}

/*returns true if the game is done because one won't be able to have any more seeds (is hungry and can't be fed)*/
bool isGameFinishedHunger(int * holes) {
    int player;
    int*possibleHoles;
    if (hasToBeFed(1, holes)){
        player = 1;
        // if both have to be fed
        if(hasToBeFed(2, holes)){
            return true;
        }
        possibleHoles = malloc(sizeof(int)*12);
        return !canFeed(player, holes, possibleHoles);
    }
    else if (hasToBeFed(2, holes)){
        player = 2;
        possibleHoles = malloc(sizeof(int)*12);
        return !canFeed(player, holes, possibleHoles);
    }
}

/*returns true if the game is done because one player has 25 or more points*/
bool isGameFinishedScore(int*holes, int myScore, int opponentScore){
    return (myScore>=25 || opponentScore>=25);
}

/*
returns true if the game is done and prints why it is done
// we're not doing the we can't capture any seeds finishing rule for the moment
*/
bool isGameFinished(int * holes, int myScore, int opponentScore) {
    bool score = isGameFinishedScore(holes, myScore, opponentScore);
    bool hunger = isGameFinishedHunger(holes);
    if (score){
        printf("One of the players has more than 25 points !\n");
    }
    if (hunger){
        printf("One of the players can't have any more seeds to continue playing !\n");
    }
    return score||hunger;
}

/*

*/
int collectSeed(int player, int * holes, int myScore, int arrival) {
    if (player == 1) {
        if (5 < arrival && arrival < 12) {
            int i = 0;
            while ((holes[arrival - i] == 2 || holes[arrival - i] == 3) && arrival > 5) {
                myScore += holes[arrival - i];
                holes[arrival-i] = 0;
                i++;
            }
        }
    }

    if (player == 2) {
        if (0 <= arrival && arrival < 6) {
            int i = 0;
            while ((holes[arrival - i] == 2 || holes[arrival -i] == 3 ) && arrival >= 0) {
                myScore += holes[arrival - i];
                holes[arrival-i] = 0;
                i++;
            }
        }
    }

    return myScore;
}

