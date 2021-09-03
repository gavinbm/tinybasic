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
    final_code = malloc(37 * sizeof(char));
    strcpy(final_code, "#include <stdio.h>\nint main(void) {\n");

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

    // PRINT expression | string nl
    if(strcmp("PRINT", curr_tok->text) == 0) {
        //printf("STATEMENT -- PRINT\n");
        curr_tok = curr_tok->next;
        /* 
        if we're printing a string, we set up a printf for the string
        and otherwise we have to set up a printf for a variable (only number vars) 
        */
        if(isstring(curr_tok)) {
            tmp_code = malloc((curr_tok->len + 15) * sizeof(char));
            strcpy(tmp_code, "printf(\"");
            memcpy(tmp_code + 8, curr_tok->text, curr_tok->len);
            strcpy(tmp_code + curr_tok->len + 8, "\\n\");\n");
            final_code = append_line(final_code, tmp_code);
            free(tmp_code);
            curr_tok = curr_tok->next;
        }
        else {
            final_code = append_line(final_code, "printf(\"%.2f\\n\", (float)(");
            curr_tok = expression(curr_tok);
            final_code = append_line(final_code, "));\n");
        }
    }
    // IF comparison THEN nl {statement} ENDIF nl
    else if(strcmp("IF", curr_tok->text) == 0) {
        //printf("STATEMENT -- IF\n");
        curr_tok = curr_tok->next;

        // start the if statement and process the comparison
        final_code = append_line(final_code, "if(");
        curr_tok = comparison(curr_tok);

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
    }
    // WHILE comparison REPEAT {statement} nl ENDWHILE
    else if(strcmp("WHILE", curr_tok->text) == 0) {
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
    }
    // LABEL ident nl
    else if(strcmp("LABEL", curr_tok->text) == 0) {
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
    }
    // GOTO ident nl
    else if(strcmp("GOTO", curr_tok->text) == 0) {
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
    }
    // LET ident = {expression} nl
    else if(strcmp("LET", curr_tok->text) == 0) {
        //printf("STATEMENT -- LET\n");
        curr_tok = curr_tok->next;

        /* 
        check if the variable exists already, create it (add it to our list)
        if it's not already there, otherwise we're just updating the value of an existing
        variable
        */
        if(isvariable(var_head, curr_tok->text) == 0) {
            createvar(vars, curr_tok->text, atoi(curr_tok->next->text));
            // we only have numeric vars, floats allow for division so we use those
            final_code = append_line(final_code, "float ");
            final_code = append_line(final_code, curr_tok->text);
            final_code = append_line(final_code, ";\n");
        }
        // add the C code that will set the variables value
        final_code = append_line(final_code, curr_tok->text);
        final_code = append_line(final_code, " = ");

        /* 
        check for an identifer, equals sign, and expression
        for example: this processes the "a = 1" part of "LET a = 1" 
        */
        curr_tok = match(curr_tok, 4);
        curr_tok = match(curr_tok, 17);
        curr_tok = expression(curr_tok);

        final_code = append_line(final_code, ";\n");
    }
    // INPUT ident nl
    else if(strcmp("INPUT", curr_tok->text) == 0) {
        //("STATEMENT -- INPUT\n");
        curr_tok = curr_tok->next;
        
        // check for the variable (ident that follows the INPUT) in our var list
        // if it's there, we just fill/overwrite its value, otherwise we need to make it
        if(isvariable(var_head, curr_tok->text) == 0) {
            createvar(vars, curr_tok->text, atoi(curr_tok->next->text));
            // decalring the new variable in our C code
            final_code = append_line(final_code, "float ");
            final_code = append_line(final_code, curr_tok->text);
            final_code = append_line(final_code, ";\n");
        }

        /* 
        set up the scanf that will fill our variable with the new value

        NOTE: we do a bit extra here to check for errors in the input in case
              scanf gives a bad exit code, this helps us avoid errors in the C code
              and keep our INPUT functionality pretty simple for the user
        */
        final_code = append_line(final_code, "if(scanf(\"%f\", ");
        final_code = append_line(final_code, "&");
        final_code = append_line(final_code, curr_tok->text);
        final_code = append_line(final_code, ") == 0) {\n");
        final_code = append_line(final_code, curr_tok->text);
        final_code = append_line(final_code, " = 0;\n");
        final_code = append_line(final_code, "scanf(\"%*s\");\n}\n");
    
        // look for the identifier after the INPUT
        curr_tok = match(curr_tok, 4);
    }
    /*
     Make LET optional for variables that have already been declared.
     ident "=" expression nl
    */
    else if(curr_tok->type == 4) {
        
        // check whether this variable exists or not, if it doesn't we need to make it
        if(isvariable(var_head, curr_tok->text) == 0) {
            createvar(vars, curr_tok->text, atoi(curr_tok->next->text));
            // decalring the new variable in our C code
            final_code = append_line(final_code, "float ");
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
        curr_tok = expression(curr_tok);
        final_code = append_line(final_code, ";\n");
    }
    // unkown statement
    else {
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

int isstring(struct Token *curr_token) {
    if(curr_token->type == 5) {
        return 1;
    } else {
        return 0;
    }
}

int iscomparisonop(struct Token *curr_token) {
    switch(curr_token->type) {
        case 22:
            return 22;
            break;
        case 23:
            return 23;
            break;
        case 24:
            return 24;
            break;
        case 25:
            return 25;
            break;
        case 26:
            return 26;
            break;
        case 27:
            return 27;
            break;
        default:
            return 0;
            break;
    }
}

int islabel(struct Label *labels, char *name) {
    struct Label *tmp = labels;
    
    while(tmp != NULL) {
        if(strcmp(tmp->name, name) == 0)
            return 1;
        
        tmp = tmp->next;
    }

    return 0;
}

int isvariable(struct Variable *vars, char *name) {
    struct Variable *tmp = vars;
    
    while(tmp != NULL) {
        if(strcmp(tmp->name, name) == 0)
            return 1;
        
        tmp = tmp->next;
    }

    return 0;
}

void createlabel(struct Label **labels, char *name) {
    struct Label **tmp = labels, *add;
    int name_len = strlen(name);

    add = malloc(sizeof(struct Label));
    add->name = malloc(name_len * sizeof(char));
    strcpy(add->name, name);
    add->visited = 0;
    add->next = NULL;

    while(*tmp)
        tmp = &(*tmp)->next;
    
    add->next = *tmp;
    *tmp = add;
}

void createvar(struct Variable **vars, char *name, int value) {
    struct Variable **tmp = vars, *add;
    int name_len = strlen(name);

    add = malloc(sizeof(struct Variable));
    add->name = malloc(name_len + 1 * sizeof(char));
    strcpy(add->name, name);
    add->value = value;
    add->next = NULL;

    while(*tmp)
        tmp = &(*tmp)->next;
    
    add->next = *tmp;
    *tmp = add;
}

struct Label *getlabel(struct Label *labels, char *name) {
    struct Label *tmp = labels;

    while(tmp != NULL) {
        if(strcmp(tmp->name, name) == 0)
            return tmp;
        
        tmp = tmp->next;
    }

    return NULL;
}

struct Variable *getvar(struct Variable *vars, char *name) {
    struct Variable *tmp = vars;

    while(tmp != NULL) {
        if(strcmp(tmp->name, name) == 0)
            return tmp;
        
        tmp = tmp->next;
    }

    return NULL;
}

