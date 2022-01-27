SOURCES = main.c calc.c
HEADERS = calc.h
OBJECTS = $(SOURCES:.c=.o)
BINARYNAME = nbody

default: main

%.o: %.c $(HEADERS)
	gcc -c -g -o $@ $<

main: $(OBJECTS)
	gcc $(OBJECTS) -o $(BINARYNAME) -Wall -lm -Og

clean:
	rm -f *.o
	rm -f nbody