#ifndef SERVER_H
#define SERVER_H

#ifdef WIN32

#include <winsock2.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF        "\r\n"
#define PORT         1977
#define MAX_CLIENTS  100

#define BUF_SIZE     1024

#define MENU    "\n ----\n \
Commands : \n \
users                - to get the names of all online users\n \
challenge [username] - to challenge this username to a game. They can either accept or reject the challenge\n \
showCommands         - to show all the commands for the game\n \
----\n"
enum commands{
    NULL_COMMAND,
    USERS,
    CHALLENGE,
    ACCEPT,
    REJECT,
    MOVE,
    SHOW_COMMANDS
};

#include "client2.h"
#include <stdbool.h>

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *clients, Client *client, int actual, const char *buffer, char from_server);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);

static void send_usernames(Client *clients, Client sender, int actual);
static enum commands checkIfCommand(const char *buffer);
static int separateCommandParameter(const char* buffer, char* commandName, char*parameter);
static int check_usernames(Client *clients, Client sender, int actual);

static Client* get_client_by_username(const char* name, Client *clients, int actual);
static void send_challenge(Client* client, Client* sender);
static void get_answer_challenge(enum commands command, Client * sender, Client *clients, int actual);

static int get_random_player_one();
static void show(char *message, Client *sender);

static void resetPlayer(Client * player, bool holesFreed);
static void showCommands(Client sender);
#endif /* guard */
