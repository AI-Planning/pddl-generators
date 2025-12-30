# Without this dummy target, the test target becomes the default target.
build:
	echo nothing to build

test:
	uv run generate.py --grid_size_splits=2 --max_nr_packages=1 --nr_instances_per_setup=1 > /dev/null && cp output/train/instance_s-2_p-1_v-0.pddl $${PDDL_TEST_DIR}/problem.pddl
