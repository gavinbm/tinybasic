#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int vars[26] = {0};

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
enum {PROG, SPR, SIF, SWL, SPC, SCL, SLB, SGO, SLT, SIN,
      COMP, EXPR, TERM, UNRY, NUMR, VARB, NWLN};

typedef struct node {
    int type;
    int value;
    struct node *o1, *o2, *o3, *o4;
} node;

node *new(int t) {
    node *res = malloc(sizeof(node));
    res->type = t;
    return res;
}

void match(int check) { // utility to check token types

    if(type != check) {
        printf("Match Error: got [%d] expected [%d]\n", type, check);
        exit(10);
    } else {
        next();
    }
}

node *nl() { // nl ::= '\n'+
    //puts("NEWLINE");
    node *res = new(NWLN);
    match(NLN);
    while(type == NLN)
        next();
    
    return res;
}

node *primary() { // primary ::= number | ident
    //puts("PRIMARY");
    node *res;

    if(type == IDENT || type == NUM) {
        if(type == IDENT) {
            res = new(VARB);
            res->value = vars[*tok - 97];
        } else {
            res = new(NUMR);
            res->value = atoi(tok);
        }

        next();
    } else
        exit(20);

    return res;
}

node *unary() { // unary ::= ["+" | "-"] primary
    //puts("UNARY");
    node *res = new(UNRY);
    
    if(type == PLS || type == MIN) {
        if(type == PLS)
            res->o1 = new(PLS);
        else
            res->o1 = new(MIN); 
        
        next();
    }
    
    res->o2 = primary();
    return res;
}

node *term() { // term ::= unary {( "/" | "*" ) unary}
    //puts("TERM");
    node *res = new(TERM);
    
    res->o1 = unary();

    while(type == DIV || type == MUL) {
        next();
        res->o2 = unary();
    }

    return res;
}

node *expr() { // expr ::= term {( "-" | "+" ) term}
    //puts("EXPR");
    node *res = new(EXPR);
    
    res->o1 = term();

    while(type == PLS || type == MIN) {
        next();
        res->o2 = term();
    }

    return res;
}

node *comp() { // comp ::= expr (("=" | "!" | ">" | "<") expr)+
    //puts("COMP");    
    node *res = new(COMP);
    res->o1 = expr();

    if(type == EQL || type == NOT || type == LES || type == GRT) {
           next();
           res->o2 = expr();
    } else {
           printf("Comp Error: Expected op got [%s] -- [%d]\n", tok, type);
           exit(11);
       }
    
    while(type == EQL || type == NOT || type == LES || type == GRT) {
           next();
           res->o3 = expr();
       }

       return res;
}

node *stmnt() {
    node *res;

    switch(type) {
        case PRINT: // "PRINT" (expression | string) nl
            //puts("STATEMENT -- PRINT");
            res = new(SPR);
            next();
            
            if(type == STR) {
                next();
            } else { 
                res->o1 = expr();
            }

            break;
        case IF: // "IF" comparison "THEN" nl {statement} "END" nl
            //puts("STATEMENT -- IF");
            res = new(SIF);
            next(); 
            res->o1 = comp(); 
            match(THEN); 
            res->o2 = nl();

            while(type != END)
                res->o3 = stmnt();

            match(END);
            break;
        case WHILE: // "WHILE" comparison "DO" nl {statement} "END" nl
            //puts("STATEMENT -- WHILE");
            res = new(SWL);

            next(); 
            res->o1 = comp(); 
            match(DO); 
            res->o2 = nl();

            while(type != END)
                res->o3 = stmnt();
            
            match(END);
            break;
        case PROC: // "PROC" ident nl {statement} "RETURN" nl
            //puts("STATEMENT -- PROC");
            res = new(SPC);
            next(); 
            match(IDENT); 
            res->o1 = nl();

            while(type != RTRN)
                res->o2 = stmnt();

            match(RTRN);
            break;
        case CALL: // "CALL" ident nl
            //puts("STATEMENT -- CALL");
            res = new(SCL);

            next(); match(IDENT); 
            break;
        case LABEL: // "LABEL" ident nl
            //puts("STATEMENT -- LABEL");
            res = new(SLB);

            next(); match(IDENT);
            break;
        case GOTO: // "GOTO" ident nl
            //puts("STATEMENT -- GOTO");
            res = new(SGO);

            next(); match(IDENT);
            break;
        case LET: // "LET" ident "=" expression nl
            //puts("STATEMENT -- LET");
            res = new(SLT);

            next(); match(IDENT); match(EQL); 
            
            res->o1 = expr();
            break;
        case INPUT: // "INPUT" ident nl
            //puts("STATEMENT -- INPUT");
            res = new(SIN);

            next(); match(IDENT);
            break;
        default:
            printf("Invalid Token: [%s] -- [%d]\n", tok, type);
            break;
    }

    res->o4 = nl();
    return res;
}

node *prog() {

    node *root = new(PROG);

    while(type == NLN || type == SPACE)
        next();

     //while(type != EOI && type != TERMINAL)
    root->o1 = stmnt();
    
    return root;
}

/* ========= Code Gen ========== */
enum {ADD, SUB, MOV, STORE, LOAD, CMP, SVC, BR};

void gen(node *tree) {
    switch(tree->type) {
        case PROG: gen(tree->o1); break;

        case EXPR: gen(tree->o1); gen(tree->o2); break;
        case TERM: gen(tree->o1); gen(tree->o2); break;
        case UNRY: gen(tree->o1); break;
        case VARB: gen(tree->o1); break;
        case NUMR: gen(tree->o1); break;
        case NWLN: gen(tree->o1); break;

        case COMP: break;

        case SPR: break;
        case SIF: break;
        case SWL: break;
        case SPC: break;
        case SCL: break;
        case SLB: break;
        case SGO: break;
        case SLT: break;
        case SIN: break;
        default:
            break;
    }
}

/* ===========  Main  ========== */
int main(int argc, char **argv) {
    char *input;
    if(argc > 1)
        input = readfile(argv[1]);
    else
        return 0;

    if(input) {
        pos = input;

        next();
        prog();

        free(input);
    }

    return 0;
}
