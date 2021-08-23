# Teeny Tiny Basic
A (poorly executed) C implementation of the Tiny BASIC compiler found in [this article](http://web.eecs.utk.edu/~azh/blog/teenytinycompiler1.html) by Dr. Austin Henley
and in [this github repo](https://github.com/AZHenley/teenytinycompiler). I did pretty well in Automata last semester and was excited to take Compiler Design this Fall, but my
school isn't offering it so I'm putting on my big boy pants and teaching myself how to write a compiler (with no memory leaks to boot!). The final goal is to compile to arm32 executables that can be run on a raspberry pi.

# How to Use
1) clone the repo via git clone
2) run make
3) write some teeny tiny basic into a .bas file
4) then to compile into C run "./tiny {path to basic file} {name of c file to produce}
5) compile the C file and enjoy

# Todo List and Milestones
- [x] Complete the lexer
- [x] Complete the parser
- [x] Complete the C emitter
- [ ] Complete the ARM emitter
- [ ] Complete testing suite with sample programs
- [ ] Write the REPL
- [ ] Get it added to Raspbian as an "educational" tool (no idea if this is even possible)

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
