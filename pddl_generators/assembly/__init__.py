

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

parser.add_argument("d", type=int, help="depth of part-of tree                                                          ")
parser.add_argument("m", type=int, help="maximal number of sons to any node in the tree                                 ")
parser.add_argument("h", type=int, help="probability that a node has any sons                                           ")
parser.add_argument("n", type=int, help="number of distinct resources                                                   ")
parser.add_argument("r", type=int, help="probability that a non-base node requires a resource                           ")
parser.add_argument("t", type=int, help="probability that an item is transient part for any higher-in-tree item         ")
parser.add_argument("a", type=int, help="probability that a pair of items has an assemble order constraint              ")
parser.add_argument("o", type=int, help="probability that an item has a remove ordering constraint with a transient part")



def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("assembly"),
                               "-s", args.seed,
                               "-d", args2.d,
                               "-m", args2.m,
                               "-h", args2.h,
                               "-n", args2.n,
                               "-r", args2.r,
                               "-t", args2.t,
                               "-a", args2.a,
                               "-o", args2.o,
                               ])),
            stdout=f)

    pass

