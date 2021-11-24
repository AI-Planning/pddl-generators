

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



domain_file = rel("child-snack-pool.pddl")


parser = argparse.ArgumentParser(
    # choose either one which suits the best
    formatter_class = argparse.RawDescriptionHelpFormatter,
    # use this to read an existing README file
    description = pathlib.Path(rel("README.txt")).read_text(),
)


parser.add_argument("children", type=int, help="the number of children", default=2)
parser.add_argument("trays", type=int, help="the number of trays", default=3)
parser.add_argument("gluten_factor", type=float, help="", default=0.4)
parser.add_argument("constrainedness", type=float, help="", default=1.3)


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel(child-snack-generator.py), "pool", args.seed, *args.rest])),
            stdout=f)

    pass

