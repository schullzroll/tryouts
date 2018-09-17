#include "shared.h"
/* "daemon" side */

int main(int argc, char** argv){

    bool done;
    int sfd;                /* socket file descriptor */
    int cfd;                /* client file descriptor */
    char msg[MAX_MSGLEN];   /* message sent between processes */

    unix_sockaddr local,    /*  */
                  endpoint; /*  */

    /* "default" */
    const socket_defaults defS = {
        .domain = AF_UNIX,
        .type = SOCK_STREAM,
        .protocol = 0
    };

    /* test region */
    const char cmdline[] = {"add list fives fives fives fives fives e"};
    char **tokenatedcmdl;

    printf("len: %zu\n", strlen(cmdline));

    if (!(tokenatedcmdl = tokenate(cmdline))){
        return -1;
    }

    executeCmdl(tokenatedcmdl);

    cmdList(tokenatedcmdl + 1);

    printf("number of args= %zu\n", noargs(tokenatedcmdl + 1));

    freeNames();

    /* creating socket through which connections are established  */
    sfd = socket(defS.domain, defS.type, defS.protocol);
    if (sfd == -1) {
        perror("socket error");
        exit(-1);
    }

    /* setup local socket to bind to sfd */
    local.sa_family = defS.domain;
    strcpy(local.sa_path, CONN_SOCK_PATH);

    /* remove file if exists */
    unlink(local.sa_path);
    /* bind local to sfd */
    socklen_t len = sizeof(local.sa_family) + strlen(local.sa_path);
    printf("dae> binding socket...\n");
    if (bind(sfd, (struct sockaddr*)&local, len) == -1) {
        perror("bind error");
        exit(-1);
    }
    printf("socket binded\n");
    
    /* open sfd to listen for connections */
    printf("dae> opening sfd for incoming connections...\n");
    if (listen(sfd, MAX_NCLIENTS) == -1) {
        perror("listen error");        
        exit(-1);
    }
    printf("sfd opened\n");

    /* main event loop */
    while(1) {
        len = sizeof(endpoint);
        printf("dae> waiting for clients to connect...\n");
        /* accept client connections (wait for clients to connect) */
        cfd = accept(sfd, (struct sockaddr *)&endpoint, &len);
        if (cfd < 0) {
            perror("accept error");
            exit(-1);
        }
       printf("client connected\n");
       
       /* service
          - sending back receieved text */
       do {
           /* recieve a message (wait for message)
              - equivalent to read(), when flags are set to 0 */
           printf("dae> waiting for message from client...\n");
           memset(msg, 0, strlen(msg));
           int nbytes = recv(cfd, msg, MAX_MSGLEN, 0);
           done = false;
           /* check number of received bytes, 0 bytes can be validly received as EOF when peer closed */
           if (nbytes <= 0) {
               if (nbytes < 0) perror("recv error");
               printf("dae> received EOF\n");
               done = true;
           }
           /* printing received message */
           if (!done) {
               printf("received message \'%s\'\n", msg);
           }

           /* send message back when received a msg*/
           if (!done) {
               printf("dae> sending message \'%s\' back...\n", msg);
               if (send(cfd, msg, nbytes, 0) < 0) {
                   perror("send error");               
                   done = true;
               }
               printf("message sent\n");
           }
       } while(!done);

       /* closing client */
       printf("dae> closing clients session!\n");
       close(cfd);
    }

    return 0;
}
