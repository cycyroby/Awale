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
users                     - to get the names of all online users\n \
challenge [username]      - to challenge this username to a game. They can either accept or reject the challenge\n \
listGames                 - to list all ongoing games\n \
friends [username]        - to add username to the list of friends\n \
removeFriend [username]   - to remove this username from the list of friends\n \
observe [username]        - to observe a game this username is playing\n \
stopObserving             - to stop observing\n \
showCommands              - to show all the commands for the game\n \
----\n"

#define SHOWCOMMANDS "\n ----\n \
General commands : \n  \
users                     - to get the names of all online users\n  \
chat [username] [message] - to send a chat to this username\n \
bio [message]             - to write a little about yourself\n \
readBio [username]        - to read the bio of this username\n \
friends [username]        - to add username to the list of friends\n \
removeFriend [username]   - to remove this username from the list of friends\n \
challenge [username]      - to challenge this username to a game. They can either accept or reject the challenge\n  \
listGames                 - to list all ongoing games\n \
observe [username]        - to observe a game this username is playing\n \
stopObserving             - to stop observing\n \
private                   - to put yourself in private mode (only your friends can observe you)\n \
public                    - to put yourself in public mode (everyone can observe you)\n \
showCommands              - to show all the commands for the game\n \
Game commands : \n  \
accept                    - to accept a challenge (only if challenged)\n  \
reject                    - to reject a challenge (only if challenged)\n  \
move [index]              - to move seeds from the hole of the given index (only if playing a game)\n \
----\n"
enum commands{
    NULL_COMMAND,
    USERS,
    CHAT,
    BIO,
    READ_BIO,
    FRIENDS,
    REMOVE_FRIENDS,
    CHALLENGE,
    ACCEPT,
    REJECT,
    MOVE,
    LIST_GAMES,
    OBSERVE,
    STOP_OBSERVING,
    PRIVATE,
    PUBLIC,
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
int getOngoingGames(Client * clients, int actual, Client ** clientPairs);
static void listGames(Client * clients, Client sender, int actual);

static void observe(Client * player, Client * sender);
static void stopObserving(Client * player, Client * observer, Client * clients, int actual);
static void showObservers(Client * player1, Client * player2, Client * clients, int actual);

static bool isFriend(Client * player, Client * asker);

#endif /* guard */
