# Without this dummy target, the test target becomes the default target.
build:
	echo nothing to build

test:
	./generate.py nets/d31.pnml --seed 1 --noise 5 --domain $${PDDL_TEST_DIR}/domain.pddl --problem $${PDDL_TEST_DIR}/problem.pddl
