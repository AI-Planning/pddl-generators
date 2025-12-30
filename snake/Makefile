# Without this dummy target, the test target becomes the default target.
build:
	echo nothing to build

test:
	./generate.py boards/no-spawn-6x6.txt 1 3 40% 1 pddl > $${PDDL_TEST_DIR}/problem.pddl
