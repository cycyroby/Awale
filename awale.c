#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ClientServeur/Serveur/client2.h"

/*
initialise the game with 4 seeds in each hole
*/
void initGame(int *holes, int numberOfHoles) {
    for (int i=0; i<numberOfHoles; i++) {
        holes[i] = 4;
    }
    /* */
    //test
    holes[0] = 0;
    holes[1] = 0;
    holes[2] = 0;
    holes[3] = 0;
    holes[4] = 4;
    holes[5] = 1;
    holes[6] = 1;
    holes[7] = 1;
    holes[8] = 1;
    holes[9] = 0;
    holes[10] = 0;
    holes[11] = 1;
    /**/
}


/*
used in show game to make the separation line
*/
void printSeparationLine(char *message, int numberOfHoles, int holeWidth){
    // separation line
    for (int i=0; i<numberOfHoles/2; i++) {
        strcat(message, " ");
        for (int j=0; j<holeWidth; j++) {
            strcat(message, "_");
        }
    }
    strcat(message, "\n");
}

/*
Line number indicates if we're printing the row of holes of the opponent (lineNumber 1) or ours (lineNumber 2)
*/
void printIndexes(char *message, int numberOfHoles, int holeWidth, int index, int lineNumber) {
    strcat(message, "\n");
    int next;
    if (lineNumber == 2) {
        next = 1;
    }
    else if (lineNumber == 1){
        next = -1;
    }
    for (int i=0; i<numberOfHoles/2; i++) {
        
        strcat(message, " ");
        
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
            strcat(message, " ");
        }
        int result = index + (i*next);
        char temp[20];
        sprintf(temp, "%d", result);
        strcat(message, temp);
        for (int j=0; j<(holeWidth - numberPlaces)/2 + numberSpacesAfter; j++) {
            strcat(message, " ");
        }
    }
    strcat(message, "     indexes\n");
}

/*
Line number indicates if we're printing the row of holes of the opponent (lineNumber 1) or ours (lineNumber 2)
*/
void printHoles(char *message, int*holes, int numberOfHoles, int holeWidth, int index, int lineNumber, char*scoreText, int score) {
    int next;
    if (lineNumber == 2) {
        next = 1;
    }
    else if (lineNumber == 1){
        next = -1;
    }
    for (int i=0; i<numberOfHoles/2; i++) {
        strcat(message, "|");
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
            strcat(message, "_");
        }
        char nOfS_char[20];
        sprintf(nOfS_char, "%d", numberOfSeeds);
        strcat(message, nOfS_char);
        for (int j=0; j<(holeWidth - numberPlaces)/2 + numberSpacesAfter; j++) {
            strcat(message, "_");
        }
    }
    strcat(message, "|    ");
    strcat(message, scoreText);
    strcat(message," : ");
    char score_char[20];
    sprintf(score_char, "%d", score);
    strcat(message, score_char);
    strcat(message, "\n");
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
void showGame(int *holes, int numberOfHoles, int player, int opponentScore, int myScore, Client *sender) {
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

    char *message = malloc(BUF_SIZE * sizeof(char));
    message[0] = '\0';
    strcpy(message, "GAME : \n");
    printIndexes(message, numberOfHoles, holeWidth, iOpp, 1);

    // first line
    printSeparationLine(message, numberOfHoles, holeWidth);


    // show opponnent's holes
    printHoles(message, holes, numberOfHoles, holeWidth, iOpp, 1, "Opponent's score", opponentScore);


    // second separation line
    printSeparationLine(message, numberOfHoles, holeWidth);

    // show my holes
    printHoles(message, holes, numberOfHoles, holeWidth, iMine, 2, "My score", myScore);

    printIndexes(message, numberOfHoles, holeWidth, iMine, 2);

    strcat(message, "\n");


    show(message, sender);
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
bool canFeed(int player, int * holes, int* possibleHoles, Client * opponent, int * possibleMoves) {
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
        printf("a player has to be fed and possible moves will no longer be NULL\n");
        show("You can only play the holes :", opponent);
        int j = 0;
        for (int k = 0; k < 12; k++) {
            if (holesThatCanFeed[k] == 1) {
                possibleHoles[j] = k;
                possibleMoves[j] = k;
                j++;
                show(" ", opponent);
                char possibleHole[20];
                sprintf(possibleHole, "%d", k);
                show(possibleHole, opponent);
            }
        }
        show("\n", opponent);
        return true;
    }
    else {
        return false;
    }

}

/*returns true if the game is done because one won't be able to have any more seeds (is hungry and can't be fed)*/
bool isGameFinishedHunger(int * holes, Client * opponent, bool *feed, int *possibleMoves) {
    int player;
    int*possibleHoles;
    if (hasToBeFed(1, holes)){
        player = 1;
        // if both have to be fed
        if(hasToBeFed(2, holes)){
            return true;
        }
        possibleHoles = malloc(sizeof(int)*12);
        *feed = true;
        return !canFeed(player, holes, possibleHoles, opponent, possibleMoves);
    }
    else if (hasToBeFed(2, holes)){
        player = 2;
        possibleHoles = malloc(sizeof(int)*12);
        *feed = true;
        return !canFeed(player, holes, possibleHoles, opponent, possibleMoves);
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
bool isGameFinished(int * holes, int myScore, int opponentScore, Client * sender, Client * opponent, bool *feed, int *possibleMoves) {
    bool score = isGameFinishedScore(holes, myScore, opponentScore);
    bool hunger = isGameFinishedHunger(holes, opponent, feed, possibleMoves);
    if (score){
        show("One of the players has more than 25 points !\n", sender);
        show("One of the players has more than 25 points !\n", opponent);
    }
    if (hunger){
        show("One of the players can't have any more seeds to continue playing !\n", sender);
        show("One of the players can't have any more seeds to continue playing !\n", opponent);
    }
    return score||hunger;
}

/*
player : player who is collecting the seeds
arrival : the index at which they arrived
*/
int collectSeed(int player, int * holes, int myScore, int arrival) {
    if (player == 1) {
        if (5 < arrival && arrival < 12) {
            int i = 0;
            while ((holes[arrival - i] == 2 || holes[arrival - i] == 3) && (arrival-i) > 5) {
                myScore += holes[arrival - i];
                holes[arrival-i] = 0;
                i++;
            }
        }
    }

    if (player == 2) {
        if (0 <= arrival && arrival < 6) {
            int i = 0;
            while ((holes[arrival - i] == 2 || holes[arrival -i] == 3 ) && (arrival-i) >= 0) {
                myScore += holes[arrival - i];
                holes[arrival-i] = 0;
                i++;
            }
        }
    }

    return myScore;
}

/*
we choose the index hole so it moves the seeds in that hole in the counterclockwise direction
*/
int moveSeed(int index, int *holes) {
    int y = holes[index];
    if (y != 0) {
        // test if the move starves the opponent
        int opponentPlayer;
        int player;
        if (0 <= index && index < 6) {
            player = 1;
            opponentPlayer = 2;
        }
        else if(6 <= index && index < 12) {
            player = 2;
            opponentPlayer = 1;
        }
        bool doesntStarve = false;

        int* testHoles = malloc(sizeof(int)*12);
        for (int i=0; i<12; i++) {
            testHoles[i] = holes[i];
        }

        int endIndex = index+y+1;
        for (int i = index+1; i < endIndex; i++){
            if (i%12 != index){
                testHoles[i%12] = testHoles[i%12] + 1;
                testHoles[index] -= 1;
            }
            else{
                endIndex++;
            }
        }
        collectSeed(player, testHoles, 0, (endIndex-1)%12);
        doesntStarve = !hasToBeFed(opponentPlayer, testHoles);
        free(testHoles);
        // end test

        if (doesntStarve) {
            int endIndex = index+y+1;
            for (int i = index+1; i < endIndex; i++){
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
            return -2;
        }
    }
    else {
        return -1;
    }
}