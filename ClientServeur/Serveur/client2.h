#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

enum state{PLAYING, CHALLENGING, CHALLENGED, OTHER};

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   enum state state;
   char opponent[BUF_SIZE];
}Client;

#endif /* guard */
