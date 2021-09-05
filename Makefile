tests = average average.c fib fib.c minmax minmax.c vector vector.c

all: build

build: lexer.o parser.o emitter.o tiny.o freeprint.o
	cc -o tiny tiny.o lexer.o parser.o emitter.o freeprint.o

test-st: lexer.o parser.o emitter.o tiny.o freeprint.o
	cc -o tiny tiny.o lexer.o parser.o emitter.o freeprint.o -ggdb
	./tiny tests/simple-tests/average.bas average.c
	./tiny tests/simple-tests/fibonacci.bas fib.c
	./tiny tests/simple-tests/minmax.bas minmax.c
	./tiny tests/simple-tests/vector.bas vector.c
	cc -o average average.c
	cc -o fib fib.c
	cc -o vector vector.c
	cc -o minmax minmax.c

test-ol: lexer.o parser.o emitter.o tiny.o freeprint.o
	cc -o tiny tiny.o lexer.o parser.o emitter.o freeprint.o -ggdb
	./tiny tests/optional-let/average.bas average.c
	./tiny tests/optional-let/fibonacci.bas fib.c
	./tiny tests/optional-let/minmax.bas minmax.c
	./tiny tests/optional-let/vector.bas vector.c
	cc -o average average.c
	cc -o fib fib.c
	cc -o vector vector.c
	cc -o minmax minmax.c

tiny.o: tiny.c freeprint.c tiny.h
	cc -c freeprint.c tiny.c

emitter.o: emitter.c tiny.h
	cc -c emitter.c

parser.o: parser.c freeprint.c tiny.h
	cc -c freeprint.c parser.c

lexer.o: lexer.c freeprint.c tiny.h
	cc -c freeprint.c lexer.c

freeprint.o: freeprint.c tiny.h
	cc -c freeprint.c

clean-build:
	rm *.o tiny

clean-tests:
	rm $(tests)

clean-full:
	rm *.o tiny $(tests)