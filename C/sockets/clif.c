#include "shared.h"

static int
connect_socket(int lsfd,
               unix_sockaddr remlink,
               socklen_t addrlen,
               bool verbose){
    int retval;

    if (verbose) printf("cli> connecting to socket...\n");

    if ((retval = connect(lsfd, (const struct sockaddr*) &remlink, addrlen)) < 0){
        perror("connect error");
        return retval;
    }

    if (verbose) printf("cli> connected\n");

    return 0;
}


int 
setup_client(socket_settings sset,
             int *lsfd,
             unix_sockaddr *remlink,
             socklen_t *addrlen,
             bool verbose){

    int retval;

    *lsfd = create_socket(sset);
    *addrlen = init_remlink(sset, remlink);

    if ((retval = connect_socket(*lsfd, *remlink, *addrlen, verbose)) < 0) return retval;

    return 0;
}
