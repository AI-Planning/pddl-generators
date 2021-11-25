

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

parser.add_argument("a", type=int, help="number of airplanes")
parser.add_argument("c", type=int, help="number of cities (minimal 1)")
parser.add_argument("s", type=int, help="city size (minimal 1)")
parser.add_argument("p", type=int, help="number of packages (minimal 1)")
parser.add_argument("t", type=int, help="number of trucks (optional, default and minimal: same as number of cities; there will be at least one truck per city)")
# parser.add_argument("-r", type=int, help="random seed (minimal 1, optional)")

def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("logistics"),
                               "-r", args.seed,
                               "-a", args2.a,
                               "-c", args2.c,
                               "-s", args2.s,
                               "-p", args2.p,
                               "-t", args2.t,
                               ])),
            stdout=f)

    pass

