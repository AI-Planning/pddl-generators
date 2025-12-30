main: trucks.c
	gcc trucks.c -lm -o trucks

adl2strips/ff:
	$(MAKE) -C adl2strips

test: main adl2strips/ff
	./trucks-strips.sh $${PDDL_TEST_DIR}/domain.pddl $${PDDL_TEST_DIR}/p01.pddl -seed 1 -t 1 -l 3 -p 3 -a 2 -n 1

clean:
	rm -f trucks
	$(MAKE) -C adl2strips clean
