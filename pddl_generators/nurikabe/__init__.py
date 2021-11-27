

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


# parser.add_argument("source", choices=["lpo", "random"])
# parser.add_argument("pid", type=int, help="problem ID on logicgamesonline.com. Ignored when source=random.")
parser.add_argument("width", type=int, help="Width of the board.")
parser.add_argument("height", type=int, help="Height of the board.")
# parser.add_argument("random_seed", type=int)
# parser.add_argument("output_type", choices=["pddl", "raw", "solution"])


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,["python3", rel("generate.py"), "random", -1, args2.width, args2.height, args.seed, "pddl"])),
            stdout=f)

    pass

