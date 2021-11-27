

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


parser.add_argument("depots", type=int, help="the number of depots")
parser.add_argument("distributors", type=int, help="the number of distributors")
parser.add_argument("trucks", type=int, help="the number of trucks")
parser.add_argument("pallets", type=int, help="the number of pallets")
parser.add_argument("hoists", type=int, help="the number of hoists")
parser.add_argument("crates", type=int, help="the number of crates")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("depots"), "-s", args.seed,
                               "-e", args2.depots,
                               "-i", args2.distributors,
                               "-t", args2.trucks,
                               "-p", args2.pallets,
                               "-h", args2.hoists,
                               "-c", args2.crates,
                               ])),
            stdout=f)

    pass

