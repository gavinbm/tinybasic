# Teeny Tiny Basic
A C implementation of the Tiny BASIC compiler found in [this article](http://web.eecs.utk.edu/~azh/blog/teenytinycompiler1.html) by Dr. Austin Henley
and in [this github repo](https://github.com/AZHenley/teenytinycompiler). I did pretty well in Automata last semester and was excited to take Compiler Design this Fall, but my
school isn't offering it so I'm putting on my big boy pants and teaching myself how to write a compiler (with no memory leaks to boot!). The final goal is to compile to arm32 executables that can be run on a raspberry pi.

# Building and Using
|          Step            |                  Commands to run                           |
|--------------------------|------------------------------------------------------------|
| clone the repo           | git clone https://github.com/gavinbm/tinybasic.git         |
| cd in the repo directory | cd tinybasic                                               |
| run the makefile         | make                                                       |
| compile some basic:      | ./tiny {path to basic file} {name of new C file}           |
| compile your C           | cc -o {exe name} {C file name}                             |

An example use:                ./tiny tests/average.bas average.c

# Todo List and Milestones
- [x] Complete the lexer
- [x] Complete the parser
- [x] Complete the C emitter
- [ ] Complete the ARM emitter
- [x] Complete testing suite with sample programs
- [ ] Add support for file reading/writing
- [ ] Add support for pointers 
- [ ] Write a REPL
- [ ] Use this to land a job, retire at 50, and spend my golden years fishing on Lake Superior

# The Grammar and Features
Here is the current grammar, I may change this later to make it more feature rich or to just be stylistically different (i.e. reading files, only goto statements for looping to make it more assembly-esqe, etc.). As of right now it's just the grammar from Dr. Henley's article. I'd also like to write a REPL for this so it behaves a bit more like some BASICs or FORTHs but I'm getting a bit ahead of myself with that.
```
program ::= {statement}
statement ::= "PRINT" (expression | string) nl
            | "IF" comparison "THEN" nl {statement} "ENDIF" nl
            | "WHILE" comparison "REPEAT" nl {statement} "ENDWHILE" nl
            | "LABEL" ident nl
            | "GOTO" ident nl
            | "LET" ident "=" expression nl
            | "INPUT" ident nl
            | ident "=" expression nl
comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+
expression ::= term {( "-" | "+" ) term}
term ::= unary {( "/" | "*" ) unary}
unary ::= ["+" | "-"] primary
primary ::= number | ident
nl ::= '\n'+
```

The language currently supports:
- Numerical variables
- Basic arithmetic
- If statements
- While loops
- Print text and numbers
- Input numbers
- Labels and goto

# Sample BASIC Programs and their C counterparts           
This is one of the programs from the tests folder (average.bas) in it's pre-compiled BASIC form
```BASIC
LET a = 0
WHILE a < 1 REPEAT
    PRINT "Enter number of scores: "
    INPUT a
ENDWHILE

LET b = 0
LET s = 0
PRINT "Enter one value at a time: "
WHILE b < a REPEAT
    INPUT c
    LET s = s + c
    LET b = b + 1
ENDWHILE

PRINT "Average: "
PRINT s / a
```
And here is the same program after being compiled into C
```C
#include <stdio.h>
int main(void) {
float a;
a = 0;
while(a<1) {
printf("Enter number of scores: \n");
if(scanf("%f", &a) == 0) {
a = 0;
scanf("%*s");
}

}
float b;
b = 0;
float s;
s = 0;
printf("Enter one value at a time: \n");
while(b<a) {
float c;
if(scanf("%f", &c) == 0) {
c = 0;
scanf("%*s");
}
s = s+c;
b = b+1;

}
printf("Average: \n");
printf("%.2f\n", (float)(s/a));
return 0;
}
```
