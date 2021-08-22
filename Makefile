all: lexer.o parser.o emitter.o tiny.o freeprint.o
	cc -o tiny tiny.o lexer.o parser.o emitter.o freeprint.o -ggdb

tiny.o: tiny.c freeprint.c tiny.h
	cc -c freeprint.c tiny.c -ggdb

emitter.o: emitter.c tiny.h
	cc -c emitter.c -ggdb

parser.o: parser.c freeprint.c tiny.h
	cc -c freeprint.c parser.c -ggdb

lexer.o: lexer.c freeprint.c tiny.h
	cc -c freeprint.c lexer.c -ggdb

freeprint.o: freeprint.c tiny.h
	cc -c freeprint.c -ggdb

clean:
	rm *.o tiny