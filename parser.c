#include "tiny.h"

/*
Welcome to the parser, this will walk through our tokens and make sure that each
line of code (which can be thought of as a list of tokens that begins just after a 
newline and ends with a newline) is actually part of our grammar. Each function, barring
some helpers, corresponds to a rule in our grammar and will check that each token is
where it's supposed to be. If any tokens are found out of place, we print an error message
and kill the program with an exit() call.
*/

char *final_code;
struct Variable *var = NULL;
struct Label *label = NULL;
struct Variable **vars = &var;
struct Label **labels = &label;

// initializes the grammar for us to step through
void parse(struct Token *tokens) {
    //printf("PARSER STARTED\n");
    program(tokens);
}

// program ::= {statement}
void program(struct Token *tokens) {
    //printf("PROGRAM\n");
    
    struct Token *tmp_tok = tokens;
    final_code = malloc(37 * sizeof(char));
    strcpy(final_code, "#include <stdio.h>\nint main(void) {\n");
    printf("[%s]\n", final_code);

    while(tmp_tok->type == 2)
        tmp_tok = tmp_tok->next;

    while(tmp_tok != NULL) {
        tmp_tok = statement(tmp_tok);
    }
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
            tmp_code = malloc(9 * sizeof(char));
            strcpy(tmp_code, "printf(\"");
            final_code = append_line(final_code, tmp_code);
            curr_tok = expression(curr_tok);
            final_code = append_line(final_code, "\\n\");\n");
            free(tmp_code);
        }
    }
    // IF comparison THEN nl {statement} ENDIF nl
    else if(strcmp("IF", curr_tok->text) == 0) {
        //printf("STATEMENT -- IF\n");
        curr_tok = curr_tok->next;
        curr_tok = comparison(curr_tok);

        curr_tok = match(curr_tok, 12);
        curr_tok = nl(curr_tok);

        while(curr_tok->type != 13)
            curr_tok = statement(curr_tok);
        
        curr_tok = match(curr_tok, 13);
    }
    // WHILE comparison REPEAT {statement} nl ENDWHILE
    else if(strcmp("WHILE", curr_tok->text) == 0) {
        //printf("STATEMENT -- WHILE\n");
        curr_tok = curr_tok->next;
        
        curr_tok = comparison(curr_tok);

        curr_tok = match(curr_tok, 15);
        curr_tok = nl(curr_tok);

        while(curr_tok->type != 16)
            curr_tok = statement(curr_tok);
        
        curr_tok = match(curr_tok, 16);
    }
    // extra condition for ENDWHILE to conclude WHILE statement
    else if(strcmp("ENDWHILE", curr_tok->text) == 0) {
        //printf("STATEMENT -- ENDWHILE\n");
        curr_tok = match(curr_tok, 16);
    }
    // LABEL ident nl
    else if(strcmp("LABEL", curr_tok->text) == 0) {
        //printf("STATEMENT -- LABEL\n");
        curr_tok = curr_tok->next;
        
        if(islabel(label_head, curr_tok->text)) {
            printf("LABEL ERROR: label [%s] already declared...\n", curr_tok->text);
            exit(4);
        }
        createlabel(labels, curr_tok->text);

        curr_tok = match(curr_tok, 4);
    }
    // GOTO ident nl
    else if(strcmp("GOTO", curr_tok->text) == 0) {
        //printf("STATEMENT -- GOTO\n");
        curr_tok = curr_tok->next;

        if((label_tmp = getlabel(label_head, curr_tok->text)) != NULL) {
            label_tmp->visited = 1;
        } else {
            printf("GOTO ERROR: label [%s] does not exist...\n", curr_tok->text);
            exit(5); 
        }

        curr_tok = match(curr_tok, 4);
    }
    // LET ident = {expression} nl
    else if(strcmp("LET", curr_tok->text) == 0) {
        //printf("STATEMENT -- LET\n");
        curr_tok = curr_tok->next;

        if(isvariable(var_head, curr_tok->text) == 0) 
            createvar(vars, curr_tok->text, atoi(curr_tok->next->text));
        
        curr_tok = match(curr_tok, 4);
        curr_tok = match(curr_tok, 17);
        curr_tok = expression(curr_tok);
    }
    // INPUT ident nl
    else if(strcmp("INPUT", curr_tok->text) == 0) {
        //("STATEMENT -- INPUT\n");
        curr_tok = curr_tok->next;
        
        if(isvariable(var_head, curr_tok->text) == 0) {
            createvar(vars, curr_tok->text, atoi(curr_tok->next->text));
        }
    
        curr_tok = match(curr_tok, 4);
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
        curr_token = curr_token->next;
        curr_token = expression(curr_token);
    } else {
        printf("COMPARISON ERROR...\n");
        exit(1);
    }

    while(iscomparisonop(curr_token)) {
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
        curr_token = term(curr_token->next);
    }
   
    return curr_token;
}

// term ::= unary {( "/" | "*" ) unary}
struct Token *term(struct Token *curr_token) {
    //printf("TERM\n");
   
    curr_token = unary(curr_token);
    printf("[%d]\n", curr_token->type);
    while(curr_token->type == 20 || curr_token->type == 21) {
        curr_token = unary(curr_token->next);
    }
    return curr_token;
}

// unary ::= ["+" | "-"] primary
struct Token *unary(struct Token *curr_token) {
    //printf("UNARY\n");

    if(curr_token->type == 18 || curr_token->type == 19)
        curr_token = curr_token->next;
  
    curr_token = primary(curr_token);
   
    return curr_token;
}

// primary ::= number | ident
struct Token *primary(struct Token *curr_token) {
    //printf("PRIMARY -- [%s]\n", curr_token->text);

    switch(curr_token->type) {
        case 3:
            curr_token = curr_token->next;
            break;
        case 4:
            if(isvariable(*vars, curr_token->text) == 0) {
                printf("PRIMARY ERROR: variable [%s] does not exist...\n", curr_token->text);
                exit(5);
            }
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

        while(curr_token->type == 2) {
            curr_token = curr_token->next;
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

