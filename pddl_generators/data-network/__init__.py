

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


parser.add_argument("data", type=int, help="the number of data items")
parser.add_argument("layers", type=int, help="the number of layers, must be smaller than the number of data items")
parser.add_argument("scripts", type=int, help="the number of scripts, must be larger or equal than [number of data items]-2")
parser.add_argument("module", choices=["ring-network","small-network","tiny-network"],
                    metavar="module",
                    help='A module describing a network, one of %(choices)s. See example files in the source code directory.')


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,["python3", rel("generator/generator.py"), "--seed", args.seed, *args.rest])),
            stdout=f)

    pass

