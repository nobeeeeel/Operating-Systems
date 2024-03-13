#include "executor.h"

/**
 * @brief Takes in a command and it's options
 * forks, and process the command.
 * 
 * @param command command to be executed.
 * @param options options after the command.
 * @return int the status code of the process.
 */
int executeCommand(char *command, char **options)
{
    int pipefd[2];
    pid_t pid;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "fork() could not create a child process!");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)                                                  //Child process
    {
        close(pipefd[1]);   // Close write end of the pipe
        dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to the read end of the pipe
        close(pipefd[0]);

        execvp(command, options);
        exit(EXIT_FAILURE);                                             //If it got to this line, that means execvp failed and return exit failure
    }
    else
    {
        close(pipefd[0]);
        int status;
        waitpid(pid, &status, 0);                             //Waits for child process with that pid
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);                                 //Returns the exit status to status code
        }
        else
        {
            return -1;                                                  // Indicates abnormal termination
        }
    }
    return -1;
}
