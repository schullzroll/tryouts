#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#define CONN_SOCK_PATH  "./connsocket"
#define MAX_NCLIENTS    1
#define MAX_MSGLEN      255

typedef char bool;
#define true            1
#define false           0

typedef struct _unix_sockaddr unix_sockaddr;
typedef struct _socket_defaults socket_defaults;

struct _unix_sockaddr {
    sa_family_t sa_family; /* AF_UNIX or AF_LOCAL */
    char sa_path[255];     /* path to socket file */
};

struct _socket_defaults {
    int domain;
    int type;
    int protocol;
};
