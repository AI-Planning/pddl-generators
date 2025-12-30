ztravel: zenogenerator.cc
	g++ zenogenerator.cc -o ztravel

test: ztravel
	./ztravel 1 3 1 2 > $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f ztravel
