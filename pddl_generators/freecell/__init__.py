

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
    formatter_class = argparse.RawDescriptionHelpFormatter,
    description = pathlib.Path(rel("README.txt")).read_text(),
)


parser.add_argument("cells", type=int, help="number of freecells (minimal 0)")
parser.add_argument("columns", type=int, help="number of columns (minimal 1)")
# parser.add_argument("suits", type=int, help="number of suits (minimal 1, maximal 4)")
parser.add_argument("stacks", type=int, help="number of initial stacks (minimal 1)")
parser.add_argument("cards", type=int, help="number of cards in each suit (minimal 2, maximal 20)")

def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("freecell"),
                               "-f", args2.cells,
                               "-c", args2.columns,
                               "-s", "4",
                               "-1", args2.cards,
                               "-2", args2.cards,
                               "-3", args2.cards,
                               "-4", args2.cards,
                               "-i", args2.stacks,
                               "-r", args.seed])),
            stdout=f)

    pass

