/**ill
 * Nikolaos Giovanopoulos - CSD4613
 * csd4613@csd.uoc.gr
 **/
#include "utils.h"

pid_t pid;
STACK s;
int last_fg = -1;
/*STACK FOR PROCESS IDS (SIGTSTP/SIGCONT)*/
void stack_init(STACK *s){s->top=-1;}
int stack_empty(STACK s){return s.top==-1;}
int stack_full(STACK s){return s.top==STACK_SIZE-1;}
int stack_top(STACK s){if (stack_empty) return -1; else return s.array[s.top];}
int stack_push(STACK *s, int x){
    if (stack_full(*s)) return 0;
    else{
        s->array[++s->top] = x;
        return 1;
    }
}
int stack_pop(STACK *s, int *x){
    if (stack_empty(*s)) return 0;
    else{
        *x=s->array[s->top];
        s->top--;
        return 1;
    }
}
/*STACK FOR PROCESS IDS (SIGTSTP/SIGCONT)*/

void type_prompt(){
    char path[PATH_MAX_CHARS];
    char *username = "luivendis"; /*getlogin();
    if(!username) {
        perror("Retrieving login username FAILED.\n");
        exit(EXIT_FAILURE);
    }*/
    if ( !getcwd(path, sizeof(path)) ){
        perror("Retrieving current directory FAILED.\n");
        exit(EXIT_FAILURE);        
    }
    printf("%s@cs345sh%s$", username, path);
}

void removewhitespaces(char *str){
	int  i,j;
	for(i=0;str[i]==' ' || str[i]=='\t';i++);
	for(j=0;str[i];i++){
		str[j++]=str[i];
	}
	str[j]='\0';
	for(i=0;str[i]!='\0';i++){
		if(str[i]!=' '&& str[i]!='\t') j=i;
	}
	str[j+1]='\0';
}

int cmd_parser(char *input, char **words){
    int count = 0;
    while (1){                             
        char word[MAX_LEN_CMD_WORD];
        removewhitespaces(input);
        if (!strlen(input)) break;          //empty input    
        if (input[0] == '\"'){              //PERIPTWSH POU EXOUME STRING "JVFIFVIJI...FJFVIJFIF"
            char *input_ptr = input + 1;    //start after the beginning " char
            int charCount = 0;
            while (input_ptr[0] != '\"'){
                input_ptr++;
                charCount++;
            }
            if (MAX_LEN_CMD_WORD <= charCount){
                perror("Command word too long!\n");
                exit(EXIT_FAILURE);
            }
            strncpy(word, input + 1, charCount); // input+1 : start after the beginning " and charCnt: end before the closing "
            word[charCount] = '\0';              // add null terminator
            words[count] = (char *)malloc(MAX_LEN_CMD_WORD * sizeof(char));
            if (!words[count]){
                perror("Memory allocation for command: Failed.\n");
                exit(EXIT_FAILURE);
            }
            strcpy(words[count], word);
            count++;
            // check if there are chars left to parse or not
            if (strlen(++input_ptr) > 0){
                input = input_ptr;
                continue;
            }
            else return count;
        }
        strcpy(word, strsep(&input, " "));  //PERIPTWSH POU EXOUME KENO
        if (!word) break;                   //nothing to split
        if (!strlen(word)) continue;        //read an empty command, re-iterate
        words[count] = (char *)malloc(MAX_LEN_CMD_WORD * sizeof(char));
        if (!words[count]){
            perror("Memory allocation for command: Failed.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(words[count], word);
        count++;
        if ( !input || !strlen(input)) break;
    }
    return count;
}


//if we want to knwo the number of pipes : retval-1.
int split_cmds(char *input, char **cmds, const char *type){
    if (!input || !strlen(input)) return 0;
    int count = 0;
    char *input_dup = strdup(input);        //preserve the original input by duplication.
    char *input_dup_start = input_dup;      //Keep its pointer in order to free the memory later
    char *token = strtok(input_dup, type);  //since strtok will change it.
    cmds[count] = (char *)malloc(MAX_LEN_CMD_WORD * sizeof(char));
        if (!cmds[count]){
        perror("Memory allocation for commands at split_cmds: Failed.\n");
        exit(EXIT_FAILURE);
    }
    while( token != NULL ){
        removewhitespaces(token);
        strcpy( cmds[count], token );
        count++;
        token = strtok(NULL, type);
        cmds[count] = (char *)malloc(MAX_LEN_CMD_WORD * sizeof(char));
        if (!cmds[count]){
            perror("Memory allocation for commands at split_cmds: Failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    free(input_dup_start);
    return count;
}

void execPipedCmds(char *input, char **cmds,int pipes){
    int numOfPipes = pipes-1;
    int fds[2 * numOfPipes];    //each pipe requires 2 file descriptors(read/write)
    int i;                      // initialize all pipes.
    for (i = 0; i < numOfPipes; i++){
        if (pipe( fds + 2 * i ) == -1){
            perror("Creation of file descriptors for piped commands at execPipedCmds: Failed\n");
            exit(EXIT_FAILURE);
        }
    }
    /*if i is even fd[i] corresponds to the input/read file descriptor of (i+1)-th command.
    if i is odd fd[i] corresponds to the output/write file descriptor of i-th command.*/
    for (i = 0; i < pipes; i++){ //iterate commands
        if ( !cmds[i] ) break;
        char *words[MAX_LEN_CMD_WORD] = {NULL};
        int numOfWords = cmd_parser(cmds[i], words);
        pid = fork();            //start a child process
        if (pid == -1){         
            perror("Fork at execPipedCmds: Failed.\n");
            exit(EXIT_FAILURE);
        }
        if (pid == 0 ){          //child process is running
            if ( i ){        //if that's not the first command in the chain
                if (dup2(fds[(i - 1) * 2], STDIN_FILENO) < 0){  // duplicate READ file descriptor from the previous command
                    perror("Reading input from previous command at execPipedCmds: Failed\n");
                    exit(EXIT_FAILURE);
                }
            }   // if that's not the last command in the chain
            if (i != numOfPipes && cmds[i + 1]){
                // duplicate WRITE file descriptor of current command
                if (dup2(fds[(i * 2 + 1)], STDOUT_FILENO) < 0){
                    perror("Writing output for the next command at execPipedCmds: Failed\n");
                    exit(EXIT_FAILURE);
                }
            }
            for (int j = 0; j < pipes; j++) close(fds[j]);  // close all copies of the file descriptors
            if (execvp(words[0], words) < 0){               // execute command
                perror("Execution of piped command: Failed\n");
                return;
            }
            for (i = 0; i < numOfWords; i++) free(words[i]);
        }
    }
    for (i = 0; i < pipes; i++) close(fds[i]);  // original file descriptors are closed by parent
    for (i = 0; i < pipes; i++) wait(0);        // parent waits all child processes to finish
}

int isvardecl(char* str){
    char* ptr = str;
    int s = 0;
    while(*ptr){
        if (s!=4 && *ptr==' ') return 0; 
        if (s==0 && *ptr!='=' && *ptr!='\"'){
            s=1;
            ptr++;
            continue;
        }
        if (s==1 && *ptr=='\"') return 0;
        if (s==1 && *ptr=='='){
            s=2;
            ptr++;
            continue;
        }
        if (s==2){
            if (*ptr=='\"'){
                s=3;
                ptr++;
                continue;
            }else return 0;
        }
        if (s==3 && *ptr=='\"'){
            s=5;
            ptr++;
            continue;
        }
        if (s==5) return 0;
        ptr++;
    }
    if(s==5) return 1;
    else return 0;
}

void declvar(char* input){
    if ( isvardecl(input) ){
        char *input_dup = strdup(input);        //duplicate input and store pointer due to strtok
        char *input_dup_start = input_dup;
        char *token1 = strtok(input_dup, "\"");
        token1[strlen(token1)-1] = '\0';        //remove trailing string quote
        char *token2 = strtok(NULL, "\"");
        if ( setenv(token1, token2, 1) ) perror("setenv failed miserably!\n");
        free(input_dup_start);
    }
}

void execcmd(char **command, int numOfWords, char* input){
    if ( !strcmp(command[0],"echo") ){
        if ( strchr(command[1], '$') && strlen(command[1])>1){
            char *var = command[1];
            var++;
            const char *path = getenv(var);
            if (!path) printf("\n");
            else printf("%s\n",path);
            return;
        }
    }
    if ( strstr(command[0], "=\"") ) declvar(command[0]);
    else if ( !strcmp(command[0],"fg") ){
        if ( !stack_empty(s) ){
            if ( !stack_pop(&s,&last_fg) ) printf("ERROR AT STACK_POP\n");
            if (!kill(last_fg,SIGCONT)) waitpid(last_fg, NULL, WUNTRACED);
        }

    }
    else if ( !strcmp(command[0], "cd") ){
        if (chdir(command[1]) == -1){
            perror("\nFailed chdir.");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(command[0], "exit") == 0){
        for (int i = 0; i < numOfWords; i++){
            free(command[i]);
        }
        free(input); // free mem allocated by getline
        exit(0);
    }
    else{
        pid = fork();   //create an identical child process
        if (pid == -1){
            perror("\nFailed to fork child..");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0){
            if (execvp(command[0], command) < 0){
                perror("Could not execute given command.\n");
            }
            exit(EXIT_FAILURE);
        }
        else{// wait for child process to finish (except from the case of stop/kill)/
            waitpid(-1,NULL,WUNTRACED);
            return;
        }
    }
}

void parser(char *input){
    int i;
    if ( strchr(input, '|') ){
        char *commands[MAX_PIPES] = {NULL};
        int numOfCmds = split_cmds(input, commands,"|");
        execPipedCmds(input, commands,numOfCmds);
        for (i = 0; i < numOfCmds; i++)
            if (commands[i]) free(commands[i]);
    }
    else{    //single commands
        char *words[MAX_LEN_CMD_WORD] = {NULL};
        int numOfWords = cmd_parser(input, words);
        execcmd(words,numOfWords,input);
        for (i = 0; i < numOfWords; i++) free(words[i]);
    }
}

void sigtstp_handler(int sig) {   
    //printf("\nctrl-z pressed\n");
    if (pid!=0){
        if (kill(pid,SIGTSTP)){
            //perror("kill SIGTSTP failed!\n");
            if ( last_fg!=-1 ){
                if ( kill(last_fg,SIGTSTP) ) perror("kill SIGTSTP failed!\n"); else stack_push(&s,last_fg);
            }
        }
        else
            stack_push(&s,pid);
    }
    fflush(stdout);
}