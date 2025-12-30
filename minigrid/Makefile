# Without this dummy target, the test target becomes the default target.
build:
	echo nothing to build

test:
	./mini_grid.py floorplans/4room2.fpl 1 > /dev/null 2>&1 && cp grid_4room2_fpl_s1_seed0_n0.pddl $${PDDL_TEST_DIR}/problem.pddl
	rm grid_4room2_fpl_s1_seed0_n0.pddl
