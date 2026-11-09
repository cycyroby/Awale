#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "server2.h"
#include "client2.h"
#include "../../awale.c"

#define numberOfHoles 12

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];

   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         // if we type on the keyboard on the server, the server stops
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }
         /* I wrote this to better understand the read_client function
         else {
            printf("reading the client's name : ");
            printf("%s\n", buffer);
         }*/

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         strncpy(c.name, buffer, BUF_SIZE - 1);

         if (check_usernames(clients, c, actual) == 1) {
            read_client(c.sock, buffer);
            strncpy(c.name, buffer, BUF_SIZE - 1);
         }
         c.state = OTHER;
         strcpy(c.observing, "");
         c.actualObservers = 0;
         printf("init client state: %d\n" ,c.state);
         // I think it counts the number of clients we have connected
         clients[actual] = c;
         actual++;
         

         write_client(c.sock, MENU);
      }
      else
      {
         int i = 0;
         // for each connected client
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               printf("--------------------\n");
               Client* senderClient = &(clients[i]);
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  // I think this tells the other clients that "name of client" disconnected
                  strncpy(buffer, (*senderClient).name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  //send_message_to_all_clients(clients, senderClient, actual, buffer, 1);

                  if ((*senderClient).state == CHALLENGED || (*senderClient).state == CHALLENGING) {
                     Client* opponent = get_client_by_username((*senderClient).opponent, clients, actual);
                     (*opponent).state = OTHER;
                     strcpy((*opponent).opponent, "");
                     write_client(opponent->sock, buffer);
                  }
                  else if ((*senderClient).state == HAS_TO_PLAY || (*senderClient).state == WAITING_TO_PLAY) {
                     Client* opponent = get_client_by_username((*senderClient).opponent, clients, actual);
                     stopObserving(senderClient, NULL, clients, actual);
                     stopObserving(opponent, NULL, clients, actual);
                     resetPlayer(opponent, false);
                     write_client(opponent->sock, buffer);
                  }

                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
               }
               else
               {
                  printf("user typed command\n");
                  char* commandName = malloc(BUF_SIZE*sizeof(char));
                  char* parameterCalculator = malloc(BUF_SIZE*sizeof(char));
                  char* parameter;
                  printf(" before seperating command and parameter buffer = %s\n", buffer);
                  int start = separateCommandParameter(buffer, commandName, parameterCalculator);
                  parameter = parameterCalculator + start;
                  //parameter = separateCommandParameter(buffer, commandName, parameter);
                  printf("command and parameter seperated : command = %s, parameter = %s\n", commandName, parameter);
                  enum commands command = checkIfCommand(commandName);
                  free(commandName);
                  
                  char message[BUF_SIZE];
                  message[0] = 0;

                  if (command == USERS){
                     printf("in users\n");
                     send_usernames(clients, (*senderClient), actual);
                     if ((*senderClient).state == CHALLENGED) {
                        printf("in users | challenged asks\n");
                        strcpy(message, "Don't forget to either accept or reject ");
                        strcat(message, (*senderClient).opponent);
                        strcat(message, "'s challenge\n");
                     }
                     write_client((*senderClient).sock, message);
                  }

                  else if (command == CHALLENGE) {
                     printf("in challenge\n");
                     Client* receiver = get_client_by_username(parameter, clients, actual);

                     if ((*senderClient).state == CHALLENGED) {
                        strcpy(message, "You have already been challenged. You can either 'accept' or 'reject'.\n");
                     }
                     else if ((*senderClient).state == CHALLENGING) {
                        strcpy(message, "You have already challenged ");
                        strcat(message, (*senderClient).opponent);
                        strcat(message, ". Wait patiently until they either accept or refuse your challenge.\n");
                     }
                     else if ((*senderClient).state == HAS_TO_PLAY || (*senderClient).state == WAITING_TO_PLAY) {
                        strcpy(message, "You are already playing a game. You cannot challenge another person.\n");
                     }
                     else if ((*senderClient).state == OTHER) {
                        // if we actually found a client
                        if (receiver != NULL) {
                           if ((*receiver).state == OTHER) {
                              send_challenge(receiver, senderClient);
                              strcpy(message, "");
                           }
                           else {
                              strcpy(message, "The user you are trying to challenge has either challenged someone else, has been challenged or is playing. Thus they cannot be challenged at the moment.\n");
                           }
                        }
                        else {
                           strcpy(message, "The user you challenged isn't online or doesn't exist :(\n");
                        } 
                     }  
                     write_client((*senderClient).sock, message);                 
                  }
                  else if(command == ACCEPT || command == REJECT) {
                     printf("in accept or reject\n");
                     if ((*senderClient).state == CHALLENGED) {
                        printf("in accept reject | challenged answers\n");
                        get_answer_challenge(command, senderClient, clients, actual);
                     }
                     else {
                        printf("in accept reject | not challenged answers\n");
                        strcpy(message, "You cannot accept nor reject since you have not been challenged.\n");
                     }
                     write_client((*senderClient).sock, message);
                  }
                  else if(command == MOVE) {
                     bool couldPlay = false;
                     int arrival;
                     printf("in move\n");
                     if((*senderClient).state != HAS_TO_PLAY && (*senderClient).state != WAITING_TO_PLAY) {
                        printf("in move | commander is not a player\n");
                        strcpy(message, "You cannot move the seeds as you are not playing.\n");
                        write_client((*senderClient).sock, message);
                     }
                     else if ((*senderClient).state == WAITING_TO_PLAY) {
                        printf("in move | not commander's turn\n");
                        strcpy(message, "You have to wait for ");
                        strcat(message, (*senderClient).opponent);
                        strcat(message, " to play.\n");
                        write_client((*senderClient).sock, message);
                     }
                     else if ((*senderClient).state == HAS_TO_PLAY){
                        printf("in move | commander's turn\n");
                        int index = atoi(parameter);
                        // enter this "if" only if the opponent player has to be fed
                        if ((*senderClient).possibleMoves) {
                           printf("in move | opponent has to be fed by commander\n");
                           for (int i = 0; i < sizeof((*senderClient).possibleMoves); i++) {

                              if (index == (*senderClient).possibleMoves[i]) {
                                 printf("in move | commander's move is in the authorised moves\n");
                                 arrival = moveSeed(index, (*senderClient).holes);
                                 couldPlay = true;
                              }
                           }
                           if (couldPlay) {
                              free(senderClient->possibleMoves);
                              printf("in move | freed possible moves\n");
                              senderClient->possibleMoves = NULL;
                              printf("in move | nulled possible moves\n");
                           }
                           else {
                              printf("in move | commander played a non playable hole | doesn't feed\n");
                              write_client((*senderClient).sock, "You can't move seeds from holes that don't feed your opponent\n");
                           }

                        }
                        // if the index belongs to the player (they can play it)
                        else if (((*senderClient).player == 1 && index >= 0 && index < 6) || ((*senderClient).player == 2 && index >= 6 && index < 12 )) {
                           printf("in move | no one has to be fed, commander's index is a correct one\n");
                           arrival = moveSeed(index, (*senderClient).holes);
                           if (arrival == -1) {
                              printf("in move | commander played a non playable hole | empty hole\n");
                              write_client((*senderClient).sock, "You can't move seeds from an empty hole\n");
                           }
                           else if (arrival == -2) {
                              printf("in move | commander played a non playable hole | starving hole\n");
                              write_client((*senderClient).sock, "You can't move seeds from a hole starves your opponent by taking all their seeds\n");
                           }
                           else {
                              printf("in move | commander played a playable hole\n");
                              couldPlay = true;
                           }
                           
                        }
                        else {
                           printf("in move | no one has to be fed, commander's index is a correct one\n");
                           strcpy(message, "You want to move seeds from a hole that doesn't belong to you.\n");
                           write_client((*senderClient).sock, message);
                        }

                        if (couldPlay) {
                           printf("in move | in could Play\n");
                           int score = collectSeed((*senderClient).player, (*senderClient).holes, (*senderClient).myScore, arrival);
                           (*senderClient).myScore = score;
                           Client *opponent = get_client_by_username((*senderClient).opponent, clients, actual);
                           showGame((*senderClient).holes, numberOfHoles, (*senderClient).player, (*opponent).myScore, (*senderClient).myScore, senderClient, "Opponent's score", "My score", senderClient);
                           showGame((*senderClient).holes, numberOfHoles, (*opponent).player, (*senderClient).myScore, (*opponent).myScore, opponent, "Opponent's score", "My score", opponent); 
                           showObservers(senderClient, opponent, clients, actual);
                           printf("in move | commander's score has been increased by %d and the board has been shown\n", score);
                           (*senderClient).state = WAITING_TO_PLAY;
                           (*opponent).state = HAS_TO_PLAY;
                           printf("in move | in could Play | turn given to the other\n");
                           write_client((*opponent).sock, "It is your turn ! YEAHH!\n");
                           bool feedValue = false;
                           // the possible moves are calculated for the opponent because only your play can starve you so your opponent has to feed you
                           // in our rules you cannot be starved by your opponent (-2 returned by the move function)
                           int *possibleMoves = malloc(sizeof(int)*numberOfHoles);
                           bool finished = isGameFinished((*senderClient).holes, (*senderClient).myScore, (*opponent).myScore, senderClient, opponent, &feedValue, possibleMoves);
                           if (feedValue && !finished) {
                              printf("in move | in could Play | commander has starved themself so opponent will have to feed them\n");
                              (*opponent).possibleMoves = possibleMoves;
                           }
                           else {
                              free(possibleMoves);
                              (*opponent).possibleMoves = NULL;
                           }
                           if (finished) {
                              printf("in move | in could Play | the game is finished\n");
                              if ((*senderClient).myScore > (*opponent).myScore) {
                                 write_client((*senderClient).sock, "You won :)\n");
                                 write_client((*opponent).sock, "You lost :(\n");
                              }

                              else if ((*senderClient).myScore < (*opponent).myScore) {
                                 write_client((*opponent).sock, "You won :)\n");
                                 write_client((*senderClient).sock, "You lost :(\n");
                              }
                              else {
                                 write_client((*senderClient).sock, "You tied :|\n");
                                 write_client((*opponent).sock, "You tied :|\n");
                              }
                              resetPlayer(senderClient, false);
                              resetPlayer(opponent, true);                              
                           }
                        }
                     }
                  }
                  else if (command == SHOW_COMMANDS) {
                     printf("in show commands\n");
                     showCommands(*senderClient);
                  }
                  else if (command == LIST_GAMES) {
                     printf("in list games\n");
                     listGames(clients, *senderClient, actual);
                  }
                  else if (command == OBSERVE) {
                     printf("in observe\n");
                     if (senderClient->state == WAITING_TO_PLAY || senderClient->state == HAS_TO_PLAY) {
                        printf("in observe | a player can't\n");
                        write_client(senderClient->sock, "You can't observe as you are playing a game.\n");
                     }
                     else {
                        Client* player = get_client_by_username(parameter, clients, actual);
                        Client *opponent = get_client_by_username((*player).opponent, clients, actual);
                        if (player != NULL) {
                           if ((*player).state == WAITING_TO_PLAY || (*player).state == HAS_TO_PLAY) {
                              observe(player, senderClient);
                              showObservers(player, opponent, clients, actual);
                           }
                           else {
                              write_client(senderClient->sock, "The user you are trying to observe isn't playing a game.\n");
                           }
                        }
                        else {
                           write_client(senderClient->sock, "The user you are trying to observe isn't online or doesn't exist :(\n");
                        } 
                     }
                  }
                  else if (command == STOP_OBSERVING) {
                     printf("in stop observing\n");
                     Client * player = get_client_by_username(senderClient->observing, clients, actual);
                     if (player != NULL) {
                        stopObserving(player, senderClient, clients, actual);
                     }
                     else {
                        write_client(senderClient->sock, "Currently you aren't observing anyone.\n");
                     }
                  }
                  else if (command == NULL_COMMAND) {
                     printf("in command not recognised\n");
                     write_client(senderClient->sock, "Command not recognized. You can use 'showCommands' to see available commands.\n");
                  }
                  free(parameterCalculator);
               }
               break;
            }
         }
      }
   }
   // what should happen when a person is challenged is that others can continue their life and the challenged cannot 
// write anything exept accept, reject or disconnect
// the challenger cannot challenge another person
// others cannot challenge the two

   clear_clients(clients, actual);
   end_connection(sock);
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client *sender, int actual, const char *buffer, char from_server)
{
   char message[BUF_SIZE];
   message[0] = 0;

   if(from_server == 0)
   {
      strncpy(message, (*sender).name, BUF_SIZE - 1);
      strncat(message, " : ", sizeof message - strlen(message) - 1);
   }
   strncat(message, buffer, sizeof message - strlen(message) - 1);

   for(int i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if((*sender).sock != clients[i].sock)
      {
         write_client(clients[i].sock, message);
      }
   }
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }
   /* I wrote this to better understand the read_client function
   else {
      printf("reading the client's message : ");
      printf("%s\n", buffer);
   }*/

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

// -----------------------------------------fct we added + main at the end-----------------------------------

static int separateCommandParameter(const char* buffer, char* commandName, char*parameter) {
   commandName[0] = 0;
   parameter[0] = 0;

   strncpy(commandName, buffer, BUF_SIZE - 1);
   strncpy(parameter, buffer, BUF_SIZE - 1);
   int ptrCmd = 0;
   int ptrPar = 0;

   while(commandName[ptrCmd] != ' ' && commandName[ptrCmd] != '\0') {
      ptrCmd++;
      ptrPar++;
   }
   if (commandName[ptrCmd] == ' ') {
      ptrPar++;
   }
   else if (commandName[ptrCmd] == '\0') {
      parameter[0] = 0;
      strncpy(parameter, "", BUF_SIZE - 1);
   }
   // marks the end of the command word
   commandName[ptrCmd] = 0;
   return ptrPar;
}

static enum commands checkIfCommand(const char *commandSent){
   enum commands command;

   if (!strncmp(commandSent, "users", sizeof("users"))){
      command = USERS; 
   }
   else if (!strncmp(commandSent, "challenge", sizeof("challenge"))) {
      command = CHALLENGE;
   }
   else if (!strncmp(commandSent, "accept", sizeof("accept"))) {
      command = ACCEPT;
   }
   else if (!strncmp(commandSent, "reject", sizeof("reject"))) {
      command = REJECT;
   }
   else if(!strncmp(commandSent, "move", sizeof("move"))) {
      command = MOVE;
   }
   else if (!strncmp(commandSent, "showCommands", sizeof("showCommands"))) {
      command = SHOW_COMMANDS;
   }
   else if(!strncmp(commandSent, "listGames", sizeof("listGames"))) {
      command = LIST_GAMES;
   }
   else if(!strncmp(commandSent, "observe", sizeof("observe"))) {
      command = OBSERVE;
   }
   else if(!strncmp(commandSent, "stopObserving", sizeof("stopObserving"))) {
      command = STOP_OBSERVING;
   }
   else {
      command = NULL_COMMAND;
   }

   return command;
}

static void send_usernames(Client *clients, Client sender, int actual)
{
   char message[BUF_SIZE];
   message[0] = 0;

   strncpy(message, "---- online users list ----\n", BUF_SIZE - 1);

   for(int i = 0; i < actual; i++)
   {
      if(sender.sock != clients[i].sock)
      {
         strncat(message, clients[i].name, sizeof message - strlen(message) - 1);
         strncat(message, "\n", sizeof message - strlen(message) - 1);
      }
   }

   strncat(message, "----\n", sizeof message - strlen(message) - 1);

   write_client(sender.sock, message);
}

static int check_usernames(Client *clients, Client sender, int actual){
   char message[BUF_SIZE];
   message[0] = 0;
   for(int i = 0; i < actual; i++)
   {
      if(strncmp(sender.name, clients[i].name, sizeof(sender.name))==0)
      {

         strncpy(message, "username already used, write a new one\n", BUF_SIZE - 1);
         write_client(sender.sock, message);
         return 1;
      }
   }

   return 0;
}

static Client* get_client_by_username(const char* name, Client *clients, int actual){
   if (actual > 0) {
      for(int i = 0; i < actual; i++){
         if(strncmp(name, clients[i].name, sizeof(name))==0){
            return &(clients[i]);
         }
      }
      return NULL;
   }
}

static void send_challenge(Client* client, Client* sender){
   strcpy((*client).opponent, (*sender).name);
   strcpy((*sender).opponent, (*client).name);
   (*client).state = CHALLENGED;
   (*sender).state = CHALLENGING;


   char message[BUF_SIZE];
   message[0] = 0;
   strncpy(message, (*sender).name, BUF_SIZE - 1);
   strncat(message, " \
wants to challenge you to a awale-game. \nDo you want to accept the challenge ?\n\
Please write 'accept' if you do and 'reject' if you don't.\n", sizeof message - strlen(message) - 1);

   
   write_client((*client).sock, message);
}


static void get_answer_challenge(enum commands command, Client * sender, Client * clients, int actual){
      Client* opponent = get_client_by_username((*sender).opponent, clients, actual);
      // the client answered accept
      if (command == ACCEPT){
         write_client((*opponent).sock, "Your challenge has been accepted.\n");
         
         stopObserving(get_client_by_username(sender->observing, clients, actual), sender, clients, actual);
         stopObserving(get_client_by_username(opponent->observing, clients, actual), opponent, clients, actual);

         int * holes;
         holes = malloc(sizeof(int)*numberOfHoles);
         initGame(holes, numberOfHoles);
         int player = get_random_player_one();
         (*opponent).player = player;
         int opponentScore = 0;
         int myScore = 0;
         if (player == 1) {
            (*sender).player = 2;
            (*sender).state = WAITING_TO_PLAY;
            (*opponent).state = HAS_TO_PLAY;
         }
         else {
            (*sender).player = 1;
            (*sender).state = HAS_TO_PLAY;
            (*opponent).state = WAITING_TO_PLAY;
         }
         (*sender).myScore = 0;
         (*opponent).myScore = 0;
         showGame(holes, numberOfHoles, (*sender).player, (*opponent).myScore, (*sender).myScore, sender, "Opponent's score", "My score", sender);

         showGame(holes, numberOfHoles, (*opponent).player, (*sender).myScore, (*opponent).myScore, opponent, "Opponent's score", "My score", opponent);
         (*sender).holes = holes;
         (*opponent).holes = holes;
         (*sender).possibleMoves = NULL;
         (*opponent).possibleMoves = NULL;

         char opponent_player[20];
         sprintf(opponent_player, "%d", (*opponent).player);
         write_client((*opponent).sock, "You are player : ");
         write_client((*opponent).sock, opponent_player);
         write_client((*opponent).sock, "\n");
         write_client((*opponent).sock, "Player 1 begins\n");
         write_client((*opponent).sock, "move [index]         - to move a seed from the hole with the given index \n");
         char sender_player[20];
         sprintf(sender_player, "%d", (*sender).player);
         write_client((*sender).sock, "You are player : ");
         write_client((*sender).sock, sender_player);
         write_client((*sender).sock, "\n");
         write_client((*sender).sock, "Player 1 begins\n");
         write_client((*sender).sock, "move [index]         - to move a seed from the hole with the given index \n");

      }
      // the client answered reject
      else if (command == REJECT){
         (*sender).state = OTHER;
         (*opponent).state = OTHER;
         strcpy((*sender).opponent, "");
         strcpy((*opponent).opponent, "");
         write_client((*opponent).sock, "Your challenge has been rejected.\n");
      }
}

static int get_random_player_one() {
   //This will give 1 if the challenger is gonna be player one and 2 if he's gonna be player two.
   srand(time(NULL));
   int n = rand()%2;
   n++;
   return n;
}

static void show(char *message, Client *sender) {
   write_client((*sender).sock, message);
}

static void showCommands(Client sender) {
   write_client(sender.sock, SHOWCOMMANDS);
}

static void resetPlayer(Client * player, bool holesFreed) {
   if (!holesFreed) {
      free(player->holes);
   }
   player->holes = NULL;

   if (player->possibleMoves) {
      free(player->possibleMoves);
      player->possibleMoves = NULL;
   }
   
   player->myScore = 0;
   strcpy(player->opponent, "");
   player->state = OTHER;
}

/*usage : 
Client ** clientPairs = malloc(sizeof(Client*)*actual);
for(int i = 0; i < actual; i++)
{
   clientPairs[i] = malloc(sizeof(Client)*2);
}
int onGoingGames = getOngoingGames(clients, actual, clientPairs);
...
for(int i = 0; i < actual; i++)
   {
      free(clientPairs[i]);
   }
   free(clientPairs);
*/
int getOngoingGames(Client * clients, int actual, Client ** clientPairs) {
   int onGoingGamesNbr = 0;
   Client * opponentAlreadyCounted = malloc(sizeof(Client)*actual); // actual/2 for the size is enough but just in case
   bool hasAlreadyBeenCounted;
   for(int i = 0; i < actual; i++)
   {
      if (clients[i].state == WAITING_TO_PLAY || clients[i].state == HAS_TO_PLAY) {
         hasAlreadyBeenCounted = false;
         int j = 0;
         while (j < onGoingGamesNbr && !hasAlreadyBeenCounted) {
            if (clients[i].sock == opponentAlreadyCounted[j].sock) {
               hasAlreadyBeenCounted = true;
            }
            j++;
         }

         if (!hasAlreadyBeenCounted) {
            clientPairs[onGoingGamesNbr][0] = clients[i];
            clientPairs[onGoingGamesNbr][1] = *get_client_by_username(clients[i].opponent, clients, actual);
            opponentAlreadyCounted[onGoingGamesNbr] = clientPairs[onGoingGamesNbr][1];
            onGoingGamesNbr++;
         }
      }
   }
   free(opponentAlreadyCounted);
   return onGoingGamesNbr;
}

static void listGames(Client * clients, Client sender, int actual) {
   char message[BUF_SIZE];
   message[0] = 0;

   Client ** clientPairs = malloc(sizeof(Client*)*actual);
   for(int i = 0; i < actual; i++)
   {
      clientPairs[i] = malloc(sizeof(Client)*2);
   }
   int onGoingGames = getOngoingGames(clients, actual, clientPairs);

   strncpy(message, "---- ongoing games ----\n", BUF_SIZE - 1);

   for(int i = 0; i < onGoingGames; i++)
   {
      // the asker's game (if they are playing) will also be shown
      char score[20];
      strncat(message, "- ", sizeof message - strlen(message) - 1);
      strncat(message, clientPairs[i][0].name, sizeof message - strlen(message) - 1);
      strncat(message, " vs ", sizeof message - strlen(message) - 1);
      strncat(message, clientPairs[i][1].name, sizeof message - strlen(message) - 1);
      strncat(message, " (", sizeof message - strlen(message) - 1);
      sprintf(score, "%d", clientPairs[i][0].myScore);
      strncat(message, score, sizeof message - strlen(message) - 1);
      strncat(message, " to ", sizeof message - strlen(message) - 1);
      sprintf(score, "%d", clientPairs[i][1].myScore);
      strncat(message, score, sizeof message - strlen(message) - 1);
      strncat(message, ")\n", sizeof message - strlen(message) - 1);
   }

   strncat(message, "----\n", sizeof message - strlen(message) - 1);

   for(int i = 0; i < actual; i++)
   {
      free(clientPairs[i]);
   }
   free(clientPairs);

   write_client(sender.sock, message);
}

static void observe(Client * player, Client * sender) {
   strcpy(sender->observing, player->name);

   if(player->observers) {
      player->observers[player->actualObservers] = sender->name;
      player->actualObservers++;
   }
}

static void stopObserving(Client * player, Client * observer, Client * clients, int actual) {
   if (player != NULL) {
      // pour chaque nom de client dans observers - on l'enlève et on enlève chez cet observer le player
      for (int i = 0; i < player->actualObservers; i++) {
         if (observer != NULL) {
            if (observer->sock == get_client_by_username(player->observers[i], clients, actual)->sock) {
               strcpy(observer->observing, "");

               /* we remove the client in the array */
               memmove(player->observers + i, player->observers + i + 1, (player->actualObservers - i - 1) * sizeof(BUF_SIZE));
               /* number client - 1 */
               player->actualObservers--;
            }
         }
         else {
            strcpy(get_client_by_username(player->observers[i], clients, actual)->observing, "");

            /* we remove the client in the array */
            memmove(player->observers + i, player->observers + i + 1, (player->actualObservers - i - 1) * sizeof(BUF_SIZE));
            /* number client - 1 */
            player->actualObservers--;
         }
      }
   }
}

static void showObservers(Client * player1, Client * player2, Client * clients, int actual) {
   for (int i = 0; i < player1->actualObservers; i++) {
      if (player1->player == 1) {
         showGame((*player1).holes, numberOfHoles, (*player1).player, (*player2).myScore, (*player1).myScore, player1, player2->name, player1->name, get_client_by_username(player1->observers[i], clients, actual));
      }
      else {
         showGame((*player2).holes, numberOfHoles, (*player2).player, (*player1).myScore, (*player2).myScore, player2, player1->name, player2->name, get_client_by_username(player1->observers[i], clients, actual));
      }
   }
   for (int i = 0; i < player2->actualObservers; i++) {
      if (player1->player == 1) {
         showGame((*player1).holes, numberOfHoles, (*player1).player, (*player2).myScore, (*player1).myScore, player1, player2->name, player1->name, get_client_by_username(player2->observers[i], clients, actual));
      }
      else {
         showGame((*player2).holes, numberOfHoles, (*player2).player, (*player1).myScore, (*player2).myScore, player2, player1->name, player2->name, get_client_by_username(player2->observers[i], clients, actual));
      }
   }
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
