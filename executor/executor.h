#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int executeCommand(char *command, char **options);

#endif
