#!/bin/sh



####### FLAGS

TYPE	=
ADDONS	=

CC      = gcc

CFLAGS	= -O3 -Wall -g $(TYPE) $(ADDONS)
# -g -pg

LIBS    = -lm


SOURCES 	= freecell.c

OBJECTS 	= $(SOURCES:.c=.o)

####### Implicit rules

.SUFFIXES:

.SUFFIXES: .c .o

.c.o:; $(CC) -c $(CFLAGS) $<

####### Build rules


freecell: $(OBJECTS)
	$(CC) -o freecell $(OBJECTS) $(CFLAGS) $(LIBS)

# misc
clean:
	rm -f *.o *.bak *~ *% core *_pure_p9_c0_400.o.warnings \
        \#*\# $(RES_PARSER_SRC) $(PDDL_PARSER_SRC)

veryclean: clean
	rm -f freecell H* J* K* L* O* graph.* *.symbex gmon.out \
	$(PDDL_PARSER_SRC) \
	lex.fct_pddl.c lex.ops_pddl.c lex.probname.c \
	*.output

lint:
	lclint -booltype Bool $(SOURCES) 2> output.lint

test: freecell
	./freecell -f 5 -c 1 -s 4 -0 2 -1 2 -2 2 -3 2 -i 1 -r 0

# DO NOT DELETE
