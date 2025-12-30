maintenance: maintenance.c
	gcc maintenance.c -o maintenance

test: maintenance
	./maintenance 60 180 1 3 5 100 > $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f maintenance
