#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#define att(type) __attribute__((type))

#define CONN_SOCK_PATH  "./connsocket"
#define MAX_NCLIENTS    1
/* TODO fix for buffer overflow */
#define MAX_BUFFLEN     80
#define CMDL_LEN        80

typedef char bool;
#define true            1
#define false           0

typedef struct _unix_sockaddr unix_sockaddr;
struct _unix_sockaddr {
    sa_family_t sa_family; /* AF_UNIX or AF_LOCAL */
    char sa_path[255];     /* path to socket file */
};

typedef struct _socket_settings socket_settings;
struct _socket_settings {
    int domain;
    int type;
    int protocol;
    char remote_path[255];
};

/* char** as arguments to functions need to have NULL array block at the end */
typedef int (*cmdhandler) (char**);
typedef struct _cmd cmd;
struct _cmd { 
    const char *strrep;    /* string representation of command */
    const char *help;      /* info for how the command is used */
    cmdhandler handler;    /* pointer to function specific to the cmd entered */
};

/* socket stuff */
int create_socket(socket_settings sset);
socklen_t init_remlink(socket_settings sset, unix_sockaddr* remlink);

int setup_daemon(socket_settings sset,
                 int *lsfd,
                 unix_sockaddr *remlink,
                 socklen_t *addrlen,
                 int maxclients,
                 bool verbose);

int 
setup_client(socket_settings sset,
             int *lsfd,
             unix_sockaddr *remlink,
             socklen_t *addrlen,
             bool verbose);

char* rfgets(char **bufp, size_t *sizep, FILE *fp, size_t sizelimit, size_t jump, bool *toolong); // get cmdl function
char *getArgs(char **tokenizedcmdl);
int executeCmdl(char **tokenatedcmdl);
void freeNames();
size_t noargs(char **args);
char ** tokenate(const char *cmdl);
