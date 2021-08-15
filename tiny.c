#include "tiny.h"

int main(int argc, char **argv) {

    FILE *read = fopen(argv[1], "r");
    struct Token *tokens = lex(read);
    struct Variable *variables = NULL;
    struct Label *labels = NULL;
    
    parse(tokens, &variables, &labels);

    free_tokens(tokens);
    free_vars(variables);
    free_labels(labels);

    fclose(read);

    return 0;
}