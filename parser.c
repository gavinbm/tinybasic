#include "tiny.h"

/*
Welcome to the parser, this will walk through our tokens and make sure that each
line of code (which can be thought of as a list of tokens that begins just after a 
newline and ends with a newline) is actually part of our grammar. Each function, barring
some helpers, corresponds to a rule in our grammar and will check that each token is
where it's supposed to be. If any tokens are found out of place, we print an error message
and kill the program with an exit() call.
*/

char *final_code;               // this string will be our final C code that gets written to the C file
char *header_code;
char *footer_code;
struct Variable *vars = NULL;   // Our linked list of variables that will be filled as we parse
struct Label *labels = NULL;    // Out linked list of labels that will be filled as we parse

// first step to get us into the grammar so we can begin parsing
void parse(struct Token *tokens) {
    //printf("PARSER STARTED\n");
    program(tokens);
}

// program ::= {statement}
void program(struct Token *tokens) {
    //printf("PROGRAM\n");
    // set up a tmp poniter to for our tokens that came from the lexer
    struct Token *tmp_tok = tokens;

    // allocate and initialize our C code
    header_code = malloc(37 * sizeof(char));
    footer_code = malloc(sizeof(char));
    final_code = malloc(sizeof(char));
    memcpy(header_code, "#include <stdio.h>\nint main(void) {\n", 37);

    // skip leading white space/newline characters
    while(tmp_tok->type == NEWLINE)
        tmp_tok = tmp_tok->next;

    // step through the basic code and parse the statements
    while(tmp_tok != NULL) {
        tmp_tok = statement(tmp_tok);
    }

    // set the return 0 and ending of our C code
    footer_code = append_line(footer_code, "return 0;\n}");
    // put the code together
    final_code = append_line(final_code, header_code);
    final_code = append_line(final_code, footer_code);
}

/*
statement ::= "PRINT" (expression | string) nl
            | "IF" comparison "THEN" nl {statement} "ENDIF" nl
            | "WHILE" comparison "REPEAT" nl {statement} "ENDWHILE" nl
            | "LABEL" ident nl
            | "GOTO" ident nl
            | "LET" ident "=" expression nl
            | "INPUT" ident nl
*/
struct Token *statement(struct Token *tokens) {
    struct Token *curr_tok = tokens;
    struct Variable *var_t;
    struct Label *label_tmp;
    char *tmp_code, *tmp_code2, *tmp_code3;

    switch(curr_tok->type) {
        // PRINT expression | string nl
        case PRINT:
            //printf("STATEMENT -- PRINT\n");
            curr_tok = curr_tok->next;
            /* 
            if we're printing a string, we set up a printf for the string
            and otherwise we have to set up a printf for a variable (only number vars) 
            */
            if(curr_tok->type == STRING) {
                tmp_code = malloc((curr_tok->len + 15) * sizeof(char));
                strcpy(tmp_code, "printf(\"");
                memcpy(tmp_code + 8, curr_tok->text, curr_tok->len);
                strcpy(tmp_code + curr_tok->len + 8, "\\n\");\n");
                footer_code = append_line(footer_code, tmp_code);
                free(tmp_code);
                curr_tok = curr_tok->next;
            } else {
                var_t = getvar(vars, curr_tok->text);

                if(var_t) {
                    
                    if(var_t->type == 0) {
                        footer_code = append_line(footer_code, "printf(\"%d\\n\", (int)(");
                        curr_tok = expression(curr_tok);
                        footer_code = append_line(footer_code, "));\n"); 
                    } else {
                        footer_code = append_line(footer_code, "printf(\"%c\\n\", (char)(");
                        curr_tok = expression(curr_tok);
                        footer_code = append_line(footer_code, "));\n");
                    }
                }
            }
            break;
        // IF comparison THEN nl {statement} ENDIF nl
        case IF:
            //printf("STATEMENT -- IF\n");
            curr_tok = curr_tok->next;

            // start the if statement and process the comparison
            footer_code = append_line(footer_code, "if(");

            // check whether if it's an ident or a comparison
            if(curr_tok->type == IDENT && !iscomparisonop(curr_tok->next)) {
                footer_code = append_line(footer_code, curr_tok->text);
                curr_tok = curr_tok->next;
            } else {
                curr_tok = comparison(curr_tok);
            }
            // we need to find a THEN (type 12) token and a newline after the comparison
            printf("%s\n", curr_tok->text);
            curr_tok = match(curr_tok, THEN);
            curr_tok = nl(curr_tok);

            // close the if statement, process the statements within the body of the if
            footer_code = append_line(footer_code, ") {\n");
            while(curr_tok->type != ENDIF)
                curr_tok = statement(curr_tok);
            
            // look for the ENDIF (type 13) token and close the if statement body
            curr_tok = match(curr_tok, ENDIF);
            footer_code = append_line(footer_code, "\n}\n");
            break;
        // WHILE comparison REPEAT {statement} nl ENDWHILE
        case WHILE:
            //printf("STATEMENT -- WHILE\n");
            curr_tok = curr_tok->next;

            // begin the while statement and look for the comparison
            footer_code = append_line(footer_code, "while(");
            curr_tok = comparison(curr_tok);

            // we need a REPEAT (type 15) token and a newline after the WHILE
            curr_tok = match(curr_tok, REPEAT);
            curr_tok = nl(curr_tok);

            // close the while statement and process the body of it
            footer_code = append_line(footer_code, ") {\n");
            while(curr_tok->type != ENDWHILE)
                curr_tok = statement(curr_tok);
            
            // find the ENDWHILE (type 16) token and close the body of the while loop
            curr_tok = match(curr_tok, ENDWHILE);
            footer_code = append_line(footer_code, "\n}\n");
            break;
        // LABEL ident nl
        case LABEL:
            //printf("STATEMENT -- LABEL\n");
            curr_tok = curr_tok->next;
            
            // check if the label has already been declared and throw an error if it has
            if(islabel(labels, curr_tok->text)) {
                printf("LABEL ERROR: label [%s] already declared...\n", curr_tok->text);
                exit(4);
            }

            // create the new label, adding it to our list and setting up the C code
            createlabel(&labels, curr_tok->text);
            footer_code = append_line(footer_code, curr_tok->text);
            footer_code = append_line(footer_code, ":\n");
            // LABEL must be followed by an identifier (which is the label name)
            curr_tok = match(curr_tok, IDENT);
            break;
        // GOTO ident nl
        case GOTO:
            //printf("STATEMENT -- GOTO\n");
            curr_tok = curr_tok->next;

            // check if the label has been visited, mark it as visited if it has been
            // throw an error if the label doesn't exist
            if((label_tmp = getlabel(labels, curr_tok->text)) != NULL) {
                label_tmp->visited = 1;
            } else {
                printf("GOTO ERROR: label [%s] does not exist...\n", curr_tok->text);
                exit(5); 
            }

            // setup the goto statement, look for the identifier (label name) after the GOTO
            footer_code = append_line(footer_code, "goto ");
            footer_code = append_line(footer_code, curr_tok->text);
            footer_code = append_line(footer_code, ";\n");
            curr_tok = match(curr_tok, IDENT);
            break;
        // GET (CHAR | INT) ident nl
        case GET:
            //("STATEMENT -- GET\n");
            curr_tok = curr_tok->next;
            
            // We're reading a character
            if(curr_tok->type == CHAR) {
                // move to the ident
                curr_tok = curr_tok->next;

                 // check for the variable (ident that follows the INPUT) in our var list
                // if it's there, we just fill/overwrite its value, otherwise we need to make it
                if(isvariable(vars, curr_tok->text) == 0) {
                    createvar(&vars, curr_tok->text, 1);
                    // decalring the new variable in our C code
                    header_code = append_line(header_code, "char ");
                    header_code = append_line(header_code, curr_tok->text);
                    header_code = append_line(header_code, ";\n");
                } else {
                    // we have to make sure the var type is a char
                    var_t = getvar(vars, curr_tok->text);
                    var_t->type = 1;
                }
                /* 
                set up the scanf that will fill our variable with the new value

                NOTE: we do a bit extra here to check for errors in the input in case
                    scanf gives a bad exit code, this helps us avoid errors in the C code
                    and keep our INPUT functionality pretty simple for the user
                */
                footer_code = append_line(footer_code, "if(scanf(\" %c\", (char *)");
                footer_code = append_line(footer_code, "&");
                footer_code = append_line(footer_code, curr_tok->text);
                footer_code = append_line(footer_code, ") == 0) {\n");
                footer_code = append_line(footer_code, curr_tok->text);
                footer_code = append_line(footer_code, " = 0;\n");
                footer_code = append_line(footer_code, "scanf(\"%*s\");\n}\n");
                // final_code = append_line(final_code, "if((");
                // final_code = append_line(final_code, curr_tok->text);
                // final_code = append_line(final_code, " = getchar()) == EOF) {\n");
                // final_code = append_line(final_code, curr_tok->text);
                // final_code = append_line(final_code, " = 0;\n}\n");
            }
            // We're reading an integer
            else if(curr_tok->type == INT) {
                // move to the ident
                curr_tok = curr_tok->next;

                // check for the variable (ident that follows the INPUT) in our var list
                // if it's there, we just fill/overwrite its value, otherwise we need to make it
                if(isvariable(vars, curr_tok->text) == 0) {
                    createvar(&vars, curr_tok->text, 0);
                    // decalring the new variable in our C code
                    header_code = append_line(header_code, "int ");
                    header_code = append_line(header_code, curr_tok->text);
                    header_code = append_line(header_code, ";\n");
                } else {
                    // we have to make sure the var type is an int
                    var_t = getvar(vars, curr_tok->text);
                    var_t->type = 0;
                }

                /* 
                set up the scanf that will fill our variable with the new value

                NOTE: we do a bit extra here to check for errors in the input in case
                    scanf gives a bad exit code, this helps us avoid errors in the C code
                    and keep our INPUT functionality pretty simple for the user
                */
                footer_code = append_line(footer_code, "if(scanf(\"%d\", (int *)");
                footer_code = append_line(footer_code, "&");
                footer_code = append_line(footer_code, curr_tok->text);
                footer_code = append_line(footer_code, ") == 0) {\n");
                footer_code = append_line(footer_code, curr_tok->text);
                footer_code = append_line(footer_code, " = 0;\n");
                footer_code = append_line(footer_code, "scanf(\"%*s\");\n}\n");
            } 
            // Unrecognized keyword
            else {
                printf("GET ERROR -- Expected \"CHAR\" or \"INT\" -- Got [%s]\n", curr_tok->text);
                exit(10);
            }

            // look for the identifier after the CHAR | INT
            curr_tok = match(curr_tok, IDENT);
            break;
        // OPEN string AS ident nl
        case OPEN:
            // move onto the string
            curr_tok = curr_tok->next;

            // store the string into memory since we'll need it later to
            // open the file
            tmp_code = malloc((curr_tok->len + 1) * sizeof(char));
            strcpy(tmp_code, curr_tok->text);
            
            // make sure we have a string token here
            curr_tok = match(curr_tok, STRING);
            // maksing sure we've got the AS keyword
            curr_tok = match(curr_tok, AS);

            // if it's a variable we're gonna throw an error becaue of potential
            // typing errors
            if(isvariable(vars, curr_tok->text)) {
                printf("Can't read file into existing variable...\n");
                exit(10);
            } else {
                // if it doesn't exist, we're all good
                createvar(&vars, curr_tok->text, 3);
                header_code = append_line(header_code, "FILE *");
                header_code = append_line(header_code, curr_tok->text);
                header_code = append_line(header_code, ";\n");
                footer_code = append_line(footer_code, curr_tok->text);
                footer_code = append_line(footer_code, " = fopen(");
                footer_code = append_line(footer_code, "\"");
                footer_code = append_line(footer_code, tmp_code);
                footer_code = append_line(footer_code, "\", \"w+\");\n");
            }

            // make sure we have an identifier in place
            curr_tok = match(curr_tok, IDENT);
            break;
        // CLOSE ident nl
        case CLOSE:
            // move to the ident
            curr_tok = curr_tok->next;

            // make sure we're closing an existing file pointer
            if(isvariable(vars, curr_tok->text) == 3) {
                footer_code = append_line(footer_code, "fclose(");
                footer_code = append_line(footer_code, curr_tok->text);
                footer_code = append_line(footer_code, ");\n");
            } else {
                printf("CLOSE ERROR: Invalid file pointer, can't close [%s]...", curr_tok->text);
                exit(11);
            }

            // make sure we have an ident and not a keyword
            curr_tok = match(curr_tok, IDENT);
            break;
        // READ (ident | number) FROM ident INTO ident nl
        case READ:
            // move onto the (ident | number)
            curr_tok = curr_tok->next;

            // if it's an integer variable or a number, it's valid
            if(isvariable(vars, curr_tok->text) == 0 || curr_tok->type == NUMBER) {
                footer_code = append_line(footer_code, "fgets(");
                // store the number of bytes we need to read from fgets
                tmp_code = malloc((curr_tok->len + 1) * sizeof(char));
                strcpy(tmp_code, curr_tok->text);
                curr_tok = curr_tok->next;
            } else {
                printf("READ ERROR: Invalid number of bytes to read...\n");
                exit(12);
            }

            // check for the FROM keyword
            curr_tok = match(curr_tok, FROM);

            // check for a valid file pointer or string, throw an error if its not
            if(isvariable(vars, curr_tok->text) == 3 || curr_tok->type == STRING) {
                // store the file pointer/string into tmp_code2 for later emission
                tmp_code2 = malloc((curr_tok->len + 1) * sizeof(char));
                strcpy(tmp_code2, curr_tok->text);
                curr_tok = curr_tok->next;
            } else {
                printf("READ ERROR: Invalid file pointer...\n");
                exit(13);
            }

            // check for the INTO keyword
            curr_tok = match(curr_tok, INTO);

            // if it's a variable, we're gonna trash it since it has to be a string
            if(isvariable(vars, curr_tok->text)) {
                printf("READ ERROR: Can't read file into an existing variable...\n");
                exit(14);
            } else {
                // create and declare our variable
                createvar(&vars, curr_tok->text, 4);
                header_code = append_line(header_code, "char ");
                header_code = append_line(header_code, curr_tok->text);
                header_code = append_line(header_code, "[");
                header_code = append_line(header_code, tmp_code);
                header_code = append_line(header_code, "];\n");

                // finish off the fgets calls in the emitted code
                footer_code = append_line(footer_code, curr_tok->text);
                footer_code = append_line(footer_code, ", ");
                footer_code = append_line(footer_code, tmp_code);
                footer_code = append_line(footer_code, ", ");
                footer_code = append_line(footer_code, tmp_code2);
                footer_code = append_line(footer_code, ");\n");

                // make sure the token is an ident
                curr_tok = match(curr_tok, IDENT);
            }

            free(tmp_code);
            free(tmp_code2);
            break;
        // WRITE (string | ident) INTO ident nl
        case WRITE:
            // move onto what we're writing
            curr_tok = curr_tok->next;

            footer_code = append_line(footer_code, "fputs(");
            // make sure we have a string or ident
            if(isvariable(vars, curr_tok->text) == 4) {
                footer_code = append_line(footer_code, curr_tok->text);
            } 
            else if(curr_tok->type == STRING) {
                footer_code = append_line(footer_code, "\"");
                footer_code = append_line(footer_code, curr_tok->text);
                footer_code = append_line(footer_code, "\"");
            } else {
                printf("WRITE ERROR: Invalid output...\n");
                exit(15);
            }

            footer_code = append_line(footer_code, ", ");

            // check for the INTO keyword
            curr_tok = curr_tok->next;
            curr_tok = match(curr_tok, INTO);

            // make sure we have a valid file pointer
            if(isvariable(vars, curr_tok->text) == 3) { 
                footer_code = append_line(footer_code, curr_tok->text);
            }
            else if(curr_tok->type == STRING) {
                footer_code = append_line(footer_code, "\"");
                footer_code = append_line(footer_code, curr_tok->text);
                footer_code = append_line(footer_code, "\"");  
            } else {
                printf("WRITE ERROR: Invalid file pointer...\n");
                exit(16);
            }

            footer_code = append_line(footer_code, ");\n");
            curr_tok = curr_tok->next;
            break;
        default:
            // {LET} ident = (expression | string) nl
            if(curr_tok->type == IDENT || curr_tok->type == LET) {

                //check for the LET keyword, if it's there we have to move to the next token
                if(curr_tok->type == LET)
                    curr_tok = curr_tok->next;
                
                // check whether this variable exists or not, if it doesn't we need to make it
                if(isvariable(vars, curr_tok->text) == 0) {
                    // check whether it's a char or int and assign the right type value
                    // if it's a char, we assign 1, if it's an int we assign 0
                    if(curr_tok->next->next->type == CHAR) {
                        createvar(&vars, curr_tok->text, 1);
                        // decalring the new variable in our C code
                        header_code = append_line(header_code, "char ");
                        header_code = append_line(header_code, curr_tok->text);
                        header_code = append_line(header_code, ";\n");
                    } else {
                        createvar(&vars, curr_tok->text, 0);
                        // decalring the new variable in our C code
                        header_code = append_line(header_code, "int ");
                        header_code = append_line(header_code, curr_tok->text);
                        header_code = append_line(header_code, ";\n");
                    }
                } else {
                    // if the variable doesn't exist, we have to make sure the type is correct
                    var_t = getvar(vars, curr_tok->text);
                    
                    if(curr_tok->next->next->type == CHAR) {
                        var_t->type = 1;
                    } else {
                        var_t->type = 0;
                    }
                }
                // emit the var name for initlization/value assignment
                footer_code = append_line(footer_code, curr_tok->text);
                // move to the next token and make sure it's an equal sign
                curr_tok = curr_tok->next;
                curr_tok = match(curr_tok, EQ);

                // emit the equal's sign
                footer_code = append_line(footer_code, " = ");
                
                // match the expression from the grammar rule and add newline and semi-colon chars
                curr_tok = expression(curr_tok);
                while(curr_tok->type == RIGHTPAREN) {
                    footer_code = append_line(footer_code, ")");
                    curr_tok = curr_tok->next;
                }
                footer_code = append_line(footer_code, ";\n");
                break;
            }
            printf("INVALID TOKEN -- [%s]\n", curr_tok->text);
            exit(3);
    }
  
    // process newlines
    curr_tok = nl(curr_tok);
    return curr_tok;
}

// comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+
struct Token *comparison(struct Token *curr_token) {
    //printf("COMPARISON\n");
    
    curr_token = expression(curr_token);

    if(iscomparisonop(curr_token)) {
        footer_code = append_line(footer_code, curr_token->text);
        curr_token = curr_token->next;
        curr_token = expression(curr_token);
    } else {
        printf("COMPARISON ERROR...\n");
        exit(1);
    }

    while(iscomparisonop(curr_token)) {
        footer_code = append_line(footer_code, curr_token->text);
        curr_token = curr_token->next;
        curr_token = expression(curr_token);
    }

    return curr_token;
}

// expression ::= term {( "-" | "+" ) term}
struct Token *expression(struct Token *curr_token) {
    //printf("EXPRESSION -- [%s]\n", curr_token->text);
    while(curr_token->type == LEFTPAREN) {
        footer_code = append_line(footer_code, "(");
        curr_token = curr_token->next;
    }
    
    curr_token = term(curr_token);
   
    while(curr_token->type == PLUS || curr_token->type == MINUS) {
        footer_code = append_line(footer_code, curr_token->text);
        curr_token = term(curr_token->next);
    }
   
    return curr_token;
}

// term ::= unary {( "/" | "*" ) unary}
struct Token *term(struct Token *curr_token) {
    //printf("TERM\n");
   
      while(curr_token->type == LEFTPAREN) {
        footer_code = append_line(footer_code, "(");
        curr_token = curr_token->next;
    }

    curr_token = unary(curr_token);

    while(curr_token->type == MUL || curr_token->type == DIV) {
        footer_code = append_line(footer_code, curr_token->text);
        curr_token = unary(curr_token->next);
    }
    return curr_token;
}

// unary ::= ["+" | "-"] primary
struct Token *unary(struct Token *curr_token) {
    //printf("UNARY\n");

    if(curr_token->type == PLUS || curr_token->type == MINUS) {
        footer_code = append_line(footer_code, curr_token->text);
        curr_token = curr_token->next;
    }
  
       while(curr_token->type == LEFTPAREN) {
        footer_code = append_line(footer_code, "(");
        curr_token = curr_token->next;
    }

    curr_token = primary(curr_token);

    return curr_token;
}

// primary ::= number | ident
struct Token *primary(struct Token *curr_token) {
    //printf("PRIMARY -- [%s]\n", curr_token->text);
    switch(curr_token->type) {
        case NUMBER:
            footer_code = append_line(footer_code, curr_token->text);
            curr_token = curr_token->next;
            break;
        case IDENT:
            if(isvariable(vars, curr_token->text) == 0) {
                printf("PRIMARY ERROR: variable [%s] does not exist...\n", curr_token->text);
                exit(5);
            }
            
            footer_code = append_line(footer_code, curr_token->text);
            curr_token = curr_token->next;
            break;
        case CHAR:
            footer_code = append_line(footer_code, "\'");
            footer_code = append_line(footer_code, curr_token->text);
            footer_code = append_line(footer_code, "\'");
            curr_token = curr_token->next;
            break;
        default:
            printf("PRIMARY ERROR: got type [%d]...\n", curr_token->type);
            exit(1);
            break;
    }
    
    return curr_token;
}

// char ::= '(A ... z)'
struct Token *character(struct Token *curr_token) {
    if(curr_token->type == CHAR) {
        footer_code = append_line(footer_code, "\'");
        footer_code = append_line(footer_code, curr_token->text);
        footer_code = append_line(footer_code, "\'");
    } else {
        printf("CHAR ERROR -- Expected type 28 but got %d...", curr_token->type);
        exit(9);
    }

    curr_token = curr_token->next;
    return curr_token;
}

// nl ::= '\n'+
struct Token *nl(struct Token *curr_token) {
    //printf("NEWLINE\n");
    
    while(curr_token != NULL && curr_token->type == NEWLINE)
        curr_token = curr_token->next;

    return curr_token;
}

struct Token *match(struct Token *token, int type) {
     
    if(token != NULL) {
        if(token->type == type) {
            return token->next;
        } else {
            printf("MATCH ERROR: expected type [%d] but got [%d]...\n", type, token->type);
            exit(2);
        }
    }

    return NULL;
}

