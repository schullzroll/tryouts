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
#define MAX_MSGLEN      255

typedef char bool;
#define true            1
#define false           0

typedef struct _unix_sockaddr unix_sockaddr;
struct _unix_sockaddr {
    sa_family_t sa_family; /* AF_UNIX or AF_LOCAL */
    char sa_path[255];     /* path to socket file */
};

typedef struct _socket_defaults socket_defaults;
struct _socket_defaults {
    int domain;
    int type;
    int protocol;
};

/* char** as arguments to functions need to have NULL array block at the end */
typedef int (*cmdhandler) (char**);
typedef struct _cmd cmd;
struct _cmd { 
    const char *strrep;    /* string representation of command */
    const char *help;      /* info for how the command is used */
    cmdhandler handler;    /* pointer to function specific to the cmd entered */
};

size_t noargs(char **args);
char ** tokenate(const char *cmdl);
