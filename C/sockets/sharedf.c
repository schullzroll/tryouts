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

char *
rfgets(char **bufp, size_t *sizep, FILE *fp, size_t sizelimit, size_t jump, bool *toolong) { // get cmdl
     size_t len;
     *toolong = false;

     if (fgets(*bufp, *sizep, fp) == NULL) return NULL;
     len = sizeof(*bufp);
     while(strchr(*bufp, '\n') == NULL) {
         if (*sizep + jump > sizelimit){
             printf("Line is too long!\n");
             fflush(fp);
             *toolong = true;
             return NULL;
         }
         *sizep += jump;
         *bufp = realloc(*bufp, *sizep);
         if (!*bufp) return NULL; 
         if(fgets(*bufp + len, *sizep - len, fp) == NULL) return *bufp;
         len += sizeof(*bufp + len);
     }
 
     return *bufp;
 }

