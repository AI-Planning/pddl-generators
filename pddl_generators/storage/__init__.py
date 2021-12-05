

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


# parser.add_argument("p", type=int, help="the number of problem (header of the problem file)")
parser.add_argument("n", type=int, help="the number of hoists (default 3)")
parser.add_argument("d", type=int, help="the number of depots (default 1)")
parser.add_argument("o", type=int, help="the number of containers (default 1)")
parser.add_argument("s", type=int, help="the number of store-areas (default 9)")
parser.add_argument("c", type=int, help="the number of crates (default 5)")
# parser.add_argument("e", type=int, help="seed for random number generator (must be different from 0)")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("storage"),
                               "-n", args2.n,
                               "-d", args2.d,
                               "-o", args2.o,
                               "-s", args2.s,
                               "-c", args2.c,
                               "-e", args.seed])),
            stdout=f)

    pass

