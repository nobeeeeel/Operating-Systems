#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "executor.h"

int executeCommand(char *command, char **options)
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "fork() could not create a child process!");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)                                                  //Child process
    {
        execvp(command, options);
        exit(EXIT_FAILURE);                                             //If it got to this line, that means execvp failed and return exit failure
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);                                       //Waits for child process with that pid
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
