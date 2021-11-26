#!/bin/sh



####### FLAGS

TYPE	=
ADDONS	=

CC      = gcc

CFLAGS	= -O3 -Wall -g -ansi $(TYPE) $(ADDONS)
# -g -pg

LIBS    = -lm


SOURCES 	= schedule.c

OBJECTS 	= $(SOURCES:.c=.o)

####### Implicit rules

.SUFFIXES:

.SUFFIXES: .c .o

.c.o:; $(CC) -c $(CFLAGS) $<

####### Build rules


generator: $(OBJECTS)
	$(CC) -o schedule $(OBJECTS) $(CFLAGS) $(LIBS)

# misc
clean:
	rm -f *.o *.bak *~ *% core *_pure_p9_c0_400.o.warnings \
        \#*\# $(RES_PARSER_SRC) $(PDDL_PARSER_SRC)

veryclean: clean
	rm -f schedule H* J* K* L* O* graph.* *.symbex gmon.out \
	$(PDDL_PARSER_SRC) \
	lex.fct_pddl.c lex.ops_pddl.c lex.probname.c \
	*.output

lint:
	lclint -booltype Bool $(SOURCES) 2> output.lint

test: generator
	./schedule -p 1 -s 0 -c 2 -w 2 -o 2 -Q 80 -W 50 -E 80 -R 50 -T 80 -Y 50 -r 5

# DO NOT DELETE
