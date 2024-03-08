#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "scanner/scanner.h"
#include "parser/shell.h"

int main(int argc, char *argv[]) {
    char *inputLine;
    List tokenList;
    List startOfTokenList;

    while (true) {
        inputLine = readInputLine();

         if (inputLine == NULL) {
            break;
        }

        tokenList = getTokenList(inputLine);
        startOfTokenList = tokenList;
        bool parsedSuccessfully = parseInputLine(&tokenList);
        if (tokenList == NULL && parsedSuccessfully) {
            // Input was parsed successfully and can be accessed in "tokenList"

            // However, this is still a simple list of strings, it might be convenient
            // to build some intermediate structure representing the input line or a
            // command that you then construct in the parsing logic. It's up to you
            // to determine how to approach this!
        } else {
            printf("Error: invalid syntax!\n");
        }

        free(inputLine);
        freeTokenList(startOfTokenList);
        //system("leaks shell");
    }
    return 0;
}
