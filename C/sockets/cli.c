#include "shared.h"
//TODO add check for when dae process stops
/* client side */

int main(int argc, char** argv){

    char *buffer;                /* message sent between processes */
    size_t buffsize = CMDL_LEN;
    unix_sockaddr remlink;      /* link to remote socket file */
    socklen_t addrlen;
    int lsfd;                   /* local socket file descriptor */
    bool toolong;

    const socket_settings sset = {
        .domain = AF_UNIX,
        .type = SOCK_STREAM,
        .protocol = 0,
        .remote_path = CONN_SOCK_PATH
    };

    /* sets up client according to sset */
    if (setup_client(sset, &lsfd, &remlink, &addrlen, false) < 0)
        return -1;

    printf("cli> ready to send command\n");
    buffer = malloc(sizeof(char) * buffsize);
    //scanf("%s ", buffer);
    /* reads commandline and allocates buffer space */

    /* ask for the cmdline multiple times if user entered in too long */
    // do {
    //     toolong = false;
    //     printf("$ ");
    //     rfgets(&buffer, &buffsize, stdin, CMDL_LEN, 1, &toolong);
    // } while(toolong);

    while(!feof(stdin)){
        memset(buffer, 0, buffsize);
        do {
            toolong = false;
            printf("$ ");
            rfgets(&buffer, &buffsize, stdin, CMDL_LEN, 1, &toolong);
        } while(toolong);

        /* strip newline char from buffer
         * - needs to add else{} branch for when buffer is overrun */
        char* nlpos = strchr(buffer, '\n');
        if (nlpos){
            *nlpos = '\0';
        }

        printf("cli> sending command \'%s\'...\n", buffer);
        if (send(lsfd, buffer, buffsize, 0) < -1) {
            perror("send error");
            exit(-1);
        }
        printf("command sent\n");

    }

    close(lsfd);

    return 0;
}
