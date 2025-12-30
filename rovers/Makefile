rovgen: rovgen.cc
	g++ rovgen.cc -o rovgen

test: rovgen
	./rovgen 1 1 4 2 1 3 > $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f rovgen
