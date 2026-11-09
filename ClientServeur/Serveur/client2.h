#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

enum state{HAS_TO_PLAY, WAITING_TO_PLAY, CHALLENGING, CHALLENGED, OTHER};

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   enum state state;
   char opponent[BUF_SIZE];
   int player;
   int *holes;
   int myScore;
   int *possibleMoves;
   char bio[BUF_SIZE];
}Client;

#endif /* guard */
