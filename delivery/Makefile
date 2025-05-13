# Without this dummy target, the test target becomes the default target.
build:
	echo nothing to build

test:
	uv run generate.py --grid_size_splits=5,7 --max_nr_packages=3 --nr_instances_per_setup=1
