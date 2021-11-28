

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
    formatter_class = argparse.RawDescriptionHelpFormatter,
    description = pathlib.Path(rel("README.md")).read_text(),
)


parser.add_argument("--rows", type=int, default=7, help="Number of rows, must be odd. default=7")
parser.add_argument("--cols", type=int, default=7, help="Number of columns, must be odd. default=7")
parser.add_argument("--rowcuts", type=int, default=2, help="Number of rows to cut from each corner. Should be less than floor(rows/2). default=2")
parser.add_argument("--colcuts", type=int, default=2, help="Number of cols to cut from each corner. Should be less than floor(cols/2). default=2")
parser.add_argument("occupancy", type=float, help="Probability that the pegs are initially occupied. (0, 1]")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,["python3", rel("generate.py"), "--seed", args.seed, *args.rest])),
            stdout=f)

    pass

