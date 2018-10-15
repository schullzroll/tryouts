#include "shared.h"
//TODO add check for when dae process stops
/* client side */

int main(int argc, char** argv){

    int sfd;                /* socket file descriptor */
    char buffer[MAX_BUFFLEN];   /* message sent between processes */

    unix_sockaddr endpoint; /* socket address to which clients connect */
    /* "default" */
    const socket_defaults defS = {
        .domain = AF_UNIX,
        .type = SOCK_STREAM,
        .protocol = 0
    };

    /* create clients socket */
    sfd = socket(defS.domain, defS.type, defS.protocol);
    if (sfd == -1) {
        perror("socket error");
    }

    /* connect to "daemon" */
    endpoint.sa_family = defS.domain;
    strcpy(endpoint.sa_path, CONN_SOCK_PATH);
    int len = sizeof(endpoint.sa_family) + strlen(endpoint.sa_path);
    printf("cli> connecting to socket...\n");
    if (connect(sfd, (struct sockaddr*)&endpoint, len) == -1){
        perror("connect error");
        exit(-1);
    }
    printf("connected\n");
    
    printf("cli> ready to send command\n");
    printf("$ ");
    fgets(buffer, sizeof(buffer), stdin);
    while(!feof(stdin)){
        /* strip newline char from buffer 
         * - needs to add else{} branch for when buffer is overrun */
        char* nlpos = strchr(buffer, '\n');
        if (nlpos){
            *nlpos = '\0';
        }

        printf("cli> sending command \'%s\'...\n", buffer);
        if (send(sfd, buffer, sizeof(buffer), 0) < -1) {
            perror("send error");
            exit(-1);
        }
        printf("command sent\n");

        printf("$ ");
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
    }

    close(sfd); 

    return 0;
}
