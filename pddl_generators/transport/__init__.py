

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


parser.add_argument("cities", type=int, choices=[1,2,3], help="the number of cities")

parser.add_argument("nodes", type=int, help="the number of nodes")
parser.add_argument("size", type=int, help="the width and the height of the grid size")
parser.add_argument("degree", type=int, help="the degree of the nodes")
parser.add_argument("distance", type=int, help="if two nodes gets closer than this distance, they get connected")
parser.add_argument("trucks", type=int, help="the number of trucks")
parser.add_argument("packages", type=int, help="the number of packages")
# seed

# ./city-generator.py          <nodes> <size^(1/2)> <degree> <mindistance> <nr-trucks> <nr-packages> <seed>
# ./two-cities-generator.py    <nodes> <size^(1/2)> <degree> <mindistance> <nr-trucks> <nr-packages> <seed>
# ./three-cities-generator.py  <nodes> <size^(1/2)> <degree> <mindistance> <nr-trucks> <nr-packages> <seed>


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel(["city-generator.py",
                                    "two-cities-generator.py",
                                    "three-cities-generator.py"][args2.cities]),
                               *args.rest,
                               args.seed])),
            stdout=f)

    pass

