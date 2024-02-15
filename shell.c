#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>

#include "scanner.h"

/**
 * The function acceptToken checks whether the current token matches a target identifier,
 * and goes to the next token if this is the case.
 * @param lp List pointer to the start of the tokenlist.
 * @param ident target identifier
 * @return a bool denoting whether the current token matches the target identifier.
 */
bool acceptToken(List *lp, char *ident)
{
    // printf("comparing %s with %s, bool: %d\n", (*lp)->t, ident, strcmp(((*lp)->t), ident));
    if (*lp != NULL && strcmp(((*lp)->t), ident) == 0)
    {
        *lp = (*lp)->next;
        return true;
    }
    return false;
}

/**
 * The function parseExecutable parses an executable.
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the executable was parsed successfully.
 */
bool parseExecutable(List *lp, char *executable)
{
    const char *regexPattern = "^(\\./)?[a-zA-Z0-9]+$";

    regex_t regex;
    if (regcomp(&regex, regexPattern, REG_EXTENDED) != 0)
    {
        printf("Failed to compile the regular expression\n");
        return 1;
    }

    // Execute the regular expression
    if (regexec(&regex, (*lp)->t, 0, NULL, 0) == 0)
    {
        executable = (char *)malloc(strlen((*lp)->t) + 1);
        strcpy(executable, (*lp)->t);
        (*lp) = (*lp)->next;
    }
    else
    {
        printf("not a regex of ./ or just chars: %s\n", (*lp)->t);
        return 0;
    }
    regfree(&regex);
    // TODO: Determine whether to accept parsing an executable here.
    //
    // It is not recommended to check for existence of the executable already
    // here, since then it'll be hard to continue parsing the rest of the input
    // line (command execution should continue after a "not found" command),
    // it'll also be harder to print the correct error message.
    //
    // Instead, we recommend to just do a syntactical check here, which makes
    // more sense, and defer the binary existence check to the runtime part
    // you'll write later.

    return true;
}

/**
 * Checks whether the input string \param s is an operator.
 * @param s input string.
 * @return a bool denoting whether the current string is an operator.
 */
bool isOperator(char *s)
{
    // NULL-terminated array makes it easy to expand this array later
    // without changing the code at other places.
    char *operators[] = {
        "&",
        "&&",
        "||",
        ";",
        "<",
        ">",
        "|",
        NULL};

    for (int i = 0; operators[i] != NULL; i++)
    {
        if (strcmp(s, operators[i]) == 0)
            return true;
    }
    return false;
}

/**
 * The function parseOptions parses options.
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the options were parsed successfully.
 */
bool parseOptions(List *lp, char ***options)
{
    // TODO: store each (*lp)->t as an option, if any exist
    int numStrings = 1;
    while (*lp != NULL && !isOperator((*lp)->t))
    {
        char *newString = (char *)malloc((strlen((*lp)->t) + 1) * sizeof(char));
        strcpy(newString, (*lp)->t);
        *options = (char **)realloc(*options, (numStrings + 1) * sizeof(char *));

        (*options)[numStrings] = newString;
        numStrings++;
        (*lp) = (*lp)->next;
    }
    *options = (char **)realloc(*options, (numStrings + 2) * sizeof(char *));
    (*options)[numStrings] = NULL;
    return true;
}

/**
 * The function parseRedirections parses a command according to the grammar:
 *
 * <command>        ::= <executable> <options>
 *
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the command was parsed successfully.
 */
bool parseCommand(List *lp)
{
    char *executable = NULL;
    char **options = NULL;
    bool parsedExecutable = parseExecutable(lp, executable);
    bool parsedOptions = parseOptions(lp, &options);
    printf("hello?\n");
    execvp(executable, &executable);

    free(executable);
    // freeArrString(options);
    return parsedExecutable && parsedOptions;
}

/**
 * The function parsePipeline parses a pipeline according to the grammar:
 *
 * <pipeline>           ::= <command> "|" <pipeline>
 *                       | <command>
 *
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the pipeline was parsed successfully.
 */
bool parsePipeline(List *lp)
{
    if (!parseCommand(lp))
    {
        return false;
    }

    if (acceptToken(lp, "|"))
    {
        return parsePipeline(lp);
    }

    return true;
}

/**
 * The function parseFileName parses a filename.
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the filename was parsed successfully.
 */
bool parseFileName(List *lp)
{
    // TODO: Process the file name appropriately
    //char *fileName = (*lp)->t;
    return true;
}

/**
 * The function parseRedirections parses redirections according to the grammar:
 *
 * <redirections>       ::= <pipeline> <redirections>
 *                       |  <builtin> <options>
 *
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the redirections were parsed successfully.
 */
bool parseRedirections(List *lp)
{
    if (isEmpty(*lp))
    {
        return true;
    }

    if (acceptToken(lp, "<"))
    {
        if (!parseFileName(lp))
            return false;
        if (acceptToken(lp, ">"))
            return parseFileName(lp);
        else
            return true;
    }
    else if (acceptToken(lp, ">"))
    {
        if (!parseFileName(lp))
            return false;
        if (acceptToken(lp, "<"))
            return parseFileName(lp);
        else
            return true;
    }

    return true;
}

/**
 * The function parseBuiltIn parses a builtin.
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the builtin was parsed successfully.
 */
bool parseBuiltIn(List *lp, char **command)
{
    char *builtIns[] = {
        "exit",
        "status",
        "echo",
        "false",
        "true",
        NULL};
    *command = (char *)malloc((strlen((*lp)->t) + 1) * sizeof(char));
    strcpy(*command, (*lp)->t);
    for (int i = 0; builtIns[i] != NULL; i++)
    {
        if (acceptToken(lp, builtIns[i]))
            return true;
    }
    return false;
}

int executeCommand(char *command, char **options)
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "fork() could not create a child process!");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        printf("Child process is executing...");
        execvp(command, options);
    }
    else
    { // Only parent process gets here
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
        else
        {
            return -1; // Indicates abnormal termination
        }
    }
    return -1;
}

/*void freeStrings(char **options){
    if(options == NULL){
        return;
    }

    for(int i = 0; *options != NULL; i++){
        //free(options[i]);
    }

    //free(options);
}*/

/**
 * The function parseChain parses a chain according to the grammar:
 *
 * <chain>              ::= <pipeline> <redirections>
 *                       |  <builtin> <options>
 *
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the chain was parsed successfully.
 */
bool parseChain(List *lp, int *statusCode)
{
    char *command = NULL;
    char **options = NULL;
    if (parseBuiltIn(lp, &command))
    {
        options = (char **)malloc(sizeof(char *));
        options[0] = command;
        bool parsedOptions = parseOptions(lp, &options);
        *statusCode = executeCommand(command, options);

        free(command);
        //freeStrings(options);
        return parsedOptions;
    }
    if (parsePipeline(lp))
    {
        return parseRedirections(lp);
    }
    return false;
}

void skipToNextCommand(List *lp)
{
    while ((*lp)->next != NULL && !isOperator((*lp)->t))
    {
        (*lp) = (*lp)->next;
        if ((*lp)->next == NULL)
        {
            *lp = NULL;
            break;
        }
    }
}
bool parseInputLine(List *lp);

bool handleOperators(List *lp, int *exitStatus)
{
    // printf("status code: %d\n", *exitStatus);
    // printf("current token: %s\n", (*lp)->t);
    // printf("next token: %s\n", (*lp)->next->t);
    if ((*exitStatus == 0) && (acceptToken(lp, "&&") || acceptToken(lp, "&")))
    {
        // printf("currently here\n");
        return parseInputLine(lp);
    }
    else if ((*exitStatus != 0) && acceptToken(lp, "||"))
    {
        return parseInputLine(lp);
    }
    else if (*exitStatus == 0 && acceptToken(lp, "||"))
    {
        // printf("skipping\n");
        skipToNextCommand(lp);
        if (acceptToken(lp, "&&"))
        {
            printf("current token: %s\n", (*lp)->t);
            skipToNextCommand(lp);
            printf("next command\n");
        }
        else if (acceptToken(lp, "||"))
        {
            return parseInputLine(lp);
        }
        else if (acceptToken(lp, ";"))
        {
            return parseInputLine(lp);
        }
    }
    else if (acceptToken(lp, ";"))
    {
        printf("current token: %s\n", (*lp)->t);
        return parseInputLine(lp);
    }
    return false;
}

/**
 * The function parseInputLine parses an inputline according to the grammar:
 *
 * <inputline>      ::= <chain> & <inputline>
 *                   | <chain> && <inputline>
 *                   | <chain> || <inputline>
 *                   | <chain> ; <inputline>
 *                   | <chain>
 *                   | <empty>
 *
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the inputline was parsed successfully.
 */
bool parseInputLine(List *lp)
{
    int exitStatus;

    if (isEmpty(*lp))
    {
        return true;
    }

    if (!parseChain(lp, &exitStatus))
    {
        return false;
    }

    handleOperators(lp, &exitStatus);

    return true;
}
