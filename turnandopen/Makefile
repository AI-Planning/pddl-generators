#!/bin/sh



####### FLAGS

TYPE	=
ADDONS	=

CC      = gcc

CFLAGS	= -O3 -Wall -g $(TYPE) $(ADDONS)
# -g -pg

LIBS    = -lm


SOURCES 	= turnandopen.c

OBJECTS 	= $(SOURCES:.c=.o)

####### Implicit rules

.SUFFIXES:

.SUFFIXES: .c .o

.c.o:; $(CC) -c $(CFLAGS) $<

####### Build rules


turnandopen: $(OBJECTS)
	$(CC) -o turnandopen $(OBJECTS) $(CFLAGS) $(LIBS)

test: turnandopen
	./turnandopen -n 1 -r 2 -o 2 -s 1 > $${PDDL_TEST_DIR}/problem.pddl

# misc
clean:
	rm -f *.o *.bak *~ *% core *_pure_p9_c0_400.o.warnings \
        \#*\# $(RES_PARSER_SRC) $(PDDL_PARSER_SRC)

veryclean: clean
	rm -f turnandopen H* J* K* L* O* graph.* *.symbex gmon.out \
	$(PDDL_PARSER_SRC) \
	lex.fct_pddl.c lex.ops_pddl.c lex.probname.c \
	*.output

lint:
	lclint -booltype Bool $(SOURCES) 2> output.lint

# DO NOT DELETE
