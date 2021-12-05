

# Each module M must implement following attributes:
#
# 1. M.main : A main function which takes two argparser.Namespace instances, args and
#    args2. args is largely irrelevant except args.seed , which was
#    supplied by the command line argument.
#
# 2. M.parser : A parser , which parses the "rest" arguments that are specific to each domain.
#
# 3. M.domain_file : An absolute pathname to the domain pddl file, or None when the domain file is produced.








import os.path
import argparse
from pddl_generators.main import run
import pathlib

def rel(path): return os.path.join(os.path.dirname(__file__), path)



domain_file = rel("domain.pddl")


parser = argparse.ArgumentParser(
    # choose either one which suits the best
    formatter_class = argparse.ArgumentDefaultsHelpFormatter,
    formatter_class = argparse.RawDescriptionHelpFormatter,
    # use this to fill it manually
    description = "",
    # use this to read an existing README file
    description = pathlib.Path(rel("README")).read_text(),
    description = pathlib.Path(rel("README.txt")).read_text(),
)


parser.add_argument("--size_x", type=int, default=4)
parser.add_argument("--size_y", type=int, default=4)
parser.add_argument("--min_height", type=int, default=1)
parser.add_argument("--max_height", type=int, default=4)
parser.add_argument("--num_towers", type=int, default=4)

parser.add_argument("--ensure_plan_tries", type=int, default=20)

parser.add_argument("--ensure_plan", action="store_true")
parser.add_argument("--dont_remove_slack", action="store_true")
parser.add_argument("--store_plan", action="store_true")

# parser.add_argument("random_seed", type=int)
# parser.add_argument("output_type", choices=["pddl", "pddlfile", "readable"])


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("generate-autoscale.py"), *args.rest, args.seed, "pddl" ])),
            stdout=f)

    pass

