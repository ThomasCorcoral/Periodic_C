all : rep periodic now when

rep : 
	mkdir -p build

periodic : libperiod.so rep
	gcc tmp/message.o tmp/error.o src/periodic.c -o build/periodic
	gcc tmp/message.o tmp/error.o  src/period.c -o build/period
	rm -r tmp

tests : libperiod.so rep
	gcc tmp/message.o tests/test.c -o build/test
	gcc tmp/message.o tests/testex2.c -o build/testex2
	gcc tmp/message.o tests/testex2_4.c -o build/testex2_4
	gcc tmp/message.o tests/testex3.c -o build/testex3
	rm -r tmp

libperiod.so : src/message.h src/message.c src/error.h src/error.c
	mkdir tmp
	gcc src/message.c -c -o tmp/message.o
	gcc src/error.c -c -o tmp/error.o

now : rep
	gcc src/now.c -o build/now

when : rep
	gcc src/when.c -o build/when

clean :
	rm -rf build