#ifndef SHELL_SHELL_H
#define SHELL_SHELL_H

#include <stdbool.h>

bool parseExecutable(List *lp, char **executable);
bool parseOptions(List *lp, char ***options);
bool parseCommand(List *lp, int *statusCode, char **command);
bool parsePipeline(List *lp, int *statusCode, char ****commands);
bool parseFileName(List *lp, char **filename);
bool parseRedirections(List *lp, int *statusCode, char *arr[]);
bool parseBuiltIn(List *lp, char **command);
bool parseChain(List *lp, int *statusCode);
bool parseInputLine(List *lp, int *statuscode);

#endif
