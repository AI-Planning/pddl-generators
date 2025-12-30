GENERATOR=pathways

generator:
	gcc main.c -lm -o $(GENERATOR)

test: generator
	./wrapper.py --seed 1 --reactions 12 --goals 1 --initial-substances 3 $${PDDL_TEST_DIR}/domain.pddl $${PDDL_TEST_DIR}/p01.pddl

clean:
	rm -f $(GENERATOR)
