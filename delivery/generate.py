import argparse
import os
from pathlib import Path
import random
import shutil

import tarski
from tarski.io import PDDLReader, FstripsWriter
from tarski.model import Model
from tarski.syntax import Constant, land

arg_parser = argparse.ArgumentParser(description="Generate delivery problems")
arg_parser.add_argument("--grid_size_splits", type=str, required=True, help="Comma separated string of grid size splits for train, validation and test. Instances will include sizes up to the specified size, starting from 2 for the training instance. For example, if the argument is '5,7,9', then the training instances will include sizes in the interval [2, 5], validation instances [6, 7] and test instances [8, 9].")
arg_parser.add_argument("--max_nr_packages", type=int, required=True, help="Maximum number of packages in an instance")
arg_parser.add_argument("--nr_instances_per_setup", type=int, required=True, help="Number of instances to generate for each setup")
args = arg_parser.parse_args()

grid_size_splits = args.grid_size_splits.split(",")
grid_size_splits = [int(size.strip()) for size in grid_size_splits]
# The smallest grid size is 2
grid_size_splits.insert(0, 1)
grid_size_intervals = [(grid_size_splits[i] + 1, grid_size_splits[i + 1]) for i in range(len(grid_size_splits) - 1)]
max_nr_packages = args.max_nr_packages
nr_instances_per_setup = args.nr_instances_per_setup

domain_file = "domain.pddl"
splits = ["train", "validation", "test"]

output_folder = "output"
if os.path.exists(output_folder):
    shutil.rmtree(output_folder)
os.makedirs(output_folder)

def _generate_instances(split: str, grid_size_interval: tuple[int, int], max_nr_packages: int, nr_instances_per_setup: int, domain_file: Path = None) -> None:
    split_folder = os.path.join(output_folder, split)
    os.makedirs(split_folder)

    if domain_file:
        # Copy the domain file to the split folder
        shutil.copy(domain_file, split_folder)
        print(f"Copied domain file to {split_folder}")

    for size in range(grid_size_interval[0], grid_size_interval[1] + 1):
        for nr_packages in range(1, max_nr_packages + 1):
            for index in range(nr_instances_per_setup):
                pddl_reader = PDDLReader(raise_on_error=True)
                pddl_reader.parse_domain(domain_file)
                problem = pddl_reader.problem
                language = problem.language
                init: Model = tarski.model.create(language)

                # Create the cells
                # All instances are square grids
                cells: list[Constant] = []
                for x in range(size):
                    for y in range(size):
                        cell = language.constant(f"c_{x}_{y}", "cell")
                        cells.append(cell)

                # Create the packages
                packages: list[Constant] = []
                for i in range(1, nr_packages + 1):
                    package = language.constant(f"p{i}", "package")
                    packages.append(package)

                # Create the truck
                truck = language.constant("t1", "truck")

                # Add the "adjacent" atoms
                adjacent_predicate = language.get_predicate("adjacent")
                for x in range(size):
                    for y in range(size):
                        current_cell = language.get_constant(f"c_{x}_{y}")
                        if x < size - 1:
                            right_neighbor = language.get_constant(f"c_{x + 1}_{y}")
                            init.add(adjacent_predicate, current_cell, right_neighbor)
                        if x > 0:
                            left_neighbor = language.get_constant(f"c_{x - 1}_{y}")
                            init.add(adjacent_predicate, current_cell, left_neighbor)
                        if y < size - 1:
                            bottom_neighbor = language.get_constant(f"c_{x}_{y + 1}")
                            init.add(adjacent_predicate, current_cell, bottom_neighbor)
                        if y > 0:
                            top_neighbor = language.get_constant(f"c_{x}_{y - 1}")
                            init.add(adjacent_predicate, current_cell, top_neighbor)

                # Randomly sample cells for the packages
                at_predicate = language.get_predicate("at")
                package_cell_names = []
                for package in packages:
                    cell = random.choice(cells)
                    package_cell_names.append(cell.name)
                    init.add(at_predicate, package, cell)

                # Randomly sample a cell for the truck
                cell = random.choice(cells)
                init.add(at_predicate, truck, cell)

                # The truck always starts out empty
                init.add(language.get_predicate("empty"), truck)

                # Create the goal
                # If all packages are in the same cell, make sure that this is not the goal cell
                if len(set(package_cell_names)) == 1:
                    goal_cell = random.choice(cells)
                    while goal_cell.name == package_cell_names[0]:
                        goal_cell = random.choice(cells)
                else:
                    goal_cell = random.choice(cells)
                goal_atoms = [at_predicate(package, goal_cell) for package in packages]

                problem.name = f"delivery-{size}x{size}-{nr_packages}"
                problem.init = init
                problem.goal = land(*goal_atoms)

                # Write the problem to a file
                writer = FstripsWriter(problem)
                output_file_name = f"instance_s-{size}_p-{nr_packages}_v-{index}.pddl"
                output_file_path = os.path.join(split_folder, output_file_name)
                writer.write_instance(output_file_path, constant_objects=[])
                print(f"Generated {output_file_path}")

for split, grid_size_interval in zip(splits, grid_size_intervals):
    _generate_instances(split, grid_size_interval, max_nr_packages, nr_instances_per_setup, domain_file=domain_file)

print(f"Generated Delivery instances in {output_folder}")
