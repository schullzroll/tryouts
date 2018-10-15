#include "shared.h"
//TODO add check for when dae process stops
/* client side */

int main(int argc, char** argv){

    char buffer[MAX_BUFFLEN];   /* message sent between processes */
    unix_sockaddr remlink;      /* link to remote socket file */
    socklen_t addrlen;
    int lsfd;                   /* local socket file descriptor */

    const socket_settings sset = {
        .domain = AF_UNIX,
        .type = SOCK_STREAM,
        .protocol = 0,
        .remote_path = CONN_SOCK_PATH
    };

    if (setup_client(sset, &lsfd, &remlink, &addrlen, true) < 0)
        return -1;
   
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
