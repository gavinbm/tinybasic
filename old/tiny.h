#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXLEN 64

/*          ---- Token Type Map ----
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
                    OPEN     = 33   MOD   = 32
                    CLOSE    = 34
                    READ     = 35
                    FROM     = 36
                    WRITE    = 37
                    INTO     = 38
                    AS       = 39
                    

    These will be assigned to each token as we lex the BASIC file
    allowing us to decipher what kind of token we're looking at when
    we parse the tokens.
*/

enum TokenType {
    EOFC, NEWLINE, IDENT, STRING, LABEL, GOTO, PRINT,
    GET, LET, IF, THEN, ENDIF, WHILE, REPEAT, ENDWHILE,
    EQ, PLUS, MINUS, MUL, DIV, EQEQ, NOTEQ, LT, LTEQ,
    GT, GTEQ, CHAR, INT, LEFTPAREN, RIGHTPAREN, MOD, OPEN, CLOSE,
    READ, FROM, WRITE, INTO, AS, STR, BREAK
};

struct Token {
    char *text;
    int len;
    int type;
    struct Token *next;
};

struct Variable {
    char *name;
    int type;
    struct Variable *next;
};

struct Label {
    char *name;
    int visited;
    struct Label *next;
};

/* ---- globals used in tiny.c and emitter.c ---- */
extern struct Variable *vars; // list of variables
extern struct Label *labels;  // list of labels
extern char *final_code;      // the final code to be emitted
extern char *header_code;     // top part of code, holds var decs
extern char *footer_code;     // bottom part, holds actual logic  

/* ---- lexer.c ---- */
struct Token *lex(FILE *read);
void createToken(struct Token **head, char *source, int type);
int iskeyword(char *str);

/* ---- parser.c ---- */
void parse(struct Token *curr_token);
void program(struct Token *curr_token);
struct Token *statement(struct Token *tokens);
struct Token *comparison(struct Token *curr_token);
struct Token *expression(struct Token *curr_token);
struct Token *term(struct Token *curr_token);
struct Token *unary(struct Token *curr_token);
struct Token *primary(struct Token *curr_token);
struct Token *character(struct Token *curr_token);
struct Token *nl(struct Token *curr_token);
struct Token *match(struct Token *token, int type);

/* ---- emitter.c ---- */
int write_file(char *filename, char *code);
char *append_line(char *curr_code, char *new_code);
char *prepend_line(char *curr, char *new);

/* ---- util.c ----- */
void declare(char *name, struct Token *peek, int type);

int iscomparisonop(struct Token *curr_token);
int islabel(struct Label *labels, char *name);
int isvariable(struct Variable *vars, char *name);

void createlabel(struct Label **labels, char *name);
void createvar(struct Variable **vars, char *name, int type);
struct Label *getlabel(struct Label *labels, char *name);
struct Variable *getvar(struct Variable *vars, char *name);

void print_tokens(struct Token *head);
void free_tokens(struct Token *head);
void free_vars(struct Variable *vars);
void free_labels(struct Label *labels);
void print_vars(struct Variable *vars);
void print_labels(struct Label *labels);