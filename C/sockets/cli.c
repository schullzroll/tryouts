#include "shared.h"
//TODO add check for when dae process stops
/* client side */

int main(int argc, char** argv){

    int lsfd;                   /* local socket file descriptor */
    char buffer[MAX_BUFFLEN];   /* message sent between processes */

    unix_sockaddr remlink;      /* link to remote socket file */

    const socket_settings sset = {
        .domain = AF_UNIX,
        .type = SOCK_STREAM,
        .protocol = 0,
        .remote_path = CONN_SOCK_PATH
    };

    /* create clients socket */
    lsfd = create_socket(sset);

    /* create remote identifier(link) to connect to it later */
    socklen_t addrlen = init_remlink(sset, &remlink);

    /* connect to "daemon" */
    printf("cli> connecting to socket...\n");
    if (connect(lsfd, (const struct sockaddr*)&remlink, addrlen) == -1){
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
        if (send(lsfd, buffer, sizeof(buffer), 0) < -1) {
            perror("send error");
            exit(-1);
        }
        printf("command sent\n");

        printf("$ ");
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
    }

    close(lsfd); 

    return 0;
}
