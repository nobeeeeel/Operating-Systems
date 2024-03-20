#include "executor.h"
#include <fcntl.h>

/**
 * @brief Takes in a command and it's options
 * forks, and process the command.
 * 
 * @param command command to be executed.
 * @param options options after the command.
 * @return int the status code of the process.
 */
int executeCommand(char ***commands, char *inputOutput[], int numOfCommands) {
    pid_t pid;
    int status = 0;
    int saved_stdout;
    int saved_stdin;
    int pipefds[2];
    int prev_pipe = STDIN_FILENO;





    //If there is an input file, sets the input to be it
    if(inputOutput[0]!=NULL){
        int file_desc;
        file_desc = open(inputOutput[0], O_RDONLY);
        saved_stdin = dup(STDIN_FILENO);
        dup2(file_desc, STDIN_FILENO);
        close(file_desc);
    }

    //If there is an output file, sets the output to be it
    if(inputOutput[1]!=NULL){
        int file_desc;
        file_desc = open(inputOutput[1], O_WRONLY);
        saved_stdout = dup(STDOUT_FILENO);
        dup2(file_desc, STDOUT_FILENO);
        close(file_desc);
    }

    //Loops for each command
    for (int i = 0; i<numOfCommands; i++) {
        char *executable = (char *)malloc((strlen(commands[i][0])+1)*sizeof(char));
        strcpy(executable, commands[i][0]);

        struct stat s;
        if (stat(executable, &s) == 0 && strlen(executable) > 0) {
            memmove(executable + 2, executable, strlen(executable) + 1); // Shift characters to the right
            executable[0] = '.'; // Add '.' at the beginning
            executable[1] = '/'; // Add '/' at the beginning
        }

        if (i < numOfCommands - 1) {
            pipe(pipefds); // Create pipe for communication between commands
        }

        
        pid = fork(); // Create a new process
        if (pid < 0) { // Error occurred
            fprintf(stderr, "Fork failed\n");
            return 1;
        } else if (pid == 0) { // Child process
            if (prev_pipe != STDIN_FILENO) {
                dup2(prev_pipe, STDIN_FILENO);
                close(prev_pipe); // Close the previous pipe's read end
            }

            // Redirect output to next command if not the last command
            if (i < numOfCommands - 1) {
                dup2(pipefds[1], STDOUT_FILENO);
                close(pipefds[1]); // Close the write end of the pipe (not needed by this process)
            }

            // Execute the command
            status = execvp(executable, commands[i]);

            perror("execvp"); // Print error message
            return -1;
        } else { // Parent process
            wait(NULL);
        }
    }

    if(inputOutput[0]!=NULL){
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }
    if(inputOutput[1]!=NULL){
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }

    return status;
}
