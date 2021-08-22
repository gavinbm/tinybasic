#include "tiny.h"
/*          ---- Token Type Map ----
    Specials        Keywords        Operators
    EOF     = 1     LABEL    = 6    EQ    = 17
    NEWLINE = 2     GOTO     = 7    PLUS  = 18
    NUMBER  = 3     PRINT    = 8    MINUS = 19
    IDENT   = 4     INPUT    = 9    MUL   = 20
    STRING  = 5     LET      = 10   DIV   = 21
                    IF       = 11   EQEQ  = 22
                    THEN     = 12   NOTEQ = 23
                    ENDIF    = 13   LT    = 24
                    WHILE    = 14   LTEQ  = 25
                    REPEAT   = 15   GT    = 26
                    ENDWHILE = 16   GTEQ  = 27
*/

/*
Takes in a file pointer that is assumed to be a Tiny BASIC file. Reads the file character by
character determing the text and type of each token. Returns a linked list of tokens.
*/
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
                    substr = malloc(2 * sizeof(char));
                    substr[0] = '+';
                    substr[1] = '\0';
                    createToken(&tokens, substr, 18);
                    free(substr);
                    break;
                case '-':
                    substr = malloc(2 * sizeof(char));
                    substr[0] = '-';
                    substr[1] = '\0';
                    createToken(&tokens, substr, 19);
                    free(substr);
                    break;
                case '*':
                    substr = malloc(2 * sizeof(char));
                    substr[0] = '*';
                    substr[1] = '\0';
                    createToken(&tokens, substr, 20);
                    free(substr);
                    break;
                case '/':
                    substr = malloc(2 * sizeof(char));
                    substr[0] = '/';
                    substr[1] = '\0';
                    createToken(&tokens, substr, 21);
                    free(substr);
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
                    } else {
                        substr = malloc(2 * sizeof(char));
                        substr[0] = '=';
                        substr[1] = '\0';
                        createToken(&tokens, substr, 17);
                        free(substr);
                    }
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
                    } else {
                        substr = malloc(2 * sizeof(char));
                        substr[0] = '<';
                        substr[1] = '\0';
                        createToken(&tokens, substr, 24);
                        free(substr); 
                    }
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
                    } else {
                        substr = malloc(2 * sizeof(char));
                        substr[0] = '>';
                        substr[1] = '\0';
                        createToken(&tokens, substr, 26);
                        free(substr);
                    }
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
                    substr = malloc((i - curr_pos + 1) * sizeof(char));
                    memcpy(substr, &buffer[curr_pos], i - curr_pos + 1);
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
                        substr = malloc((i - curr_pos + 1) * sizeof(char));
                        memcpy(substr, &buffer[curr_pos], i - curr_pos + 1);
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
                            substr = malloc((i - curr_pos + 1) * sizeof(char));
                            memcpy(substr, &buffer[curr_pos], i - curr_pos + 1);
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

/*
Takes a double pointer to a linked list of tokens to which we will append a new node with the 
text of source(the second arg) and a type of type(the third arg).
*/
void createToken(struct Token **head, char *source, int type) {
    struct Token **tmp = head, *add;
    int source_len = strlen(source);
    
    add = malloc(sizeof(struct Token));
    add->text = malloc(source_len + 1 * sizeof(char));
    add->len = source_len;
    add->type = type;
    strcpy(add->text, source);
    add->next = NULL;
    
    while(*tmp)
        tmp = &(*tmp)->next;
    
    add->next = *tmp;
    *tmp = add;
}

/*
Sets up an array of all our possible token types and checks the given string to see if the text matches
the token type.
*/
int iskeyword(char *str) {
    char keywords[27][9] = {
        "EOF", "NEWLINE", "NUMBER",
        "IDENT", "STRING", "LABEL",
        "GOTO", "PRINT", "INPUT",
        "LET", "IF", "THEN",
        "ENDIF", "WHILE", "REPEAT",
        "ENDWHILE", "EQ", "PLUS",
        "MINUS", "MUL", "DIV",
        "EQEQ", "NOTEQ", "LT",
        "LTEQ", "GT", "GTEQ"};
    
    for(int i = 0; i < 27; ++i) {
        if(strcmp(str, keywords[i]) == 0)
            return i + 1;
    }
    return 0;
}
