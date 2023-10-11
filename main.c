/**
 * Nikolaos Giovanopoulos - CSD4613
 * csd4613@csd.uoc.gr
 **/
#include "utils.h"

int main(){
    char *input = NULL;  // getline will allocate the buffer
    size_t inputlen = 0;
    struct termios options;             // termios struct
    tcgetattr (0, &options) ;           // Read current options
    options.c_cflag |= IXON ;           // Enable XON flow control on output
    tcsetattr (0, TCSANOW, &options) ;  // Set new options
    signal(SIGTSTP, sigtstp_handler);
    extern STACK s;
    stack_init(&s);
    while (1){
        type_prompt();     /* display prompt */
        if (getline(&input, &inputlen, stdin) == -1){
            perror("Error reading input.");
            exit(EXIT_FAILURE);
        }
        if (input[0] == '\n') continue;
        else{
            input[strlen(input)-1] = '\0'; //remove trailing newline of input
            parser(input);
        }
    }
    return 0;
}