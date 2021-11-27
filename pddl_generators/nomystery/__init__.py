

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


parser.add_argument("locations", type=int, help="number of locations (>= 2; preset: 6)")
parser.add_argument("packages", type=int, help="number of packages (>= 1; preset: 6)")
parser.add_argument("average_degree", type=float, help="average number of edges per location (>= 1; preset: 1.500000)")
parser.add_argument("distance", type=int, help="edge weights drawn from 1 .. <-m> (>= 1; preset: 25)")

parser.add_argument("constrainedness", type=int, help="constrainedness * [optimalcost] will be the initial fuel supply (>= 1; preset: 1.000000)")
parser.add_argument("mode", type=int, help="type of encoding (0 = hard; 1 = hard_cost; preset: 0)")



def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("mystery"),
                               "-l", args2.locations,
                               "-p", args2.packages,
                               "-n", args2.average_degree,
                               "-m", args2.distance,
                               "-c", args2.constrainedness,
                               "-e", args2.mode,
                               "-s", args.seed,])),
            stdout=f)

    pass

