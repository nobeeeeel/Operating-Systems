#include "../executor/executor.h"
#include "../scanner/scanner.h"
#include "parserAux.h"

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

    // Changes an input that starts with / to one without for execvp
    if (strncmp(executable, "./", 2) != 0 && strncmp(executable, "/", 1) == 0) {
        memmove(executable, executable + 1, strlen(executable));

        // Null-terminate the string
        executable[strlen(executable)] = '\0';

    }

    options[0] = (char *)malloc((strlen(executable) + 1) * sizeof(char));  //Initialises the first string to size of command
    strcpy(options[0], executable);     
    bool parsedOptions = parseOptions(lp, &options);
    if(isBuiltIn(executable)){
        *statusCode = executeCommand(executable, options);
    } else if(access(executable, X_OK) == 0){
        *statusCode = executeCommand(executable, options);
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
    *command = (char *)malloc((strlen((*lp)->t) + 1) * sizeof(char));       //Allocates space for the commands for error free freeing
    strcpy(*command, (*lp)->t);
    if(isBuiltIn((*lp)->t)){
        (*lp) = (*lp)->next;
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
            handleStatus(*statusCode);
            free(command);
            return true;
        } else if(strcmp(command, "cd") == 0){
            handleCD(lp, statusCode);
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

bool parseInputLine(List *lp, int *exitStatus);

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
bool parseInputLine(List *lp, int *exitStatus)
{
    if (isEmpty(*lp))                   //Checks if the input is empty, it returns succesfully if it is
    {
        return true;
    }

    if(strcmp((*lp)->t, "exit") == 0){  //Handles exit as input, returns exit succes here
        exit(EXIT_SUCCESS);
    }

    if (!parseChain(lp, exitStatus))   //Parses a chain
    {
        return false;
    }

    handleOperators(lp, exitStatus);   //Handles the following operators and its logic if there are any.
    return true;
}
