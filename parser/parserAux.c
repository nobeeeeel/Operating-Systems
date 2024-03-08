#include "parserAux.h"

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

/**
 * @brief Custom free function to
 * free a string of strings.
 * 
 * @param strings all strings to be freed.
 */
void freeStrings(char ***strings) {
    if (*strings != NULL) {
        for (int i = 0; (*strings)[i] != NULL; i++) {
            free((*strings)[i]);
        }
        free(*strings);
        *strings = NULL;
    }
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
 * @brief checks wether it should
 * skip after an || operator.
 * 
 * @param s the character to be checked.
 * @return true It skips.
 * @return false It does not skip.
 */
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

/**
 * @brief checks wether it should
 * skip after an && operator.
 * 
 * @param s the character to be checked.
 * @return true It skips.
 * @return false It does not skip.
 */
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
 * @brief skips to next available token
 * if a || was processed.
 * 
 * @param lp list of tokens.
 */
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

/**
 * @brief skips to next available token
 * if a && was processed.
 * 
 * @param lp list of tokens.
 */
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

/**
 * @brief logic for all status code,
 * and operators
 * 
 * @param lp list of tokens.
 * @param exitStatus the status code of the operation.
 * @return true 
 * @return false 
 */
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