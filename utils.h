/**
 * Nikolaos Giovanopoulos - CSD4613
 * csd4613@csd.uoc.gr
 **/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include<signal.h>
#include <termios.h>

#define PATH_MAX_CHARS 1024
#define MAX_PIPES 20
#define MAX_CMDS 10
#define MAX_LEN_CMD_WORD 100
#define STACK_SIZE 50
struct stack{
    int array[STACK_SIZE];
    int top;
};
typedef struct stack STACK;
void stack_init(STACK *s);
int stack_empty(STACK s);
int stack_full(STACK s);
int stack_top(STACK s);
int stack_push(STACK *s, int x);
int stack_pop(STACK *s, int *x);
/**
 * Displays the prompt of the shell with information like
 * the username and the current directory.
 **/
void type_prompt();

/**
 * Removes leading and trailing whitespaces from a given string.
 * @param str The string to remove whitespaces
 **/
void removewhitespaces(char *str);

/**
 * Parses given command into words and stores them in the given array
 * @param input The command to parse.
 * @param words The array to store the parsed words.
 * @return The number of words in the given command
 **/
int cmd_parser(char *input, char **words);

/**
 * Splits an input in commands, acoording to the given delimeter and store
 * each command in the given array.
 * @param input The string to split
 * @param cmds The array where each command will be stored.
 * @param type The delimiter character that input contains.
 * @return The number of commands that occur.
 **/
int split_cmds(char *input, char **cmds, const char *type);

/**
 * Executes the the commands of the array using pipes.
 * A child process is created for each command
 * and the processes communicate using the created pipes.
 * @param input The original input(containing pipe symbols).
 * @param cmds The array with the commands for execution.
 * @param pipes The number of pipes we have
 **/
void execPipedCmds(char *input, char **cmds,int pipes);

/**
 * Function that takes a command and returns 1 if it is 
 * a variable declaration and 0 otherwise.
 * @param str The command to check if is a variable declaration.
 * @param return 1 if the command is variable declaration. Otherwise 0.
 **/
int isvardecl(char* str);

/**
 * Function that takes a command and if it is 
 * a variable declaration it sets it as an enviroment variable
 * with the corresponding value
 * @param input The variable declararation command.
 **/
void declvar(char* input);

/**
 * Executes the commands given in an array.
 * @param command An array containing the command to execute in the 
 * first position, followed by the words to execute it with.
 * @param numOfWords The number of words in cmds array. Used to perform cleanup upon exit.
 * @param input The original input in case user exits to free its allocated space.
 **/
void execcmd(char **command, int numOfWords, char* input);

/**
 * Parses the given commands and then executes them.
 * @param input string with the commands to execute.
 **/
void parser(char *input);

/**
 * Signal handler function for SIGSTP (aka Ctrl+Z)
 * @param sig the number of signal occurred.
 **/
void sigtstp_handler(int sig);

