all: 2pddl-typed 2pddl-untyped

2pddl-typed: 2pddl-typed.c
	$(CC) 2pddl-typed.c -o 2pddl-typed

2pddl-untyped: 2pddl-untyped.c
	$(CC) 2pddl-untyped.c -o 2pddl-untyped

test: 2pddl-typed
	../bwstates.1/bwstates -s 2 -n 5 -r 1 > $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f 2pddl-typed 2pddl-untyped
