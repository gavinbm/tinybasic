#include "../inc/tiny.h"

/*          ---- Token Type Map ----
    Specials        Keywords        Operators
    Specials        Keywords        Operators
    EOF     = 1     LABEL    = 6    EQ    = 17
    NEWLINE = 2     GOTO     = 7    PLUS  = 18
    NUMBER  = 3     PRINT    = 8    MINUS = 19
    IDENT   = 4     GET      = 9    MUL   = 20
    STRING  = 5     LET      = 10   DIV   = 21
    CHAR    = 28    IF       = 11   EQEQ  = 22
    INT     = 29    THEN     = 12   NOTEQ = 23
    (       = 30    ENDIF    = 13   LT    = 24
    )       = 31    WHILE    = 14   LTEQ  = 25
                    REPEAT   = 15   GT    = 26
                    ENDWHILE = 16   GTEQ  = 27
                                    MOD   = 32
*/

/*
Takes in a file pointer that is assumed to be a Tiny BASIC file. Reads the file character by
character determing the text and type of each token. Returns a linked list of tokens.
*/
struct Token *lex(FILE *read) {

    int curr_pos, paren_count = 0, key;
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
                    createToken(&tokens, "+", PLUS);
                    break;
                case '-':
                    createToken(&tokens, "-", MINUS);
                    break;
                case '*':
                    createToken(&tokens, "*", MUL);
                    break;
                case '/':
                    createToken(&tokens, "/", DIV);
                    break;
                case '%':
                    createToken(&tokens, "%%", MOD);
                    break;
                /* now we have to check for two character tokens. Check the current
                 char and the next to decide which logical operation is happening. 
                 In order to capture >=, <=, !=, and == we have to give extra space
                 for the second character before we make our token*/
                case '=':
                    if(buffer[i + 1] == '=') {
                        // giving the extra space so the full text can be stored in the token
                        createToken(&tokens, "==", EQEQ);
                        i++;
                    } else {
                        createToken(&tokens, "=", EQ);
                    }
                    break;
                case '<':
                    if(buffer[i + 1] == '=') {
                        createToken(&tokens, "<=", LTEQ);
                        i++;
                    } else {
                        createToken(&tokens, "<", LT);
                    }
                    break;
                case '>':
                    if(buffer[i + 1] == '=') {
                        createToken(&tokens, ">=", GTEQ);
                        i++;
                    } else {
                        createToken(&tokens, ">", GT);
                    }
                    break;
                case '!':
                    if(buffer[i + 1] == '=') {
                        createToken(&tokens, "!=", NOTEQ);
                        i++;
                    } else {// we don't allow for just '!' to be a token, so it's an error
                        printf("Singular \"!\" not supported, must be \"!=\"...\n");
                        exit(1);
                    }
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
                    createToken(&tokens, substr, STRING);
                    free(substr);
                    break;
                // handle individual char's, these can be put in variables and must be 
                // in between single quote marks (identical to the standard C syntax)
                case '\'':
                    i++;
                    substr = malloc(2 * sizeof(char));
                    substr[0] = buffer[i];
                    substr[1] = '\0';
                    createToken(&tokens, substr, CHAR);
                    free(substr);
                    i++;
                    break;
                // handle new lines, these mark the end of a statement and are not tokenized
                case '\n':
                    createToken(&tokens, "\\n", NEWLINE);
                    break;
                // null-terminator, dictates the very end of a statement
                case '\0':
                    createToken(&tokens, "\\0", EOFC);
                    break;
                // handle comments, we don't even want to tokenize these to save memory
                // all comments are single line and begin with a #
                // we're just gonna run over the whole line until we hit a new line char
                case '#':
                    while(buffer[i] != '\n')
                        i++;
                    break;
                case '(':
                    paren_count++;
                    createToken(&tokens, "(", LEFTPAREN);
                    break;
                case ')':
                    paren_count--;
                    createToken(&tokens, ")", RIGHTPAREN);
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
                            createToken(&tokens, substr, IDENT);
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
                            createToken(&tokens, substr, INT);
                            free(substr);
                            i--;
                    } else {
                        printf("unkown token: %s\n", buffer);
                        exit(1);
                    } 
            }
        }
    }

    if(paren_count != 0) {
        printf("unbalanced parentheses...\n");
        exit(1);
    }

    createToken(&tokens, "\\n", NEWLINE);
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
    char keywords[40][10] = {
        "EOF", "NEWLINE", "IDENT", "STRING", "LABEL",
        "GOTO", "PRINT", "GET",
        "LET", "IF", "THEN",
        "ENDIF", "WHILE", "REPEAT",
        "ENDWHILE", "EQ", "PLUS",
        "MINUS", "MUL", "DIV",
        "EQEQ", "NOTEQ", "LT",
        "LTEQ", "GT", "GTEQ",
        "CHAR", "INT", "LEFTPAREN", "RIGHTPAREN",
        "MOD", "OPEN", "CLOSE", "READ",
        "FROM", "WRITE", "INTO", "AS", "STR", "BREAK"};
    
    for(int i = 0; i < 40; ++i) {
        if(strcmp(str, keywords[i]) == 0)
            return i;
    }
    return 0;
}
