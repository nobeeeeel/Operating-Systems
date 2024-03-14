#ifndef SHELLAUX_H
#define SHELLAUX_H

#include <sys/stat.h>
#include <regex.h>
#include <unistd.h>
#include <errno.h>

#include "../scanner/scanner.h"
#include "shell.h"

bool acceptToken(List *lp, char *ident);
void freeStrings(char ***strings);
bool isOperator(char *s);
bool isBuiltIn(char *s);
bool skipFromOrOperator(char *s);
bool skipFromAndOperator(char *s);
void skipToNextCommandOr(List *lp);
void skipToNextCommandAnd(List *lp);
bool handleOperators(List *lp, int *exitStatus);
bool handleCD(List *lp, int *exitStatus);
bool folderExists(const char *path);
void handleStatus(int statusCode);

#endif
