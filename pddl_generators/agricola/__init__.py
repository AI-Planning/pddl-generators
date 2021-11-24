

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
    # use this to read an existing README file
    description = pathlib.Path(rel("README")).read_text(),
)


parser.add_argument("last_stage", type=int)
# parser.add_argument("seed", type=int, help=argparse.SUPPRESS)
parser.add_argument("--num_workers", type=int, default=5)
parser.add_argument("--num_ints", type=int, default=16)
parser.add_argument("--num_rounds", type=int, default=20)
parser.add_argument("--must_create_workers", action="store_true")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,["python3", rel("GenAgricola.py"), args.rest[0], args.seed, args.rest[1:]])),
            stdout=f)

    pass

