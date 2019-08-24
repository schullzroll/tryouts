#include "shared.h"
/* "daemon" side */

int main(int argc, char** argv){

    char *buffer;   /* message sent between processes */
    size_t buffsize = CMDL_LEN;
    char **tokenatedcmdl;       /* cmdl divided into tokens (words) */
    unix_sockaddr remlink;      /* link to remote socket file */
    socklen_t addrlen;
    bool done;
    int lsfd;                   /* local socket file descriptor */
    int csfd;                   /* client socket file descriptor */

    const socket_settings sset = {
        .domain = AF_UNIX,
        .type = SOCK_STREAM,
        .protocol = 0,
        .remote_path = CONN_SOCK_PATH
    };

    // TODO thread pool setup, shared memory blocks, 
    buffer = malloc(buffsize + 1);
    if (!buffer) {
        return -1;
    }
    buffer[buffsize + 1] = '\0';

    if (setup_daemon(sset, &lsfd, &remlink, &addrlen, MAX_NCLIENTS, false) < 0)
        return -1;     

    /* main event loop */
    while(1) {
        addrlen = sizeof(remlink) + strlen(remlink.sa_path);
        printf("dae> waiting for clients to connect...\n");
        /* accept client connections (wait for clients to connect) */
        csfd = accept(lsfd, (struct sockaddr *)&remlink, &addrlen);
        if (csfd < 0) {
            perror("accept error");
            exit(-1);
        }
       printf("client connected\n");
       
       /* service
          - sending back receieved text */
       do {
           /* recieve a message (wait for message)
              - equivalent to read(), when flags are set to 0 */
           printf("dae> waiting for command from client...\n");
           memset(buffer, 0, buffsize);
           buffer[buffsize + 1] = '\0';
           int nbytes = recv(csfd, buffer, buffsize, 0);
           done = false;
           /* check number of received bytes, 0 bytes can be validly received as EOF when peer closed */
           if (nbytes <= 0) {
               if (nbytes < 0) perror("recv error");
               printf("dae> received EOF\n");
               done = true;
           }

           /* parse clients cmdline and execute specified command */
           if (!done) {
               if (!(tokenatedcmdl = tokenate(buffer))){
                   printf("tokenizing error\n");  
               }
               else{
                   printf("received command \'%s\'\n"
                          , buffer);
                   executeCmdl(tokenatedcmdl);
               }
           }
       } while(!done);

       /* closing client */
       printf("dae> closing clients session!\n");
       close(csfd);
    }

    close(lsfd);
    freeNames();
    return 0;
}
