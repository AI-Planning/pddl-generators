

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
    description = pathlib.Path(rel("README.txt")).read_text(),
)


parser.add_argument("-p", type=int, help="number of parts (minimal 1)")
parser.add_argument("-s", type=int, help="number of shapes (preset: 0, maximal: 2)")
parser.add_argument("-c", type=int, help="number of colors (preset: 2, minimal 1, maximal: 4)")
parser.add_argument("-w", type=int, help="number of widths (preset: 2, minimal 1, maximal: 3)")
parser.add_argument("-o", type=int, help="number of orientations (preset: 2, minimal 1, maximal: 2)")
parser.add_argument("-Q", type=int, help="probability cylindrical goal (preset: 80)")
parser.add_argument("-W", type=int, help="probability of colour in I (preset: 50)")
parser.add_argument("-E", type=int, help="probability of colour in G (preset: 80)")
parser.add_argument("-R", type=int, help="probability of hole in I (preset: 50)")
parser.add_argument("-T", type=int, help="probability of hole in G (preset: 80)")
parser.add_argument("-Y", type=int, help="probability of surface condition in G (preset: 50)")
# parser.add_argument("-r", type=int, help="random seed (minimal 1, optional)")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("schedule"), *args.rest, "-r", args.seed])),
            stdout=f)

    pass

