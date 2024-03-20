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

    if(inputOutput[0]!=NULL){

    }
    if(inputOutput[1]!=NULL){
        int file_desc;
        file_desc = open(inputOutput[1], O_WRONLY);
        saved_stdout = dup(STDOUT_FILENO);
        dup2(file_desc, STDOUT_FILENO);
        close(file_desc);
    }

    for (int i = 0; i<numOfCommands; i++) {
        char *executable = (char *)malloc((strlen(commands[i][0])+1)*sizeof(char));
        strcpy(executable, commands[i][0]);

        struct stat s;
        if (stat(executable, &s) == 0 && strlen(executable) > 0) {
            memmove(executable + 2, executable, strlen(executable) + 1); // Shift characters to the right
            executable[0] = '.'; // Add '.' at the beginning
            executable[1] = '/'; // Add '/' at the beginning
        }
        
        pid = fork(); // Create a new process
        if (pid < 0) { // Error occurred
            fprintf(stderr, "Fork failed\n");
            return 1;
        } else if (pid == 0) { // Child process

            // Execute the command
            status = execvp(executable, commands[i]);

            perror("execvp"); // Print error message
            return -1;
        } else { // Parent process
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
            wait(NULL);
        }
    }

    return status;
}
