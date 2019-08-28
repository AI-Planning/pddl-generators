####### Flags

TYPE =
ADDONS =
CC = gcc
CFLAGS = -O3 -Wall -g -ansi $(TYPE) $(ADDONS)
LIBS = -lm
SOURCES = assembly.c
OBJECTS = $(SOURCES:.c=.o)

####### Implicit rules

.SUFFIXES:
.SUFFIXES: .c .o
.c.o:; $(CC) -c $(CFLAGS) $<

####### Build rules

build: $(OBJECTS)
	$(CC) -o assembly $(OBJECTS) $(CFLAGS) $(LIBS)

# misc
clean:
	rm -f $(OBJECTS)

distclean: clean
	rm -f assembly

test: build
	./assembly -d 5 -m 5 -h 90 -n 1 -r 5 -t 5 -a 25 -o 50

lint:
	lclint -booltype Bool $(SOURCES)
