#include "shared.h"
/* "daemon" side */

int main(int argc, char** argv){

    bool done;
    int lsfd;                   /* local socket file descriptor */
    int csfd;                   /* client socket file descriptor */
    char buffer[MAX_BUFFLEN];   /* message sent between processes */
    char **tokenatedcmdl;       /* cmdl divided into tokens (words) */

    unix_sockaddr remlink;      /* link to remote socket file */

    const socket_settings sset = {
        .domain = AF_UNIX,
        .type = SOCK_STREAM,
        .protocol = 0,
        .remote_path = CONN_SOCK_PATH
    };

    /* create local socket */
    lsfd = create_socket(sset);

    /* create remote identifier(link) to bind to it later */
    socklen_t addrlen = init_remlink(sset, &remlink);

    /* remove file if exists */
    unlink(remlink.sa_path);
    /* bind local socket to remote socket*/
    printf("dae> binding socket...\n");
    if (bind(lsfd, (const struct sockaddr*)&remlink, addrlen) == -1) {
        perror("bind error");
        exit(-1);
    }
    printf("socket binded\n");
    
    /* listen for cojnections on remlink */
    printf("dae> opening lsfd for incoming connections...\n");
    if (listen(lsfd, MAX_NCLIENTS) == -1) {
        perror("listen error");        
        exit(-1);
    }
    printf("lsfd opened\n");

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
           memset(buffer, 0, sizeof(buffer));
           int nbytes = recv(csfd, buffer, sizeof(buffer), 0);
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
