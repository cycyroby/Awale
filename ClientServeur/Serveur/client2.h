#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"
#include <stdbool.h>

enum state{HAS_TO_PLAY, WAITING_TO_PLAY, CHALLENGING, CHALLENGED, OTHER};




typedef struct Client
{
   SOCKET sock;
   char name[BUF_SIZE];
   enum state state;
   bool private;
   char opponent[BUF_SIZE];
   int player;
   int *holes;
   int myScore;
   int *possibleMoves;
   char observing[BUF_SIZE];
   char * observers[BUF_SIZE];
   int actualObservers;
   char bio[BUF_SIZE];
   char *friends[BUF_SIZE];
   int numberFriends;
} Client;

#endif /* guard */
