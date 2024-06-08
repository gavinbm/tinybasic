tests = shell shell.c readfile readfile.c average average.c fib fib.c minmax minmax.c vector vector.c myname myname.c hellofile hellofile.c
obj_files = obj/tiny.o obj/lexer.o obj/parser.o obj/emitter.o obj/util.o

all: build

build: objects
	cc -o tiny $(obj_files) -ggdb

tests: objects
	cc -o tiny $(obj_files) -ggdb
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

objects: lexer.o parser.o emitter.o tiny.o util.o
	mkdir obj
	mv *.o obj

tiny.o: src/tiny.c src/util.c inc/tiny.h
	cc -c src/util.c src/tiny.c -ggdb

emitter.o: src/emitter.c inc/tiny.h
	cc -c src/emitter.c -ggdb

parser.o: src/parser.c src/util.c inc/tiny.h
	cc -c src/util.c src/parser.c -ggdb

lexer.o: src/lexer.c src/util.c inc/tiny.h
	cc -c src/util.c src/lexer.c -ggdb

freeprint.o: src/util.c inc/tiny.h
	cc -c src/util.c -ggdb

clean-build:
	rm -rf obj tiny

clean-tests:
	rm /$(tests)

clean:
	rm -rf obj tiny $(tests)
	