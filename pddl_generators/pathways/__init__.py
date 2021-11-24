

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



domain_file = None


parser = argparse.ArgumentParser(
    # choose either one which suits the best
    formatter_class = argparse.RawDescriptionHelpFormatter,
    # use this to read an existing README file
    description = pathlib.Path(rel("README")).read_text(),
)


parser.add_argument("reactions", type=int, help="the number of ")
parser.add_argument("goals", type=int, help="the number of ")
parser.add_argument("substances", type=int, help="the number of ")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    run(args,list(map(str,["python3", rel("wrapper.py"),
                           "--seed", args.seed,
                           "--reactions", args2.reactions,
                           "--goals", args2.goals,
                           "--initial-substances", args2.substances,
                           args.output_domain,
                           args.output])))

    pass

