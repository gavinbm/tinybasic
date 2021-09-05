#include "tiny.h"

/*
    Utility functions that don't really fit anywhere else particularly well.
    The first grouping of functions tells use what a given token is, returning an
    integer for boolean-esque functionality in if or switch statements.
*/


/* -------- Type-Checking and Create functions -------- */

/*
    This will tell us if the given token is a comparison operator. I'd love to use
    the "case low ... high:"" syntax here cause it would save lines, but that's not 
    standard ANSI C, it's only a feature of the GNU compiler and that (albeit slightly)
    reduces the portability of our emitted C code and thus reduces the portability of our
    BASIC and that's no bueno.
*/
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

/*
    Will tell use if the given identifier is a label. It does this by searching
    through our list of previsously declared labels.
*/
int islabel(struct Label *labels, char *name) {
    struct Label *tmp = labels;
    
    while(tmp != NULL) {
        if(strcmp(tmp->name, name) == 0)
            return 1;
        
        tmp = tmp->next;
    }

    return 0;
}

/*
    Same as islabel but for variables, I literally copy/pasted the code then changed
    every instance of the word "label" to "vars"
*/
int isvariable(struct Variable *vars, char *name) {
    struct Variable *tmp = vars;
    
    while(tmp != NULL) {
        if(strcmp(tmp->name, name) == 0)
            return 1;
        
        tmp = tmp->next;
    }

    return 0;
}

/*
    creates a new label and adds it to our list of labels.
*/
void createlabel(struct Label **labels, char *name) {
    struct Label **tmp = labels, *add;
    int name_len = strlen(name);

    add = malloc(sizeof(struct Label));
    add->name = malloc(name_len * sizeof(char));
    memcpy(add->name, name, name_len);
    add->visited = 0;
    add->next = NULL;

    while(*tmp)
        tmp = &(*tmp)->next;
    
    add->next = *tmp;
    *tmp = add;
}

/*
    creates a variable and adds it to our list of variables
    The type arg is a flag to let us know if this is a char or an int
    if type is 0 it's an int
    if type is 1 it's a char
*/
void createvar(struct Variable **vars, char *name, int type) {
    struct Variable **tmp = vars, *add;
    int name_len = strlen(name);

    add = malloc(sizeof(struct Variable));
    add->name = malloc(name_len + 1 * sizeof(char));
    memcpy(add->name, name, name_len);
    add->type = type;
    add->next = NULL;

    while(*tmp)
        tmp = &(*tmp)->next;
    
    add->next = *tmp;
    *tmp = add;
}

/*
    finds the label in our list and returns it so we can do stuff to it
    (I'm not fully sure of what that "stuff" is or what it's purpose is but
    I can always remove this if I decide it's superfluous)
*/
struct Label *getlabel(struct Label *labels, char *name) {
    struct Label *tmp = labels;

    while(tmp != NULL) {
        if(strcmp(tmp->name, name) == 0)
            return tmp;
        
        tmp = tmp->next;
    }

    return NULL;
}

/* -------- Free and Print functions -------- */

/*
function to print all current tokens, takes a reference to the head of a
 list of tokens and iteraotes over the list, printing the details of each one 
*/
void print_tokens(struct Token *head) {

    printf("\nPrinting Tokens...\n");
    struct Token *tmp = head;
    while(tmp != NULL) {
        printf("[%s]--[%d]--[%d]\n", tmp->text, tmp->len, tmp->type);
        tmp = tmp->next;
    }
    printf("NULL\n");
}

/*
frees all tokens starting from the head reference given, iterates over the list
freeing each token individually then gives a count at the end;
*/
void free_tokens(struct Token *head) {
    struct Token *tmp = head, *tmp2;

    int i = 0;
    while(tmp != NULL) {
        free(tmp->text);
        tmp2 = tmp;
        tmp = tmp->next;
        free(tmp2);
        i++;
    }
    printf("%d tokens free'd...\n", i);
}

/* 
the rest of these functions do what the previous ones do, just for variables
and labels respectively.
*/

void free_vars(struct Variable *vars) {
    struct Variable *tmp = vars, *tmp2;
    
    int i = 0;
    while(tmp != NULL) {
        free(tmp->name);
        tmp2 = tmp;
        tmp = tmp->next;
        free(tmp2);
        i++;
    }

    printf("%d variables free'd...\n", i);
}

void free_labels(struct Label *labels) {
    struct Label *tmp = labels, *tmp2;
    
    int i = 0;
    while(tmp != NULL) {
        free(tmp->name);
        tmp2 = tmp;
        tmp = tmp->next;
        free(tmp2);
        i++;
    }

    printf("%d labels free'd...\n", i);
}

void print_vars(struct Variable *vars) {
    printf("Printing variables...\n");
    struct Variable *tmp = vars;
    
    int i = 1;
    while(tmp != NULL) {
        printf("[%s] -- [%d]\n", tmp->name, i);
        tmp = tmp->next;
        i++;
    }
}

void print_labels(struct Label *labels) {
    printf("Printing Labels...\n");
    struct Label *tmp = labels;
    
    int i = 1;
    while(tmp != NULL) {
        printf("[%s] -- [%d]\n", tmp->name, i);
        tmp = tmp->next;
        i++;
    }
}