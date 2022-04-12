A C implementation of the Tiny BASIC compiler found in [this article](http://web.eecs.utk.edu/~azh/blog/teenytinycompiler1.html) and [this github repo](https://github.com/AZHenley/teenytinycompiler) by Dr. Austin Henley. This is a rewrite of the compiler I wrote previously. The old one can be found in the old directory in this repo, it's a hot mess and is way more inefficient than it has any right being. This version should be better :)

# Goals
I'm writing an interpreter and a compiler for this language since I've only written very simple interpreters and need the practice. The interpreter is intrprt.c and the compiler is cmplr.c

The interpreter is just supposed to be an interpreter version of the original compiler; instead of spitting out C like the original did it'll just execute the code statement by statement as it reads the input file.

The compiler is a bit more of a mixed bag. I've been using it as a sandbox to figure out how to implement abstract syntax trees and I've gotten it to parse just fine but the final code generation is another monster. My approach to the parser and code generation was based upon [this compiler](https://gist.github.com/KartikTalwar/3095780#file-tiny-c) which I've found to be both incredibly interesting and confusing.

The goals are just learning as much as I can, so in the next sections about the techincal details I'm just going to go over what's new to me here.Hopefully I learn enough to make a useable language of my own :D

# The interpreter
The Grammar
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

The lexer

The lexer is a lexer, really nothing special at this point I've written tons of these and a taken a bunch of different approaches. This one just uses a global pointer to keep track of the current position in the code, a global character buffer to store the current token, and a global integer to keep track of the current token type.
```C
int type = -100;
char tok[128], *pos;
```
The above code is from lines 39 and 40 of intrprt.c. type is the global type integer, tok is the current token, and pos is where we are in the input code. The next() function just grabs the next token and updates those global variables.

The parser

Yet another recursive descent parser, these things are so cool because of how elegantly simple they can be. Keyword being "CAN" the old compiler for this language was a massive pain to add features to. Do you know how long it took me to get that file I/O feature working? Of course you don't, you didn't write it, but trust me it was a longer than it should've been.

What's new to me here is I'm actually evaluating expressions instead of just copying them into into the output code. If I kept this as a compiler I'd have constant folding going on but since it's an interpreter it's supposed to do all of this anyway. But regardless I guess I've technically implemented constant folding or at least a more comprehensive form of expression parsing than what I've done in the past.

The only other big thing happening here is the execution of our statements. We're not allocating a tree explicitly in the interpreter so we execute as we parse instead of parsing the code into a tree and then walking/executing the tree.