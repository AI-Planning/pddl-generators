tpp: tpp.c
	gcc tpp.c -o tpp

test: tpp
	./tpp -s 1 -m 1 -p 1 -t 1 -d 1 -l 1 $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f tpp
