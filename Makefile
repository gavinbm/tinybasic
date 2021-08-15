all: lexer.o parser.o tiny.o freeprint.o
	cc -o tiny tiny.o lexer.o parser.o freeprint.o -ggdb

tiny.o: tiny.c freeprint.c tiny.h
	cc -c freeprint.c tiny.c -ggdb

parser.o: parser.c freeprint.c tiny.h
	cc -c freeprint.c parser.c -ggdb

lexer.o: lexer.c freeprint.c tiny.h
	cc -c freeprint.c lexer.c -ggdb

freeprint.o: freeprint.c tiny.h
	cc -c freeprint.c -ggdb

clean:
	rm *.o tiny