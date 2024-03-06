#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

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
    if (*lp != NULL && strcmp(((*lp)->t), ident) == 0)
    {
        *lp = (*lp)->next;
        return true;
    }
    return false;
}

void freeStrings(char ***strings) {
    if (*strings != NULL) {
        for (int i = 0; (*strings)[i] != NULL; i++) {
            free((*strings)[i]);
        }
        free(*strings);
        *strings = NULL;
    }
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


/**
 * The function parseExecutable parses an executable.
 * @param lp List pointer to the start of the tokenlist.
 * @return a bool denoting whether the executable was parsed successfully.
 */
//TODO: empty this
bool parseExecutable(List *lp, char **executable)
{
    const char *regexPattern = "^(\\/?(\\w|-|_|.)*)$";

    regex_t regex;
    int compileResult = regcomp(&regex, regexPattern, REG_EXTENDED);
    if (compileResult != 0){
        char errorMessage[100];
        regerror(compileResult, &regex, errorMessage, sizeof(errorMessage));
        printf("Failed to compile the regular expression: %s\n", errorMessage);
        return 1;
    }

    // Execute the regular expression
    if (regexec(&regex, (*lp)->t, 0, NULL, 0) == 0)
    {
        strcpy(*executable, (*lp)->t);
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

bool skipFromOrOperator(char *s){
    char *operators[] = {
        ";",
        "\n",
        "&&",
        NULL};

    for (int i = 0; operators[i] != NULL; i++)
    {
        if (strcmp(s, operators[i]) == 0){
            return true;
        }
    }
    return false;
}

bool skipFromAndOperator(char *s){
    char *operators[] = {
        ";",
        "\n",
        "||",
        NULL};

    for (int i = 0; operators[i] != NULL; i++)
    {
        if (strcmp(s, operators[i]) == 0){
            return true;
        }
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
    int numStrings = 1;                                                             //First string is the command
    while (*lp != NULL && !isOperator((*lp)->t))                                    //list must not be empty and not an operator to be an option
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
bool parseCommand(List *lp, int *statusCode)
{
    char *executable = (char *)malloc((strlen((*lp)->t) + 1) * sizeof(char));
    char **options = (char **)malloc(sizeof(char *));

    bool parsedExecutable = parseExecutable(lp, &executable);

    options[0] = (char *)malloc((strlen(executable) + 1) * sizeof(char));  //Initialises the first string to size of command
    strcpy(options[0], executable);     
    bool parsedOptions = parseOptions(lp, &options);

    printf("%s %s\n", executable, options[0]);

    executeCommand(executable, options);
    if(access(executable, X_OK) == 0){
    } else {
        printf("Error: command not found!\n");
        *statusCode = 127;
    }
    
    free(executable);
    freeStrings(&options);
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
bool parsePipeline(List *lp, int *statusCode)
{
    if (!parseCommand(lp, statusCode))
    {
        return false;
    }

    if (acceptToken(lp, "|"))
    {
        return parsePipeline(lp, statusCode);
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
//TODO: empty this
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
    char *builtIns[] = {                                                    //List of commands that are recognized as built in
        "exit",
        "status",
        "echo",
        "false",
        "true",
        NULL};
    *command = (char *)malloc((strlen((*lp)->t) + 1) * sizeof(char));       //Allocates space for the commands for error free freeing
    strcpy(*command, (*lp)->t);
    for (int i = 0; builtIns[i] != NULL; i++)
    {
        if (acceptToken(lp, builtIns[i]))
            return true;
    }
    free(*command);
    return false;
}


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
    if (parseBuiltIn(lp, &command))                                         //Checks to see if current token is in list of built in commands
    {
        if(strcmp(command, "status") == 0){
            printf("The most recent exit code is: %d\n", *statusCode);
            free(command);
            return true;
        }
        options = (char **)malloc(sizeof(char *));                          //Initial malloc for error free freeing
        options[0] = (char *)malloc((strlen(command) + 1) * sizeof(char));  //Initialises the first string to size of command
        strcpy(options[0], command);                                        //Adds the command as the first string in options - as per execvp
        bool parsedOptions = parseOptions(lp, &options);                    //Parses and adds all options if any.
        *statusCode = executeCommand(command, options);                     //Executes command and returns the status code

        free(command);
        freeStrings(&options);
        return parsedOptions;
    } 
    else if (parsePipeline(lp, statusCode))
    {
        return parseRedirections(lp);
    }
    return false;
}

void skipToNextCommandOr(List *lp)
{
    while ((*lp)->t != NULL && !skipFromOrOperator((*lp)->t))       //If current command is not the last one and is
    {                                                               //not an \n, ; or &&, it keeps skipping.
        if ((*lp)->next == NULL)
        {
            *lp = NULL;
            break;
        }
        (*lp) = (*lp)->next;
    }

    if(*lp != NULL)
    {
        if(!((*lp)->t==NULL))
        {
            if((strcmp((*lp)->t, ";") == 0)){
            (*lp) = (*lp)->next;
            }
        }
    }
}

void skipToNextCommandAnd(List *lp)
{
    while ((*lp)->t != NULL && !skipFromAndOperator((*lp)->t))      //If current command is not the last one and is
    {                                                               //not an \n, ; or &&, it keeps skipping.
        if ((*lp)->next == NULL)
        {
            *lp = NULL;
            break;
        }
        (*lp) = (*lp)->next;
    }

    if(*lp != NULL)
    {
        if(!((*lp)->t==NULL))
        {
            if((strcmp((*lp)->t, ";") == 0)){
            (*lp) = (*lp)->next;
            }
        }
    }
}

bool parseInputLine(List *lp);

bool handleOperators(List *lp, int *exitStatus)
{
    bool returnBool = false;
    if ((*exitStatus == 0) && (acceptToken(lp, "&&") || acceptToken(lp, "&")))  //if prior exit code is 0 and following operator is &&, process the input line.
    {
        returnBool = parseInputLine(lp);
    }
    else if ((*exitStatus != 0) && acceptToken(lp, "||"))                       //If prior exit code is not a 0 and following operator is a ||, process the input line.
    {
        returnBool = parseInputLine(lp);
    }
    else if (*exitStatus == 0 && acceptToken(lp, "||"))                         //If exit code is 0 and following operator is ||
    {                 
        skipToNextCommandOr(lp);
        acceptToken(lp, "&&");                                                  //it skips to either a newline, ;, or an &&
        returnBool = parseInputLine(lp);
    }
    else if (*exitStatus != 0 && acceptToken(lp, "&&")){                        //If exit code is not 0, and following operator is &&
        skipToNextCommandAnd(lp);
        acceptToken(lp, "||");                                                  //it skips to either a newline, ;, or an ||
        returnBool = parseInputLine(lp);
    }
    else if (acceptToken(lp, ";"))                                              //If there is a ;, it just parses the following command
    {
        returnBool = parseInputLine(lp);
    }
    acceptToken(lp, ";");
    return returnBool;
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
    if (isEmpty(*lp))                   //Checks if the input is empty, it returns succesfully if it is
    {
        return true;
    }

    if(strcmp((*lp)->t, "exit") == 0){  //Handles exit as input, returns exit succes here
        exit(EXIT_SUCCESS);
    }

    if (!parseChain(lp, &exitStatus))   //Parses a chain
    {
        return false;
    }

    handleOperators(lp, &exitStatus);   //Handles the following operators and its logic if there are any.
    return true;
}