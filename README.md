A C implementation of the Tiny BASIC compiler found in [this article](http://web.eecs.utk.edu/~azh/blog/teenytinycompiler1.html) and [this github repo](https://github.com/AZHenley/teenytinycompiler) by Dr. Austin Henley. This is a rewrite of the compiler I wrote previously. The old one can be found in the old directory in this repo, it's a hot mess and is way more inefficient than it has any right being. This version should be better :)

The goals are just learning as much as I can, so in the next sections about the techincal details I'm just going to go over what's new to me here.Hopefully I learn enough to make a useable language of my own :D

# The Grammar
```
prog      ::= {stmnt}
stmnt     ::= "PRINT" (expr | string) nl
            | "IF" comp "THEN" nl {stmnt} "END" nl
            | "WHILE" comp "DO" nl {stmnt} "END" nl
            | "PROC" ident nl {stmnt} "RETURN" nl
            | "CALL" ident nl
            | "LABEL" ident nl
            | "GOTO" ident nl
            | "LET" ident "=" expr nl
            | "INPUT" ident nl
comp      ::= expr (("=" | "!" | ">" | "<") expr)+
expr      ::= term {( "-" | "+" ) term}
term      ::= unary {( "/" | "*" ) unary}
unary     ::= ["+" | "-"] primary
primary   ::= number | ident
nl        ::= '\n'+
```
This can be found in g2.txt, it's just a copy/paste of that file for your convenience. It's only got small modifications to the original grammar by Dr. Henley like adding processes (functions) and reducing comparison operators to single character tokens. I only did the comparison op thing because that's something some older tiny basics did and I thought it was cool.

