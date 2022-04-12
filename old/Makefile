tests = shell shell.c readfile readfile.c average average.c fib fib.c minmax minmax.c vector vector.c myname myname.c hellofile hellofile.c

all: build

build: lexer.o parser.o emitter.o tiny.o util.o
	cc -o tiny tiny.o lexer.o parser.o emitter.o util.o -ggdb

test: lexer.o parser.o emitter.o tiny.o util.o
	cc -o tiny tiny.o lexer.o parser.o emitter.o util.o -ggdb
	./tiny tests/average.bas average.c
	./tiny tests/fibonacci.bas fib.c
	./tiny tests/minmax.bas minmax.c
	./tiny tests/vector.bas vector.c
	./tiny tests/myname.bas myname.c
	./tiny tests/hellofile.bas hellofile.c
	./tiny tests/shell.bas shell.c
	./tiny tests/readfile.bas readfile.c
	cc -o average average.c
	cc -o fib fib.c
	cc -o vector vector.c
	cc -o minmax minmax.c
	cc -o myname myname.c
	cc -o hellofile hellofile.c
	cc -o shell shell.c
	cc -o readfile readfile.c

tiny.o: tiny.c util.c tiny.h
	cc -c util.c tiny.c -ggdb

emitter.o: emitter.c tiny.h
	cc -c emitter.c -ggdb

parser.o: parser.c util.c tiny.h
	cc -c util.c parser.c -ggdb

lexer.o: lexer.c util.c tiny.h
	cc -c util.c lexer.c -ggdb

freeprint.o: util.c tiny.h
	cc -c util.c -ggdb

clean-build:
	rm *.o tiny

clean-tests:
	rm $(tests)

clean:
	rm *.o tiny $(tests)
	