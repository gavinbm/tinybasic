#include "tiny.h"

/*
The emitter! The final piece! here we will define functions that will be used to spit out
our compiled code. For now let's aim for C but after that getting to assembly should
be not much of an issue.
*/

int write_file(char *filename, char *code) {
    FILE *output = fopen("filename", "w+");

    if(fputs(code, output)) {
        printf("Compiler finished...\n");
        fclose(output);
        return 0;
    } else {
        printf("error writing to c file...\n");
        fclose(output);
        return 1;
    }
}

void init_code(char **code, const char *initial_value) {
    *code = malloc(strlen(initial_value));
    strcpy(*code, initial_value);
}

void prepend_line(char *curr_code, const char *new_code) {
    int curr_len = strlen(curr_code), new_len = strlen(new_code), final_len = curr_len + new_len;

    char *res = realloc(curr_code, new_code);
    if(res) {
        memmove(curr_code + new_len, curr_code, new_len + 1);
        memcpy(curr_code, new_code, new_len);
    } else {
        printf("PREPEND ERROR...\n");
    }
    
}

char *append_line(char *curr_code, char *new_code) {

}

void emit(char *curr_code, char *new_code) {

}