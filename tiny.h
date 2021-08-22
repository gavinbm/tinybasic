#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXLEN 64

struct Token {
    char *text;
    int len;
    int type;
    struct Token *next;
};

struct Variable {
    char *name;
    int value;
    struct Variable *next;
};

struct Label {
    char *name;
    int visited;
    struct Label *next;
};

/* ---- globals used in tiny.c and emitter.c ---- */
extern struct Variable *var; // list of variables
extern struct Label *label;  // list of labels
extern char *final_code;     // the final code to be emitted

/* ---- lexer.c ---- */
struct Token *lex(FILE *read);
void createToken(struct Token **head, char *source, int type);
int iskeyword(char *str);
void print_tokens(struct Token *head);
void free_tokens(struct Token *head);

/* ---- parser.c ---- */
void parse(struct Token *curr_token);
void program(struct Token *curr_token);
struct Token *statement(struct Token *tokens);
struct Token *comparison(struct Token *curr_token);
struct Token *expression(struct Token *curr_token);
struct Token *term(struct Token *curr_token);
struct Token *unary(struct Token *curr_token);
struct Token *primary(struct Token *curr_token);
struct Token *nl(struct Token *curr_token);
struct Token *match(struct Token *token, int type);

int isstring(struct Token *curr_token);
int iscomparisonop(struct Token *curr_token);
int islabel(struct Label *labels, char *name);
int isvariable(struct Variable *vars, char *name);

void createlabel(struct Label **labels, char *name);
void createvar(struct Variable **vars, char *name, int value);
struct Label *getlabel(struct Label *labels, char *name);
struct Variable *getvar(struct Variable *vars, char *name);

/* ---- emitter.c ---- */
int write_file(char *filename, char *code);
void prepend_line(char *curr_code, const char *new_code);
char *append_line(char *curr_code, char *new_code);
void emit(char *curr_code, char *new_code);

/* ---- freeprint.c ----- */
void print_tokens(struct Token *head);
void free_tokens(struct Token *head);
void free_vars(struct Variable *vars);
void free_labels(struct Label *labels);
void print_vars(struct Variable *vars);
void print_labels(struct Label *labels);