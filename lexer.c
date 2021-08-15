#include "tiny.h"
// int main(int argc, char **argv) {
    
//     FILE *read = fopen(argv[1], "r");
//     struct Token *tokens = lex(read);
//     print_tokens(tokens);
//     free_tokens(tokens);
//     fclose(read);
//     return 0;
// }


struct Token *lex(FILE *read) {

    int curr_pos, key;
    char buffer[MAXLEN], test[2], *substr, curr_char;
    struct Token *tokens = NULL;

    /* read the file line by line so we can tokenize each line
     and analyze each token */
    while(fgets(buffer, MAXLEN, read) != NULL) {
        buffer[strlen(buffer)] = '\0'; // set null terminator for the line
        
        /* iterate over the line char by char so we can determine where each token is
         and what type of token it is */
        for(int i = 0; i < strlen(buffer); ++i) {
            curr_char = buffer[i];
            /* check what character the current one is and set the text of the
             current token accordingly */
            switch(curr_char) {
                case ' ': // the character is a space, we skip these
                    break;
                // arithmetic signs are all standalone tokens and will be stored individually
                case '+':
                    createToken(&tokens, &curr_char, 18);
                    break;
                case '-':
                    createToken(&tokens, &curr_char, 19);
                    break;
                case '*':
                    createToken(&tokens, &curr_char, 20);
                    break;
                case '/':
                    createToken(&tokens, &curr_char, 21);
                    break;
                /* now we have to check for two character tokens. Check the current
                 char and the next to decide which logical operation is happening. 
                 In order to capture >=, <=, !=, and == we have to give extra space
                 for the second character before we make our token*/
                case '=':
                    if(buffer[i + 1] == '=') {
                        // giving the extra space so the full text can be stored in the token
                        substr = malloc(3 * sizeof(char));
                        substr[0] = buffer[i];
                        substr[1] = buffer[i + 1];
                        substr[2] = '\0';
                        createToken(&tokens, substr, 22);
                        free(substr);
                        i++;
                    }
                    else
                        createToken(&tokens, &curr_char, 17);
                    break;
                case '<':
                    if(buffer[i + 1] == '=') {
                        substr = malloc(3 * sizeof(char));
                        substr[0] = buffer[i];
                        substr[1] = buffer[i + 1];
                        substr[2] = '\0';
                        createToken(&tokens, substr, 25);
                        free(substr);
                        i++;
                    }
                    else
                        createToken(&tokens, &curr_char, 24); 
                    break;
                case '>':
                    if(buffer[i + 1] == '=') {
                        substr = malloc(3 * sizeof(char));
                        substr[0] = buffer[i];
                        substr[1] = buffer[i + 1];
                        substr[2] = '\0';
                        createToken(&tokens, substr, 27);
                        free(substr);
                        i++;
                    }
                    else
                        createToken(&tokens, &curr_char, 26); 
                    break;
                case '!':
                    if(buffer[i + 1] == '=') {
                        substr = malloc(3 * sizeof(char));
                        substr[0] = buffer[i];
                        substr[1] = buffer[i + 1];
                        substr[2] = '\0';
                        createToken(&tokens, substr, 23);
                        free(substr);
                        i++;
                    } else // we don't allow for just '!' to be a token, so it's an error
                        printf("WRONG\n"); 
                    break;
                /* Now we process strings, which are of the form
                 "text" where text can be any collection of characters.
                 we have to capture all the characters between the quotes and save them */
                case '\"':
                    i++; // get our current position passed the first qutoation
                    curr_pos = i; // save the position of the first character of the string
                    while(buffer[i] != '\"') {
                        // iterate over the string checking for illegal characters
                        if(buffer[i] == '\n' || buffer[i] == '\t' || buffer[i] == '\r' || buffer[i] == '\\' || buffer[i] == '%') {
                            printf("illegal char in string...\n");
                            exit(1);
                        }
                        i++; // increase the iterator so we keep moving through the file
                    }
                    // allocate memory for and copy the contents of the string 
                    substr = malloc((i - curr_pos) * sizeof(char));
                    memcpy(substr, &buffer[curr_pos], i - curr_pos);
                    substr[i - curr_pos] = '\0'; // set the null-terminator
                    createToken(&tokens, substr, 5);
                    free(substr);
                    break;
                // handle new lines, these mark the end of a statement and are not tokenized
                case '\n':
                    createToken(&tokens, "\\n", 2);
                    break;
                // null-terminator, dictates the very end of a statement
                case '\0':
                    createToken(&tokens, "\\0", 1);
                    break;
                // handle all other tokens, including keywords, variables, and numeric literals
                default:
                    // curr_char is a letter
                    if(isdigit(curr_char) == 0) { 
                        curr_pos = i;
                        while(isalnum(buffer[i])) {
                            i++;
                        }
                        substr = malloc((i - curr_pos) * sizeof(char));
                        memcpy(substr, &buffer[curr_pos], i - curr_pos);
                        substr[i - curr_pos] = '\0'; // set the null-terminator
                        
                        key = iskeyword(substr);
                        if(key) {
                            createToken(&tokens, substr, key);
                        } else {
                            createToken(&tokens, substr, 4);
                        }
                        free(substr);
                        i--;
                    } 
                    // curr_char is a number
                    else if(isdigit(curr_char) != 0) {
                            // save the current position
                            curr_pos = i;
                            while(isdigit(buffer[i]) || buffer[i] == '.') {
                                if(buffer[i] == '.') {
                                    if(!isdigit(buffer[i + 1])) {
                                        printf("invalid decimal number...\n");
                                        exit(1);
                                    }
                                }
                                i++;
                            }
                            substr = malloc((i - curr_pos) * sizeof(char));
                            memcpy(substr, &buffer[curr_pos], i - curr_pos);
                            substr[i - curr_pos] = '\0'; // set the null-terminator
                            createToken(&tokens, substr, 3);
                            free(substr);
                            i--;
                    } else {
                        printf("unkown token: %s\n", buffer);
                        exit(1);
                    } 
            }
        }
    }

    return tokens;
}

void createToken(struct Token **head, char *source, int type) {
    struct Token **tmp = head, *add;
    int source_len = strlen(source);
    
    add = malloc(sizeof(struct Token));
    add->text = malloc(source_len * sizeof(char));
    add->len = source_len;
    add->type = type;
    strcpy(add->text, source);
    add->next = NULL;
    
    while(*tmp)
        tmp = &(*tmp)->next;
    
    add->next = *tmp;
    *tmp = add;
}

int iskeyword(char *str) {
    char keywords[16][8] = {
        "EOF", "NEWLINE", "NUMBER",
        "IDENT", "STRING", "LABEL",
        "GOTO", "PRINT", "INPUT",
        "LET", "IF", "THEN",
        "ENDIF", "WHILE", "REPEAT",
        "ENDWHILE"}; 
        // "EQ", "PLUS",
        // "MINUS", "MUL", "DIV",
        // "EQEQ", "NOTEQ", "LT",
        // "LTEQ", "GT", "GTEQ"};
    
    for(int i = 0; i < 16; ++i) {
        if(strcmp(str, keywords[i]) == 0)
            return i + 1;
    }
    return 0;
}
