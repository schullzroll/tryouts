static int
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
add(char **args){
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
list( att(unused) char **args){
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
