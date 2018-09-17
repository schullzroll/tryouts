#include "shared.h"
//TODO add check for when dae process stops
/* client side */

int main(int argc, char** argv){

    int sfd;                /* socket file descriptor */
    char msg[MAX_MSGLEN];   /* message sent between processes */

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
    
    printf("cli> ready to send message\n");
    printf("msg: ");
    scanf("%s", msg);
    while(!feof(stdin)){
        printf("cli> sending message \'%s\'...\n", msg);
        if (send(sfd, msg, strlen(msg), 0) < -1) {
            perror("send error");
            exit(-1);
        }
        printf("message sent\n");
        printf("cli> waiting for reply...\n");

        int nbytes = recv(sfd, msg, MAX_MSGLEN, 0);
        if (nbytes > 0) {
            printf("received message \'%s\'\n", msg);
        }
        else {
            if (nbytes < 0) perror("recv error");
            printf("dae closed connection\n");
            printf("cli> exiting process\n");
            exit(0);
        }

        printf("msg: ");
        memset(msg, 0, strlen(msg));
        scanf("%s", msg);
    }

    close(sfd); 

    return 0;
}
