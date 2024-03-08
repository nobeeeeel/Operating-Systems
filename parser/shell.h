#ifndef SHELL_SHELL_H
#define SHELL_SHELL_H

#include <stdbool.h>

bool parseExecutable(List *lp, char **executable);
bool parseOptions(List *lp, char ***options);
bool parseCommand(List *lp, int *statusCode);
bool parsePipeline(List *lp, int *statusCode);
bool parseFileName(List *lp);
bool parseRedirections(List *lp);
bool parseBuiltIn(List *lp, char **command);
bool parseChain(List *lp, int *statusCode);
bool parseInputLine(List *lp);

#endif
