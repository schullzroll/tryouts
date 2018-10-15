#include "shared.h"

int
create_socket(socket_settings sset){
    /* create socket file descriptor */
    int sfd = socket(sset.domain, sset.type, sset.protocol);
    if (sfd < 0){
        perror("socket error");
    }

    return sfd;
}

socklen_t
init_remlink(socket_settings sset, unix_sockaddr* remlink){
    remlink->sa_family = sset.domain;
    strcpy(remlink->sa_path, sset.remote_path);

    socklen_t addrlen = sizeof(remlink->sa_family) + strlen(remlink->sa_path);
    
    return addrlen; 
}

