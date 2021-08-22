#include "tiny.h"

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