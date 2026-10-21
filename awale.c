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

