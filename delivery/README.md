A generator for the Delivery domain. 

How to use:
- Install the packages in requirements.txt
- Run like the following:
```bash
python generate.py --grid_size_splits=5,7,9 --max_nr_packages=3 --nr_instances_per_setup=5
```

The above example will generate training instances with grid sizes of 2-5, validation instances with grid sizes of 6-7, and test instances with grid sizes of 8-9. For each grid size there will be 5 instances with 1-3 packages, for a total of 5 * 3 = 15 instances per grid size. The generated instances will be saved in the `output` directory. Currently, only square grids are supported.

Description of the parameters:
- `grid_size_splits`: Specifies the maximum size of the grid in each of the dataset splits. The grid size splits for the delivery domain. It should be a comma-separated list of integers.
- `max_nr_packages`: The maximum number of packages to be delivered. The generator will create instances with [1, max_nr_packages] packages, for each integer in the interval.
- `nr_instances_per_setup`: The number of instances to be generated for each setup. 

