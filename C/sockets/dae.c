#include "shared.h"
/* "daemon" side */

char **names;
size_t nnames = 0;

int cmdAdd(char **args);
int cmdList(char **args);

cmd commands[] = {
    {.strrep = "add",
     .help = "$ add <name>",
     .handler = cmdAdd,
    },
    {.strrep = "list",
     .help = "$ list",
     .handler = cmdList,
    },

    /* last must always be NULL */
    {.strrep = NULL}
};

/* extracts cmd appropriate for given potential cmd */
void *
getHandler(const char* potcmd){
    cmd *cmditerator = commands;
    while (cmditerator) {
        if (!strncmp(potcmd, cmditerator->strrep, strlen(cmditerator->strrep))) {
            return cmditerator->handler;
        }

        cmditerator++;
    }

    return NULL;
}

int
addname(const char* name){
    if (!name) {
        return -1;
    }
    /* increase size of string array */
    char **namescp = realloc(names, (nnames + 1) * sizeof(*namescp));
    if (!namescp){
        return -1; 
    }

    /* allocate space for last name to be copied to */
    char *newnamepos = namescp[nnames];
    newnamepos = realloc(newnamepos, strlen(name) * sizeof(**namescp));
    if (!newnamepos) {
        return -1;
    }

    /* insert new name to array */
    names = namescp;
    names[nnames] = newnamepos;
    strcpy(names[nnames], name);
    nnames++;
    
    return 0;
}

int
cmdAdd(char **args){
    while(args){
        if (addname(*args) < 0) {
            return -1;    
        }
        args++;
    }

    return 0;
}

size_t
noargs(char **args){
    size_t n;
    
    for (n=0; *args; n++, args++);

    return n;
}

int
cmdList( att(unused) char **args){
    for (size_t i = 0; i < nnames; i++){
        printf("[%zu] \'%s\'\n", i, names[i]);
    }
   
    return 0; 
}

/*known ISSUE, when passing string of strlen = 40, it adds 'Q' to the end of last token
 for now it doesnt seem that bad bud it is still a BUG */
char **
tokenate(const char *cmdl){
    char *cmdlcopy = malloc(sizeof(*cmdl) * strlen(cmdl)); 
    if (!cmdlcopy) {
        return NULL;
    }
    strcpy(cmdlcopy, cmdl);

    const char separators[] = " \r\t";
    char *nexttoken = strtok(cmdlcopy, separators);
    char **tokenatedcmdl = NULL;
    size_t ntokens = 0;

    /* extracting tokens from commandline */
    while (nexttoken) {
        tokenatedcmdl = realloc(tokenatedcmdl, sizeof(*tokenatedcmdl) * ++ntokens);
        if (!tokenatedcmdl) {
            goto cleanup;    
        }

        tokenatedcmdl[ntokens - 1] = nexttoken;
        nexttoken = strtok(NULL, separators);
    }

    /* adding ending NULL ptr to mark end of tokens */
    tokenatedcmdl = realloc(tokenatedcmdl, sizeof(*tokenatedcmdl) * ntokens + 1);
    if (!tokenatedcmdl)
        goto cleanup;
    tokenatedcmdl[ntokens] = NULL;

    return tokenatedcmdl;

 cleanup:
    if (cmdlcopy)
        free(cmdlcopy);
    /* free tokens */
    for (size_t i = 0; i < ntokens; i++) {
        if (tokenatedcmdl[i])
            free(tokenatedcmdl[i]);
    }
    if (tokenatedcmdl)
        free(tokenatedcmdl);
        
    return NULL;
}

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
    const char cmdline[] = {"fives fives fives fives fives fives five"};
    char **parsedcmdl;

    printf("len: %zu\n", strlen(cmdline));

    if (!(parsedcmdl = tokenate(cmdline))){
        return -1;
    }

    cmdAdd(parsedcmdl);
    cmdList(parsedcmdl);

    /* free up stuff from test region */
    for (size_t i = 0; i < nnames; i++) {
        free(names[i]);
    }
    free(names);

    printf("number of args= %zu\n", noargs(parsedcmdl));

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
