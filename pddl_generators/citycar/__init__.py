

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
    description = pathlib.Path(rel("README")).read_text(),
)



parser.add_argument("--sparse",type=float,default=1.0,help="The ratio of the available roads.")
parser.add_argument("rows",type=int,help="the grid row.")
parser.add_argument("columns",type=int,help="the grid column.")
parser.add_argument("cars",type=int,help="how many cars have to go through the network")
parser.add_argument("garages",type=int,help="the number of starting garages")
# parser.add_argument("--seed",type=int,"random seed")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,["python3", rel("generator.py"), "--seed", args.seed, *args.rest])),
            stdout=f)

    pass

