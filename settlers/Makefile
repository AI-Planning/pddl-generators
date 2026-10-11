# Without this dummy target, the test target becomes the default target.
build:
	echo nothing to build

# Requires numpy and scipy.
test:
	./generator.py 1 3 2 0 2 5 10 --domain $${PDDL_TEST_DIR}/domain.pddl --problem $${PDDL_TEST_DIR}/p01.pddl
	./generator.py 2 5 5 1 4 5 10 -c 1.5 --ci 5 -v 1.5 --vi 3 --domain $${PDDL_TEST_DIR}/domain.pddl --problem $${PDDL_TEST_DIR}/p02.pddl
