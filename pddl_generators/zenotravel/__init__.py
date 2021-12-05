

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


parser.add_argument("cities", type=int, help="the number of cities")
parser.add_argument("planes", type=int, help="the number of planes")
parser.add_argument("people", type=int, help="the number of people")
parser.add_argument("distance", type=int, nargs="?", help="the distance between cities. "
                    "If distance is a positive value then the distances between "
                    "locations is set randomly using this value as the bound."
                    )


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("ztravel"), args.seed, *args.rest])),
            stdout=f)

    pass

