dlgen: generator.cc
	g++ generator.cc -o dlgen

test: dlgen
	./dlgen 1 5 2 2 2 > $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f dlgen
