#include "tiny.h"

int main(int argc, char **argv) {

    if(argc == 3) {
        FILE *read = fopen(argv[1], "r");   // the Tiny BASIC source code file
        struct Token *tokens = lex(read);   // the tokens after lexical analysis
        
        parse(tokens);                      // syntax analysis

        write_file(argv[2], final_code);
        
        free(final_code);

        free_tokens(tokens);                // free the tokens
        free_vars(var);                     // free the vars
        free_labels(label);                 // free the labels

        fclose(read);                       // close the basic source file
    } else {
        printf("Please provide BASIC file and desired C file name...\n");
        exit(1);
    }

    return 0;
}