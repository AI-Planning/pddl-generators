#!/bin/sh

####### FLAGS

TYPE	=
ADDONS	=
CC      = gcc
CFLAGS	= -O3 -Wall -g $(TYPE) $(ADDONS)
LIBS    = -lm
SOURCES = grid.c
OBJECTS = $(SOURCES:.c=.o)

####### Implicit rules

.SUFFIXES: .c .o
.c.o:; $(CC) -c $(CFLAGS) $<

####### Build rules

generator: $(OBJECTS)
	$(CC) -o grid $(OBJECTS) $(CFLAGS) $(LIBS)

clean:
	rm -f *.o *.bak *~ *% core *_pure_p9_c0_400.o.warnings \
        \#*\# $(RES_PARSER_SRC) $(PDDL_PARSER_SRC)

veryclean: clean
	rm -f grid H* J* K* L* O* graph.* *.symbex gmon.out \
	$(PDDL_PARSER_SRC) \
	lex.fct_pddl.c lex.ops_pddl.c lex.probname.c \
	*.output

lint:
	lclint -booltype Bool $(SOURCES) 2> output.lint

test:
	./generate.py 4 5
