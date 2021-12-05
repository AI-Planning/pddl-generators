

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


parser.add_argument("size",
                    type=int,
                    help="the number of the conveyer belt segments")
parser.add_argument("segment_type",
                    type=str,
                    choices=["ab"],
                    help="either a string ab or not probided. If present, each segment is split in half and moves independently.")
parser.add_argument("inout",
                    type=str,
                    choices=["in","both","none"],
                    help="Defines which conveyers are connected to the imaging chamber. "
                    "If none, only one converyers goes into / out of the chamber."
                    "If in, all converyers goes into the chamber, but only one converyer is connected to the exit of the chamber."
                    "If both, all converyers can go into the chamber, and come out of the chamber."
                    )


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("generator.py"), "--seed", args.seed, *args.rest])),
            stdout=f)

    pass

