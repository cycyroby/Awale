#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int moveSeed(int x, int *holes) {

    int y = holes[x];
    int i;

    for (i = x+1; i < x+y+1; i++){
        holes[i%12] = holes[i%12] + 1;
    }

    return (i-1)%12; //à tester

}

bool hasToBeFed(int player, int * holes) {
    printf("a\n");
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
    printf("%d\n", losing);
    printf("b\n");

    return losing; 
}

int * canFeed(int player, int * holes) { //player losing
    int numberOfHoles = 0;
    int * holesToCheck = malloc((sizeof(int)*6));
    if (player == 1) {
        for (int i = 6; i < 12; i++) {
            moveSeed(i, holes);
            if (!hasToBeFed(player, holes)) {
                numberOfHoles++;
                holesToCheck[i-6] = 1;
            }
            else {
                holesToCheck[i-6] = 0;
            }
        }
        if(numberOfHoles != 0) {
            int * possibleHoles = malloc(sizeof(int)*numberOfHoles);
            int j = 0;
            for (int k = 0; k < 6; k++) {
                if (holesToCheck[k] == 1) {
                    possibleHoles[j] = k + 6;
                }

                j++;
            }
            return possibleHoles;

        }

        else {
            return NULL;
        }
    }
    else {
        for (int i = 0; i < 6; i++) {
            moveSeed(i, holes);
            if (!hasToBeFed(player, holes)) {
                numberOfHoles++;
                holesToCheck[i] = 1;
            }
            else {
                holesToCheck[i] = 0;
            }
        }
        if(numberOfHoles != 0) {
            int * possibleHoles = malloc(sizeof(int)*numberOfHoles);
            int j = 0;
            for (int k = 0; k < 6; k++) {
                if (holesToCheck[k] == 1) {
                    possibleHoles[j] = k;
                }
                j++;
            }

            return possibleHoles;

        }
        else {
            return NULL;
        }

    }

}

bool isGameFinished(int player, int * holes) {
    printf("debut iGF\n");
    if (canFeed(player, holes) != NULL) {
        printf("appel CF\n");
        return false;
    }

    else {
        printf("fin appek CF\n");
        return true;
    }
}

int main() {
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

    printf("%d\n", isGameFinished(1, holes));
}

