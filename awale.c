#include <stdio.h>

int moveSeed(int x, int **holes) {

    int y = holes[x];

    for (int i = x+1; i < x+y+1; i++){
        holes[i%12] = holes[i%12] + 1;
    }

    return (i-1)%12; //à tester

}
