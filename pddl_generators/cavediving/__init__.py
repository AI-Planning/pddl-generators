

# Each module M must implement following attributes:
#
# 1. M.main : A main function which takes two argparser.Namespace instances, args and
#    args2. args is largely irrelevant except args.seed , which was
#    supplied by the command line argument.
#
# 2. M.parser : A parser , which parses the "rest" arguments that are specific to each domain.
#
# 3. M.domain_file : An absolute pathname to the domain pddl file, or None when the domain file is produced.







import sys
import os.path
import argparse
from pddl_generators.main import run
import pathlib

def rel(path): return os.path.join(os.path.dirname(__file__), path)



domain_file = None


parser = argparse.ArgumentParser(
    formatter_class = argparse.RawDescriptionHelpFormatter,
    description = pathlib.Path(rel("README")).read_text(),
)


# parser.add_argument("", type=int, help="the number of ")  -domain_file          : The output PDDL domain file [optional, default: None]
# parser.add_argument("", type=int, help="the number of ")  -problem_file         : The output PDDL problem file
# parser.add_argument("", type=int, help="the number of ")  -problem_name         : The name of the problem instance
parser.add_argument("-cave_branches",        default=3, help="Depths of the cave branches (x0:...:xk) {1 <= k, 2 <= xi} [optional, default: [3]]")
parser.add_argument("-objectives",           default=3, help="The depths of the photo objectives (x0:...:xk) {1 <= k, 1 <= xi} [optional, default: [3]]")
parser.add_argument("-num_tank_adjustment",  default=0, type = int, help="How many more/fewer tanks than required to use [optional, default: 0]")
parser.add_argument("-num_diver_adjustment", default=0, type = int, help="How many more/fewer divers than required to use [optional, default: 0]")
parser.add_argument("-neg_link_prob",        default=0.5, type = float, help="The probability of a negative diver relationship link {0 <= x <= 1} [optional, default: 0.5]")
parser.add_argument("-neg_cycle_length",     default=0.0, help="The frac of divers in a negative cycle {0 <= x <= 1} [optional, default: 0.0]")
parser.add_argument("-minimum_hiring_cost",  default=10, type = int, help="The minimum cost of any hiring action {0 <= x} [optional, default: 10]")
parser.add_argument("-maximum_hiring_cost",  default=100, type = int, help="The maximum cost of any hiring action {0 <= x} [optional, default: 100]")
parser.add_argument("-perturb_hiring_costs", default=0, type = float, help="How much hiring costs should be perturbed {0 <= x <= 1} [optional, default: 0]")
parser.add_argument("-other_action_cost",    default=1, type = int, help="The cost of non-hiring actions {0 <= x} [optional, default: 1]")
# parser.add_argument("strips",               type = int, help="If the domain should be compiled to STRIPS {True, False} [optional, default: False]")
parser.add_argument("-order_tanks",          default=True, type = bool, help="Force an order of the preparation of the tanks {True, False} [optional, default: True]")
# parser.add_argument("seed",                 type = int, help="Random seed (None for system time) [optional, default: None]")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    run(args,list(map(str,["python3", rel("generator/generator.py"),
                           "-domain_file", args.output_domain,
                           "-problem_file", args.output,
                           "-problem_name", "prob",
                           "-cave_branches", args2.cave_branches,
                           "-objectives", args2.objectives,
                           "-num_tank_adjustment", args2.num_tank_adjustment,
                           "-num_diver_adjustment", args2.num_diver_adjustment,
                           "-neg_link_prob", args2.neg_link_prob,
                           "-neg_cycle_length", args2.neg_cycle_length,
                           "-minimum_hiring_cost", args2.minimum_hiring_cost,
                           "-maximum_hiring_cost", args2.maximum_hiring_cost,
                           "-perturb_hiring_costs", args2.perturb_hiring_costs,
                           "-other_action_cost", args2.other_action_cost,
                           "-strips", "True",
                           "-order_tanks", args2.order_tanks,
                           "-seed", args.seed,
                           ])),
        stdout=sys.stderr,
        stderr=sys.stderr)

    pass

