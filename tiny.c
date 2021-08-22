#include "tiny.h"

int main(int argc, char **argv) {

    FILE *read = fopen(argv[1], "r");   // the Tiny BASIC source code file
    struct Token *tokens = lex(read);   // the tokens after lexical analysis
    print_tokens(tokens);
    parse(tokens);                      // syntax analysis
    
    printf("[%s]\n", final_code);
    
    write_file("out.c", final_code);
    
    free(final_code);

    free_tokens(tokens);                // free the tokens
    free_vars(var);                     // free the vars
    free_labels(label);                 // free the labels

    fclose(read);                       // close the basic source file

    return 0;
}