#include "tiny.h"

/*
The emitter! The final piece! here we will define functions that will be used to spit out
our compiled code. For now let's aim for C but after that getting to assembly should
be not much of an issue.
*/

int write_file(char *filename, char *code) {
    FILE *output = fopen(filename, "w+");

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

/*
Appeend the given string (new_code) to the end of the other given string (curr_code). Will perform all
malloc's needed to do so and will work provided that curr_code has been malloc'd prior to calling this function.
returns a pointer to a new string.
*/
char *append_line(char *curr_code, char *new_code) {
    int curr_len = strlen(curr_code), added_len = strlen(new_code);

    char *full_code = malloc((curr_len + added_len + 1) * sizeof(char));
    memcpy(full_code, curr_code, curr_len);
    memcpy(full_code + curr_len, new_code, added_len);
    full_code[(curr_len + added_len)] = '\0';
    
    free(curr_code);
    return full_code;
}
