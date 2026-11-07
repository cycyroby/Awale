#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

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
               printf("client talking : name-%s, opponent-%s, state-%d\n", clients[i].name, clients[i].opponent, clients[i].state);
               Client* senderClient = &(clients[i]);
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  if ((*senderClient).state == CHALLENGED || (*senderClient).state == CHALLENGING) {
                     Client* opponent = get_client_by_username((*senderClient).opponent, clients, actual);
                     (*opponent).state = OTHER;
                     strcpy((*opponent).opponent, "");
                  }
                  else if ((*senderClient).state == HAS_TO_PLAY || (*senderClient).state == WAITING_TO_PLAY) {
                     Client* opponent = get_client_by_username((*senderClient).opponent, clients, actual);
                     // à compléter.........
                  }

                  // I think this tells the other clients that "name of client" disconnected
                  strncpy(buffer, (*senderClient).name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, senderClient, actual, buffer, 1);
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
               }
               else
               {
                  char* commandName = malloc(BUF_SIZE*sizeof(char));
                  char* parameter = malloc(BUF_SIZE*sizeof(char));
                  parameter = separateCommandParameter(buffer, commandName, parameter);
                  enum commands command = checkIfCommand(commandName);
                  
                  char message[BUF_SIZE];
                  message[0] = 0;

                  if (command == USERS){
                     printf("in users\n");
                     send_usernames(clients, (*senderClient), actual);
                     if ((*senderClient).state == CHALLENGED) {
                        printf("in users challe\n");
                        strcpy(message, "Don't forget to either accept or reject ");
                        strcat(message, (*senderClient).opponent);
                        strcat(message, "'s challenge\n");
                     }
                     write_client((*senderClient).sock, message);
                  }

                  else if (command == CHALLENGE) {
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
                        // if we actually found a client (the name isn't "")
                        printf("in challe before check receiver is client\n");
                        //if (strcmp((*receiver).name, "") != 0) {
                        if (receiver != NULL) {
                           printf("in challe after check receiver is client\n");
                           if ((*receiver).state == OTHER) {
                              send_challenge(receiver, senderClient);
                              printf(" after send challenge : %s as %d from %s as %d\n",(*senderClient).opponent, (*receiver).state, (*receiver).opponent, (*senderClient).state);
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
                     printf("in challe message sent\n");                  
                  }
                  else if(command == ACCEPT || command == REJECT) {
                     printf("in accept or reject, opponent : %s, state : %d we want 2\n", (*senderClient).opponent, (*senderClient).state);
                     if ((*senderClient).state == CHALLENGED) {
                        printf("in accept rej challe\n");
                        get_answer_challenge(command, senderClient, clients, actual);
                     }
                     else {
                        printf("in accept rej not challe\n");
                        strcpy(message, "Command not recognized. You have not been challenged.\n");
                     }
                     write_client((*senderClient).sock, message);
                  }
                  else if(command == MOVE) {
                     if((*senderClient).state != HAS_TO_PLAY && (*senderClient).state != WAITING_TO_PLAY) {
                        strcpy(message, "You cannot move the seeds as you are not playing.\n");
                     }
                     else if ((*senderClient).state == WAITING_TO_PLAY) {
                        strcpy(message, "You hava to wait for ");
                        strcat(message, (*senderClient).opponent);
                        strcat(message, " to play.\n");
                     }
                     else if ((*senderClient).state == HAS_TO_PLAY){
                        int index = atoi(parameter);
                        if (((*senderClient).player == 1 && index >= 0 && index < 6) ||((*senderClient).player == 2 && index > 5 && index < 12)) {
                           int arrival = moveSeed(index, (*senderClient).holes);
                           if (arrival != -1) {
                              int myScore = collectSeed((*senderClient).player, (*senderClient).holes, (*senderClient).myScore, arrival);
                              (*senderClient).myScore = myScore;
                           }
                           Client *opponent = get_client_by_username((*senderClient).opponent, clients, actual);
                           showGame((*senderClient).holes, numberOfHoles, (*senderClient).player, (*opponent).myScore, (*senderClient).myScore, senderClient);
                           showGame((*senderClient).holes, numberOfHoles, (*opponent).player, (*senderClient).myScore, (*opponent).myScore, opponent); 
                           (*senderClient).state = WAITING_TO_PLAY;
                           (*opponent).state = HAS_TO_PLAY;
                        }

                        else {
                           strcpy(message, "You want to move seeds from a hole that does't belong to you.\n");
                        }

                     }
                     write_client((*senderClient).sock, message);
                  }

                 /* else if (command == SHOW) {
                     strcpy(message, "The first command is 'move [number]' which means you want to move the seeds from one of your holes");
                  }*/
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

static char* separateCommandParameter(const char* buffer, char* commandName, char*parameter) {
   commandName[0] = 0;
   parameter[0] = 0;

   strncpy(commandName, buffer, BUF_SIZE - 1);
   strncpy(parameter, buffer, BUF_SIZE - 1);
   char* ptrCmd = commandName;
   char* ptrPar = parameter;

   while(*ptrCmd != ' ' && *ptrCmd != '\0') {
      ptrCmd++;
      ptrPar++;
   }
   if (*ptrCmd == ' ') {
      ptrPar++;
   }
   else if (*ptrCmd == '\0') {
      ptrPar[0] = 0;
      strncpy(ptrPar, "", BUF_SIZE - 1);
   }
   *ptrCmd = 0;
   parameter = ptrPar;
   return parameter;
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
      command = SHOW;
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
   printf("in get user name start\n");
   if (actual > 0) {
      for(int i = 0; i < actual; i++){
         if(strncmp(name, clients[i].name, sizeof(name))==0){
            return &(clients[i]);
         }
      }
      printf("in get user name after for\n");
      return NULL;
   }
}

static void send_challenge(Client* client, Client* sender){
   strcpy((*client).opponent, (*sender).name);
   strcpy((*sender).opponent, (*client).name);
   (*client).state = CHALLENGED;
   (*sender).state = CHALLENGING;
   printf("in send challenge after state and oppo bouhhhhhh put\n");


   char message[BUF_SIZE];
   message[0] = 0;
   strncpy(message, (*sender).name, BUF_SIZE - 1);
   strncat(message, " \
wants to challenge you to a awale-game. \nDo you want to accept the challenge ?\n\
Please write 'accept' if you do and 'reject' if you don't.\n", sizeof message - strlen(message) - 1);

   
   write_client((*client).sock, message);
   printf("send challenge : %s as %d from %s as %d\n",(*sender).opponent, (*client).state, (*client).opponent, (*sender).state);
}


static void get_answer_challenge(enum commands command, Client * sender, Client * clients, int actual){
      Client* opponent = get_client_by_username((*sender).opponent, clients, actual);
      // the client answered accept
      if (command == ACCEPT){
         printf("in accept\n");
         write_client((*opponent).sock, "Your challenge has been accepted.\n");
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
         printf("%s : %d\n", (*sender).name, (*sender).state);
         printf("%s : %d\n", (*opponent).name, (*opponent).state);
         (*sender).myScore = 0;
         (*opponent).myScore = 0;
         showGame(holes, numberOfHoles, (*sender).player, (*opponent).myScore, (*sender).myScore, sender);

         showGame(holes, numberOfHoles, (*opponent).player, (*sender).myScore, (*opponent).myScore, opponent);
         (*sender).holes = holes;
         (*opponent).holes = holes;

         char opponent_player[20];
         sprintf(opponent_player, "%d", (*opponent).player);
         write_client((*opponent).sock, "You are player : ");
         write_client((*opponent).sock, opponent_player);
         write_client((*opponent).sock, "\n");
         char sender_player[20];
         sprintf(sender_player, "%d", (*sender).player);
         write_client((*sender).sock, "You are player : ");
         write_client((*sender).sock, sender_player);
         write_client((*sender).sock, "\n");

      }
      // the client answered reject
      else if (command == REJECT){
         printf("in reject\n");
         (*sender).state = OTHER;
         (*opponent).state = OTHER;
         strcpy((*sender).opponent, "");
         strcpy((*opponent).opponent, "");
         write_client((*opponent).sock, "Your challenge has been rejected.\n");
      }
}

// what should happen when a person is challenged is that others can continue their life and the challenged cannot 
// write anything exept accept, reject or disconnect
// the challenger cannot challenge another person
// others cannot challenge the two

static int get_random_player_one() {
   //This will give 1 if the challenger is gonna be player one and 2 if he's gonna be player two.
   int n = rand()%2;
   n++;
   return n;
}

void show(char *message, Client *sender) {
   write_client((*sender).sock, message);
}


int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
