

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


parser.add_argument("areas", type=int, help="how many areas")
parser.add_argument("area_size", type=int, help="")

parser.add_argument("passengers", type=int, help="how many floors")

parser.add_argument("fast_elevators", type=int, help="")
parser.add_argument("slow_elevators", type=int, help="")

parser.add_argument("--fast_capacity", type=int, default=6, help="")
parser.add_argument("--slow_capacity", type=int, default=4, help="")
parser.add_argument("--slow_cost", type=int, default=1, help="")
parser.add_argument("--stop_slow_cost", type=int, default=5, help="")
parser.add_argument("--fast_cost", type=int, default=3, help="")
parser.add_argument("--stop_fast_cost", type=int, default=1, help="")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,["python3", rel("generate.py"), "--seed", args.seed, *args.rest])),
            stdout=f)

    pass

