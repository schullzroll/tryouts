#include "shared.h"

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

    /* last strrep must always be NULL */
    {.strrep = NULL}
};

char*
getArgs(char **tokenizedcmdl){
    if (!tokenizedcmdl){
        return NULL; 
    }        
    char **tokcmdlcopy = tokenizedcmdl + 1;
    char* args = NULL;

    while (tokcmdlcopy) {
        args = realloc(args, (strlen(*tokcmdlcopy) + (args) ? strlen(args) + 1: 1));    
        if (!args){
            return NULL;
        }
        args[strlen(args)] = '\0';
        strcat(args, *tokcmdlcopy);

        tokcmdlcopy++;
    }

    return args;
}

void
freeNames(){
    /* free up stuff from test region */
    for (size_t i = 0; i < nnames; i++) {
        free(names[i]);
    }
    free(names);
    return;
}

/* extracts cmd appropriate for given potential cmd */
void *
getHandler(const char* potcmd){
    cmd *cmditerator = commands;
    while (cmditerator->strrep) {
        if (!strncmp(potcmd, cmditerator->strrep, strlen(cmditerator->strrep))) {
            return cmditerator;
        }

        cmditerator++;
    }
    
    return NULL;
}

int
cmdAddPush(const char* name){
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
    newnamepos = realloc(newnamepos, (strlen(name) + 1) * sizeof(**namescp));
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
        if (cmdAddPush(*args) < 0) {
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

/*  tokenate()
 *  Transform command line buffer from client to 2D array of individual 'words'(tokens).
 *  This function doesn't test whether the given buffer is a valid command.
 *
 *  On sucessfull separation returns pointer to first 'word'(token), otherwise NULL
 */
char **
tokenate(const char *cmdl){
    char *cmdlcopy = malloc(sizeof(*cmdl) * (strlen(cmdl) + 1)); 
    if (!cmdlcopy) {
        return NULL;
    }
    cmdlcopy[strlen(cmdl)] = '\0';
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

/*  executeCmdl()
 *  Parses command handler out of tokenated cmdline and performs the execution
 *  of given command.
 *
 *  When successfully parsed command line and found handler, return value of
 *  handler function, otherwise negative value.
 */
int
executeCmdl(char **tokenatedcmdl){
    cmd *command; 

    if (!tokenatedcmdl){
        return -1; 
    }

    if (!*tokenatedcmdl){
        return -1;
    }

    if (!(command = (cmd*) getHandler(*tokenatedcmdl))){
        printf("%s> unknown command \'%s\'!\n", "dae", *tokenatedcmdl); 
        return -1;
    }

    char **args = tokenatedcmdl + 1;

    return command->handler(args);
}
