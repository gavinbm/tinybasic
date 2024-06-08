# Teeny Tiny Basic (OLD AND DEPRECATED)
A C implementation of the Tiny BASIC compiler found in [this article](http://web.eecs.utk.edu/~azh/blog/teenytinycompiler1.html) and [this github repo](https://github.com/AZHenley/teenytinycompiler) by Dr. Austin Henley. I did pretty well in Automata last semester and was excited to take Compiler Design this Fall, but my
school isn't offering it so I'm putting on my big boy pants and teaching myself how to write a compiler (with no memory leaks to boot!).

# Building and Using
|          Step            |                  Commands to run                           |
|--------------------------|------------------------------------------------------------|
| clone the repo           | git clone https://github.com/gavinbm/tinybasic.git         |
| cd in the repo directory | cd tinybasic                                               |
| run the makefile         | make                                                       |
| compile some basic:      | ./tiny {path to basic file} {name of new C file}           |
| compile your C           | cc -o {exe name} {C file name}                             |
|                          |                                                            |
| Example compiler call    |./tiny tests/simple-tests/average.bas average.c             |

# Todo List and Milestones
- [x] Complete the lexer
- [x] Complete the parser
- [x] Complete the C emitter
- [x] Complete testing suite with sample programs
- [x] Support character variables
- [x] Support string variables
- [x] Add file I/O
- [ ] Write a 3 to 5 function stdlib
- [ ] Write a REPL
- [ ] Optimize the compiler via constant folding, hash tables, etc.
- [ ] Use this to land a job, retire at 50, and spend my golden years fishing on Lake Superior

# The Grammar and Features
Here is the current grammar, it will most definitely change as I add features or change current ones. The notation is as follows:
```
 ::=  Refers to a definition of a rule; the rule will be on the left side and the definition on the right
 |    Is a logical or
 ()   Is just for grouping
 []   Means zero or one of what's inside
 {}   Means zero or more of what's inside
 +    Means one or more of whatever is left of the +
```
Keywords or operators, things that are always needed as constants, are written in double quotation marks

Not every feature that you find in the grammar is implemented yet, as of right now the feature list below the grammar should be considered the final reference for what the language can do. I'm currently working on implementing arrays so any rule or part of any rule that deals with arrays should be ignored.
```
program   ::= {statement}
statement ::= "BREAK" nl
            | "PRINT" (expression | string | ident) nl
            | "IF" (comparison | ident) "THEN" nl {statement} "ENDIF" nl
            | "WHILE" comparison "REPEAT" nl {statement} "ENDWHILE" nl
            | "LABEL" ident nl
            | "GOTO" ident nl
            | {"LET"} ident "=" (expression | char) nl
            | "GET" ident "AS" (INT | CHAR | STR) nl
            | "OPEN" string "AS" ident nl
            | "CLOSE" ident nl
            | "READ" (ident | number) FROM (ident | string) INTO ident nl
            | "WRITE" (string | ident) INTO (ident | string) nl
comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+
expression ::= {(} term {( "-" | "+" ) term} {)}
term       ::= unary {( "/" | "*" ) unary}
unary      ::= ["+" | "-"] primary
primary    ::= number | char | string | ident
char       ::= '(A ... z)'
nl         ::= '\n'+
```

The language currently supports:
- Numeric variables
- Character variables
- String variables (assignment, printing, reading from file, writing to file only)
- Basic arithmetic
- Parentheses in arithmetic expressions
- File I/O
- If statements
- While loops
- Print text and numbers
- Input numbers and characters
- Labels and goto
- Single-line Comments (begin line with #)

# Sample BASIC Programs and their C counterparts
The tests directory holds a comprehensive catalogue of BASIC programs for you to demo the language and test its features. Most of the tests are taken right from Dr. Henley's repo, but some are mine and I've edited a few to match my version of the language.

This is one of the programs from simple-tests in the tests folder (average.bas) in it's pre-compiled BASIC form
```BASIC
LET a = 0
WHILE a < 1 REPEAT
    PRINT "Enter number of scores: "
    GET INT a
ENDWHILE

LET b = 0
LET s = 0
PRINT "Enter one value at a time: "
WHILE b < a REPEAT
    GET INT c
    LET s = s + c
    LET b = b + 1
ENDWHILE

PRINT "Average: "
PRINT s / a

```
And here is the same program after being compiled into C
```C
#include <stdio.h>
int main(int argc, char **argv) {
int a;
a = 0;
while(a<1) {
printf("Enter number of scores: \n");
if(scanf("%d", &a) == 0) {
a = 0;
scanf("%*s");
}

}
int b;
b = 0;
int s;
s = 0;
printf("Enter one value at a time: \n");
while(b<a) {
int c;
if(scanf("%d", &c) == 0) {
c = 0;
scanf("%*s");
}
s = s+c;
b = b+1;

}
printf("Average: \n");
printf("%d\n", (int)(s/a));
return 0;
}
```

# Things This Project Taught Me
This is by far my favorite project I've done in my computer science career, and I intend to develop it until I can say
it's deviated enough from Dr. Henley's grammar and design that I can call it mine. There's no doubt ways to make this program
simpler and take fewer lines of code, but I've learned a ton about C programming and the application of automata theory
so I'm not too concerned with reducing the number of lines of code I took to do things just yet. Overall if you're a 
programmer or computer scientist, you HAVE to try writing a compiler or interpreter at some point, it's taught me more than
any other project I've done.

Catching Memory Errors and Leaks
 - Always compile with the -ggdb flag when trying to test your programs, it will make valgrind show you what line
   memory errors occur on making it MUCH easier than deciphering the standard hex value memory addresses.
 - Memory errors in valgrind are much more problematic than I used to think. Many of my peers even are of the
   the mindset "if there's no leaks and it runs on my machine it's good" and that's far more harmful than it seems.
   Not only do they open up potential for crashing and undefined behavior, but they also create security flaws and
   reduce the portability of your code as machines with different processors/memory capacity may hit critical errors
   where your development machine doesn't
 - C is a higher-level language; it's also the lowest of all portable general purpose languages. You need to code with
   the machine in mind when writing C as you're able to directly interact with the memory and hardware in a way that
   may seem mysterious/abstracted but in reality is still a very low-level operation. Take time to make precise allocations
   where possible and if you must dynamically allocate, make sure to not lose your pointers and read the valgrind output
   very carefully to find read/write and leak errors.

Application of Automata Theory
 - My favorite part; the theory. I've become something a theory nut in my last few semesters of college (shout-out to Dr. Carberry 
   and Dr. Silber). All a compiler really is is an implementation of a grammar, usually it's a context-free grammar as most programming
   languages are context-free languages. Being able to break a grammar into functions or methods and see how those theoretical concepts
   get applied has no doubt improved my ability to write code that doesn't look awful.
 - We can also see very clearly how reading the grammar (much easier than reading the code) would directly translate to being able to
   understand the code since it's almost a 1:1 translation of production rules to functions. Even if we didn't follow that model of
   functional decomposition, we'd still be able to see how each production rule gets applied in our code, allowing us to see not just
   the code itself but the entire design philosophy behind it.
