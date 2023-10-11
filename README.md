# Linux Shell

Unix shell created by Nikos Giovanopoulos as part of an assignment in the course of CS-345 Operating Systems at the University Of Crete.

## Features

- **Single Command Execution:**
  - The shell supports the execution of single commands, including built-in commands such as `cd` to change directories and `exit` to terminate the shell.

- **Global Variables:**
  - Implemented the ability to define and access global variables within the shell.
  - Variables are declared and accessed using a `$` prefix (e.g., `$my_var`) and are set using the `my_var="value"` syntax.
  - Variables can be accessed within the shell and its subshells.

- **Command Parsing:**
  - The input commands are parsed and executed using the `parser()` function.
  - Supports the execution of multiple commands separated by pipes (`|`), which are executed from left to right.
  - Handles the separation of arguments for each command and executes them accordingly.

- **Built-in Commands:**
  - **cd:** Implemented the `cd` command to allow users to change directories using the `chdir()` system call.
  - **exit:** Implemented the `exit` command to terminate the shell.

- **Signal Handling:**
  - **CTRL-Z:** The shell catches the CTRL-Z signal and sets the current running process to the background.
  - **CTRL-C:** Allows users to interrupt the currently running process using the CTRL-C signal.
  - **CTRL-S and CTRL-Q:** Freezing and unfreezing the screen.

- **Pipes Implementation:**
  - Supports the execution of commands separated by pipes (`|`), allowing one command's output to be the input for the next command.
  - Implemented pipes using the `pipe()` system call to establish communication between commands.

## Usage

1. **Running Commands:**
   - Users can input single commands to be executed by the shell.
   - Supports commands with or without arguments, including built-in commands like `cd` and `exit`.
   - Example: `<user>@cs345sh/<dir>$ ls -l`

2. **Global Variables:**
   - Users can define and access global variables within the shell.
   - Define a variable: `<user>@cs345sh/<dir>$ my_var="Hello, World!"`
   - Access a variable: `<user>@cs345sh/<dir>$ echo $my_var`
   - Output: `Hello, World!`

3. **Pipes:**
   - Users can execute commands separated by pipes (`|`) to create pipelines.
   - Example: `<user>@cs345sh/<dir>$ cat file.txt | grep "pattern" | sort`

4. **Changing Directories:**
   - Users can change directories using the `cd` command.
   - Example: `<user>@cs345sh/<dir>$ cd /path/to/directory`

5. **Exiting the Shell:**
   - Users can exit the shell using the `exit` command.
   - Example: `<user>@cs345sh/<dir>$ exit`

## Implementation Details

For each input line received, we call the function parser(), which is responsible for determining whether it contains a single command to execute or multiple commands (e.g., piped commands). In the case of piped commands, parser() will call split_cmds(), which separates the initial line into individual commands. These commands are then executed using execPipedCmds(). Before execution, the arguments of each command need to be separated (if they exist). This task is handled by the cmd_parser() function, which is called internally within execPipedCmds().

If we have single commands, parser() will call cmd_parser() to separate the arguments of the current command. Subsequently, the command is executed using execcmd().

To implement Global Variables in execcmd(), which executes the shell commands, we check if the input contains the '=' character,possibly indicating a variable declaration. In this case, the declvar() function is called with the current command as input. The isvardecl() function checks if it is indeed a declaration statement. If yes, the appropriate handling is performed, and the variable is defined as an environment variable. For printing, we check if the current command has the word "echo" as an argument. If yes, we further check if the immediately following argument is an existing variable using getenv(). If yes, we print it; otherwise, we print a newline.

For Signal Handling, the CTRL-Z signal is caught using a signal_handler, while the fg signal is processed through execcmd() using a simple comparison. In each case, kill() is called with the appropriate arguments, as well as waitpid(), specifically for fg. To support multiple processes, I created a stack where successful SIGTSTP signaling pushes the process, and an additional variable holds the last extracted PID. This was necessary because, for some reason, after extracting and returning the process to the foreground, if Ctrl+Z is pressed again, the SIGTSTP signal is not sent, so the process is not stored in the stack, and thus it is lost. CTRL-S and CTRL-Q did not require any handling. Enabling XON flow control was enough, using the "termios" library, by creating a new "termios" struct and setting its parameters in the system.