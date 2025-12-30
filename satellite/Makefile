satgen: satgen.cc
	g++ satgen.cc -o satgen

test: satgen
	./satgen 1 1 3 3 7 3 > $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f satgen
