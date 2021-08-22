#include "tiny.h"

int main(int argc, char **argv) {

    FILE *read = fopen(argv[1], "r");   // the Tiny BASIC source code file
    struct Token *tokens = lex(read);                 // the tokens after lexical analysis

    //parse(tokens); // syntax analysis
 
    free_tokens(tokens);                // free the tokens
    //free_vars(var);               // free the vars
    //free_labels(label);                // free the labels

    fclose(read);                       // close the basic source file

    return 0;
}