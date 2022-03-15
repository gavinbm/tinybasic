#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

enum { SPACE = -3, TERMINAL, EOI,
      PRINT = 0, IF, THEN, WHILE, DO, LABEL, GOTO, LET, INPUT, END,
      PROC, RTRN, CALL, EQL, NOT, LES, GRT, PLS, MIN, MUL, DIV, NLN, 
      NUM, STR, IDENT};

char *readfile(char *filename) {
    FILE *fp = fopen(filename, "r");
    char *buffer = NULL, c;
    int length = 0, i = 0;

    while((c = fgetc(fp)) != EOF)
        length++;  
    
    if(length) {
        fseek(fp, 0L, SEEK_SET);
        buffer = malloc(length + 1);
        
        while((c = fgetc(fp)) != EOF)
            buffer[i++] = c;
        
        buffer[length] = '\0';
        fclose(fp);
    } else {
        puts("couldn't read file...");
        exit(50);
    }

    return buffer;
} 

// ================= Lexer ================
int type = -100;
char tok[128], *pos;

int iskey(char *src) {
    char keys[EQL][7] = { "print", "if", "then", "while", "do", 
                          "label", "goto", "let", "input", "end", "proc", 
                          "return", "call"};
    
    for(int i = 0; i < strlen(src); i++) {
        if(src[i] >= 'A' && src[i] <= 'Z')
            src[i] = src[i] + 32;
    }

    for(int i = 0; i < EQL; i++) {
        if(strcmp(keys[i], src) == 0)
            return i;
    }

    if(isalpha(*src) && strlen(src) < 2)
        return IDENT;

    return -1;
}

void updatetok(char *ntok, int ntype, int size) {
    if(size >= 128)
        exit(100);
    memcpy(tok, ntok, size);
    tok[size] = '\0';
    type = ntype;
}

void next() {

    char *peek = pos, *sub;
    int len = 0, key;

    repeat:
        switch(*peek) {
            case ' ': case '\t': case '\r': peek++; goto repeat;
            case EOF: type = EOI; break;
            case '\0': type = TERMINAL; break;
            case '\n': updatetok(peek, NLN, 1); peek++; break;
            case '+': updatetok(peek, PLS, 1); peek++; break;
            case '-': updatetok(peek, MIN, 1); peek++; break;
            case '*': updatetok(peek, MUL, 1); peek++; break;
            case '/': updatetok(peek, DIV, 1); peek++; break;
            case '=': updatetok(peek, EQL, 1); peek++; break;
            case '!': updatetok(peek, NOT, 1); peek++; break;
            case '>': updatetok(peek, GRT, 1); peek++; break;
            case '<': updatetok(peek, LES, 1); peek++; break;
            case '\"':
                peek++;
                while(peek[len] != '\"') {
                    if(peek[len] == '\n' || peek[len] == '\t' || peek[len] == '\r' || peek[len] == '\\' || peek[len] == '%') {
                        printf("illegal char in string...\n");
                        exit(10);
                    }
                    len++;
                }

                updatetok(peek, STR, len + 1);

                peek = peek + len + 1;
                break;
            default:
                if(isalpha(*peek)) {
                    while(isalpha(peek[len]))
                        len++;
                    
                    memcpy(tok, peek, len + 1);
                    tok[len] = '\0';
                    key = iskey(tok);

                    if(key == -1) {
                        printf("invalid token [%s]...\n", sub);
                        exit(2);
                    } else {
                        type = key;
                    }
                }
                else if(*peek >= '0' && *peek <= '9') {
                    while(peek[len] >= '0' && peek[len] <= '9')
                        len++;
                    
                    if(peek[len] == '.') {
                        puts("No floats..."); exit(4);
                    }
                
                    updatetok(peek, NUM, len + 1);
                } else {
                    printf("invalid char [%c]\n", *peek);
                    exit(1);
                }
                peek = peek + len;
                break;
        }

    pos = peek;
}

/* =========== Parser ========== */
enum {PROG, SPR, SIF, SWL, SCL, SLB, SGO, SLT, SIN,
      COMP, EXPR, TERM, UNRY, PRIM, NUMR, VARB, NWLN};

typedef struct node {
    int type;
    int value;
    node *o1, *o2, *o3, *o4;
} node;

node *new(int t) {
    node *res = malloc(sizeof(node));
    res->type = t;
    return res;
}

node *match(int check) { // utility to check token types

    if(type != check) {
        printf("Match Error: got [%d] expected [%d]\n", type, check);
        exit(10);
    } else {
        next();
    }
}

node *nl() { // nl ::= '\n'+
    puts("NEWLINE");

    match(NLN);
    while(type == NLN)
        next();
}

node *primary() { // primary ::= number | ident
    puts("PRIMARY");

    if(type == IDENT || type == NUM)
        next();
    else
        exit(20);
}

node *unary() { // unary ::= ["+" | "-"] primary
    puts("UNARY");
    if(type == PLS || type == MIN)
        next();
    
    primary();
}

node *term() { // term ::= unary {( "/" | "*" ) unary}
    puts("TERM");
    unary();

    while(type == DIV || type == MUL) {
        next();
        unary();
    }
}

node *expr() { // expr ::= term {( "-" | "+" ) term}
    puts("EXPR");
    term();

    while(type == PLS || type == MIN) {
        next();
        term();
    }
}

node *comp() { // comp ::= expr (("=" | "!" | ">" | "<") expr)+
    puts("COMP");    
    expr();

    if(type == EQL || type == NOT || type == LES || type == GRT) {
           next();
           expr();
    } else {
           printf("Comp Error: Expected op got [%s] -- [%d]\n", tok, type);
           exit(11);
       }
    
    while(type == EQL || type == NOT || type == LES || type == GRT) {
           next();
           expr();
       }
}

node *stmnt() {

    switch(type) {
        case PRINT: // "PRINT" (expression | string) nl
            puts("STATEMENT -- PRINT");
            next();

            if(type == STR) {
                next();
            } else { 
                expr();
            }

            break;
        case IF: // "IF" comparison "THEN" nl {statement} "END" nl
            puts("STATEMENT -- IF");
            next(); comp(); match(THEN); nl();

            while(type != END)
                stmnt();

            match(END);
            break;
        case WHILE: // "WHILE" comparison "DO" nl {statement} "END" nl
            puts("STATEMENT -- WHILE");
            next(); comp(); match(DO); nl();

            while(type != END)
                stmnt();
            
            match(END);
            break;
        case PROC: // "PROC" ident nl {statement} "RETURN" nl
            puts("STATEMENT -- PROC");
            next(); match(IDENT); nl();

            while(type != RTRN)
                stmnt();

            match(RTRN);
            break;
        case CALL: // "CALL" ident nl
            puts("STATEMENT -- CALL");

            next(); match(IDENT); 
            break;
        case LABEL: // "LABEL" ident nl
            puts("STATEMENT -- LABEL");
            next(); match(IDENT);
            break;
        case GOTO: // "GOTO" ident nl
            puts("STATEMENT -- GOTO");
            next(); match(IDENT);
            break;
        case LET: // "LET" ident "=" expression nl
            puts("STATEMENT -- LET");
            next(); match(IDENT); match(EQL); expr();
            break;
        case INPUT: // "INPUT" ident nl
            puts("STATEMENT -- INPUT");
            next(); match(IDENT);
            break;
        default:
            printf("Invalid Token: [%s] -- [%d]\n", tok, type);
            break;
    }

    nl();
}

node *prog() {

    node *root = new(PROG);
    root->o1 = stmnt();

    while(type == NLN || type == SPACE)
        next();

    while(type != EOI && type != TERMINAL)
        stmnt();
    
    return root;
}

/* ===========  Main  ========== */
int main(int argc, char **argv) {
    char *input = readfile(argv[1]);

    if(input) {
        pos = input;

        next();
        prog();

        free(input);
    }

    return 0;
}
