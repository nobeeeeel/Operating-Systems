#ifndef SHELLAUX_H
#define SHELLAUX_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../scanner/scanner.h"
#include "shell.h"

bool acceptToken(List *lp, char *ident);
void freeStrings(char ***strings);
bool isOperator(char *s);
bool skipFromOrOperator(char *s);
bool skipFromAndOperator(char *s);
void skipToNextCommandOr(List *lp);
void skipToNextCommandAnd(List *lp);
bool handleOperators(List *lp, int *exitStatus);

#endif