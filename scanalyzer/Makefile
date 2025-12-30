# Without this dummy target, the test target becomes the default target.
build:
	echo nothing to build

test:
	./generator.py 3 half in --seed 1 > $${PDDL_TEST_DIR}/problem.pddl
