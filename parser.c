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
struct Variable *var = NULL;    // Our linked list of variables that will be filled as we parse
struct Label *label = NULL;     // Out linked list of labels that will be filled as we parse
struct Variable **vars = &var;  // A pointer to our variable list, this makes editing the list easier with double pointer tricks
struct Label **labels = &label; // A pointer to our label list, once again, double pointer tricks are great

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
    final_code = malloc(54 * sizeof(char));
    memcpy(final_code, "#include <stdio.h>\nint main(int argc, char **argv) {\n", 54);

    // skip leading white space/newline characters
    while(tmp_tok->type == 2)
        tmp_tok = tmp_tok->next;

    // step through the basic code and parse the statements
    while(tmp_tok != NULL) {
        tmp_tok = statement(tmp_tok);
    }

    // set the return 0 and ending of our C code
    final_code = append_line(final_code, "return 0;\n}");
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
    struct Variable *var_head = *vars;
    struct Label *label_head = *labels, *label_tmp;
    char *tmp_code;

    switch(curr_tok->type) {
        // PRINT expression | string nl
        case 8:
            //printf("STATEMENT -- PRINT\n");
            curr_tok = curr_tok->next;
            /* 
            if we're printing a string, we set up a printf for the string
            and otherwise we have to set up a printf for a variable (only number vars) 
            */
            if(curr_tok->type == 5) {
                tmp_code = malloc((curr_tok->len + 15) * sizeof(char));
                strcpy(tmp_code, "printf(\"");
                memcpy(tmp_code + 8, curr_tok->text, curr_tok->len);
                strcpy(tmp_code + curr_tok->len + 8, "\\n\");\n");
                final_code = append_line(final_code, tmp_code);
                free(tmp_code);
                curr_tok = curr_tok->next;
            }
            else {
                final_code = append_line(final_code, "printf(\"%d\\n\", (int)(");
                curr_tok = expression(curr_tok);
                final_code = append_line(final_code, "));\n");
            }
            break;
        // IF comparison THEN nl {statement} ENDIF nl
        case 11:
            //printf("STATEMENT -- IF\n");
            curr_tok = curr_tok->next;

            // start the if statement and process the comparison
            final_code = append_line(final_code, "if(");

            // check whether if it's an ident or a comparison
            if(curr_tok->type == 4 && !iscomparisonop(curr_tok->next)) {
                final_code = append_line(final_code, curr_tok->text);
                curr_tok = curr_tok->next;
            } else {
                curr_tok = comparison(curr_tok);
            }
            // we need to find a THEN (type 12) token and a newline after the comparison
            curr_tok = match(curr_tok, 12);
            curr_tok = nl(curr_tok);

            // close the if statement, process the statements within the body of the if
            final_code = append_line(final_code, ") {\n");
            while(curr_tok->type != 13)
                curr_tok = statement(curr_tok);
            
            // look for the ENDIF (type 13) token and close the if statement body
            curr_tok = match(curr_tok, 13);
            final_code = append_line(final_code, "\n}\n");
            break;
        // WHILE comparison REPEAT {statement} nl ENDWHILE
        case 14:
            //printf("STATEMENT -- WHILE\n");
            curr_tok = curr_tok->next;

            // begin the while statement and look for the comparison
            final_code = append_line(final_code, "while(");
            curr_tok = comparison(curr_tok);

            // we need a REPEAT (type 15) token and a newline after the WHILE
            curr_tok = match(curr_tok, 15);
            curr_tok = nl(curr_tok);

            // close the while statement and process the body of it
            final_code = append_line(final_code, ") {\n");
            while(curr_tok->type != 16)
                curr_tok = statement(curr_tok);
            
            // find the ENDWHILE (type 16) token and close the body of the while loop
            curr_tok = match(curr_tok, 16);
            final_code = append_line(final_code, "\n}\n");
            break;
        // LABEL ident nl
        case 6:
            //printf("STATEMENT -- LABEL\n");
            curr_tok = curr_tok->next;
            
            // check if the label has already been declared and throw an error if it has
            if(islabel(label_head, curr_tok->text)) {
                printf("LABEL ERROR: label [%s] already declared...\n", curr_tok->text);
                exit(4);
            }

            // create the new label, adding it to our list and setting up the C code
            createlabel(labels, curr_tok->text);
            final_code = append_line(final_code, curr_tok->text);
            final_code = append_line(final_code, ":\n");
            // LABEL must be followed by an identifier (which is the label name)
            curr_tok = match(curr_tok, 4);
            break;
        // GOTO ident nl
        case 7:
            //printf("STATEMENT -- GOTO\n");
            curr_tok = curr_tok->next;

            // check if the label has been visited, mark it as visited if it has been
            // throw an error if the label doesn't exist
            if((label_tmp = getlabel(label_head, curr_tok->text)) != NULL) {
                label_tmp->visited = 1;
            } else {
                printf("GOTO ERROR: label [%s] does not exist...\n", curr_tok->text);
                exit(5); 
            }

            // setup the goto statement, look for the identifier (label name) after the GOTO
            final_code = append_line(final_code, "goto ");
            final_code = append_line(final_code, curr_tok->text);
            final_code = append_line(final_code, ";\n");
            curr_tok = match(curr_tok, 4);
            break;
        // INPUT ident nl
        case 9:
            //("STATEMENT -- INPUT\n");
            curr_tok = curr_tok->next;
            
            // check for the variable (ident that follows the INPUT) in our var list
            // if it's there, we just fill/overwrite its value, otherwise we need to make it
            if(isvariable(var_head, curr_tok->text) == 0) {
                createvar(vars, curr_tok->text, 0);
                // decalring the new variable in our C code
                final_code = append_line(final_code, "int ");
                final_code = append_line(final_code, curr_tok->text);
                final_code = append_line(final_code, ";\n");
            }

            /* 
            set up the scanf that will fill our variable with the new value

            NOTE: we do a bit extra here to check for errors in the input in case
                scanf gives a bad exit code, this helps us avoid errors in the C code
                and keep our INPUT functionality pretty simple for the user
            */
            final_code = append_line(final_code, "if(scanf(\"%d\", ");
            final_code = append_line(final_code, "&");
            final_code = append_line(final_code, curr_tok->text);
            final_code = append_line(final_code, ") == 0) {\n");
            final_code = append_line(final_code, curr_tok->text);
            final_code = append_line(final_code, " = 0;\n");
            final_code = append_line(final_code, "scanf(\"%*s\");\n}\n");
        
            // look for the identifier after the INPUT
            curr_tok = match(curr_tok, 4);
            break;
        default:
            // {LET} ident = (expression | string) nl
            if(curr_tok->type == 4 || curr_tok->type == 10) {

                //check for the LET keyword, if it's there we have to move to the next token
                if(curr_tok->type == 10)
                    curr_tok = curr_tok->next;
                
                // check whether this variable exists or not, if it doesn't we need to make it
                if(isvariable(var_head, curr_tok->text) == 0) {
                    // check whether it's a char or int and assign the right type value
                    // if it's a char, we assign 1, if it's an int we assign 0
                    if(curr_tok->next->next->type == 28) {
                        createvar(vars, curr_tok->text, 1);
                    } else {
                        createvar(vars, curr_tok->text, 0);
                    }
                    // decalring the new variable in our C code
                    final_code = append_line(final_code, "int ");
                    final_code = append_line(final_code, curr_tok->text);
                    final_code = append_line(final_code, ";\n");
                }
                // emit the var name for initlization/value assignment
                final_code = append_line(final_code, curr_tok->text);
                // move to the next token and make sure it's an equal sign
                curr_tok = curr_tok->next;
                curr_tok = match(curr_tok, 17);

                // emit the equal's sign
                final_code = append_line(final_code, " = ");
                
                // match the expression from the grammar rule and add newline and semi-colon chars
                if(curr_tok->type == 28) {
                    curr_tok = character(curr_tok);
                } else {
                    curr_tok = expression(curr_tok);
                }
                final_code = append_line(final_code, ";\n");
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
        final_code = append_line(final_code, curr_token->text);
        curr_token = curr_token->next;
        curr_token = expression(curr_token);
    } else {
        printf("COMPARISON ERROR...\n");
        exit(1);
    }

    while(iscomparisonop(curr_token)) {
        final_code = append_line(final_code, curr_token->text);
        curr_token = curr_token->next;
        curr_token = expression(curr_token);
    }

    return curr_token;
}

// expression ::= term {( "-" | "+" ) term}
struct Token *expression(struct Token *curr_token) {
    //printf("EXPRESSION -- [%s]\n", curr_token->text);
    
    curr_token = term(curr_token);

    while(curr_token->type == 18 || curr_token->type == 19) {
        final_code = append_line(final_code, curr_token->text);
        curr_token = term(curr_token->next);
    }
   
    return curr_token;
}

// term ::= unary {( "/" | "*" ) unary}
struct Token *term(struct Token *curr_token) {
    //printf("TERM\n");
   
    curr_token = unary(curr_token);

    while(curr_token->type == 20 || curr_token->type == 21) {
        final_code = append_line(final_code, curr_token->text);
        curr_token = unary(curr_token->next);
    }
    return curr_token;
}

// unary ::= ["+" | "-"] primary
struct Token *unary(struct Token *curr_token) {
    //printf("UNARY\n");

    if(curr_token->type == 18 || curr_token->type == 19) {
        final_code = append_line(final_code, curr_token->text);
        curr_token = curr_token->next;
    }
  
    curr_token = primary(curr_token);

    return curr_token;
}

// primary ::= number | ident
struct Token *primary(struct Token *curr_token) {
    //printf("PRIMARY -- [%s]\n", curr_token->text);

    switch(curr_token->type) {
        case 3:
            final_code = append_line(final_code, curr_token->text);
            curr_token = curr_token->next;
            break;
        case 4:
            if(isvariable(*vars, curr_token->text) == 0) {
                printf("PRIMARY ERROR: variable [%s] does not exist...\n", curr_token->text);
                exit(5);
            }
            final_code = append_line(final_code, curr_token->text);
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
    if(curr_token->type == 28) {
        final_code = append_line(final_code, "\'");
        final_code = append_line(final_code, curr_token->text);
        final_code = append_line(final_code, "\'");
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
    
    if(curr_token != NULL) {
        curr_token = match(curr_token, 2);
        
        if(curr_token != NULL) {
            while(curr_token->type == 2) {
                curr_token = curr_token->next;
            }
        }
    }

    return curr_token;
}

struct Token *match(struct Token *token, int type) {
     
    if(token->type == type) {
        return token->next;
    } else {
        printf("MATCH ERROR: expected type [%d] but got [%d]...\n", type, token->type);
        exit(2);
    }
}

