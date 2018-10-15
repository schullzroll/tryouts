#include "shared.h"

int
create_socket(socket_settings sset){
    /* create clients socket */
    int rsfd = socket(sset.domain, sset.type, sset.protocol);

    return rsfd;
}

socklen_t
init_remlink(socket_settings sset, unix_sockaddr* remlink){
    remlink->sa_family = sset.domain;
    strcpy(remlink->sa_path, sset.remote_path);

    socklen_t addrlen = sizeof(remlink->sa_family) + strlen(remlink->sa_path);
    
    return addrlen; 
}

