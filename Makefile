tests = average average.c fib fib.c minmax minmax.c vector vector.c myname myname.c

all: build

build: lexer.o parser.o emitter.o tiny.o util.o
	cc -o tiny tiny.o lexer.o parser.o emitter.o util.o -ggdb

test-st: lexer.o parser.o emitter.o tiny.o util.o
	cc -o tiny tiny.o lexer.o parser.o emitter.o util.o -ggdb
	./tiny tests/simple-tests/average.bas average.c
	./tiny tests/simple-tests/fibonacci.bas fib.c
	./tiny tests/simple-tests/minmax.bas minmax.c
	./tiny tests/simple-tests/vector.bas vector.c
	./tiny tests/simple-tests/myname.bas myname.c
	cc -o average average.c
	cc -o fib fib.c
	cc -o vector vector.c
	cc -o minmax minmax.c
	cc -o myname myname.c

test-ol: lexer.o parser.o emitter.o tiny.o util.o
	cc -o tiny tiny.o lexer.o parser.o emitter.o util.o -ggdb
	./tiny tests/optional-let/average.bas average.c
	./tiny tests/optional-let/fibonacci.bas fib.c
	./tiny tests/optional-let/minmax.bas minmax.c
	./tiny tests/optional-let/vector.bas vector.c
	cc -o average average.c
	cc -o fib fib.c
	cc -o vector vector.c
	cc -o minmax minmax.c

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

clean-full:
	rm *.o tiny $(tests)
	