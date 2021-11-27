

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
    formatter_class = argparse.RawDescriptionHelpFormatter,
    description = pathlib.Path(rel("README")).read_text(),
)


parser.add_argument("products", type=int, help="Number of products")
parser.add_argument("orders", type=int, help="Number of orders")
parser.add_argument("density", type=int, help="approximate percentage of products included in an order")
parser.add_argument("--domain", default="domain.pddl")
parser.add_argument("--problem", default="problem.pddl")
parser.add_argument("--seed", type=int, default=1)


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    run(args,list(map(str,["python3", rel("generator.py"),
                           "--seed", args.seed,
                           "--domain", args.output_domain,
                           "--problem", args.output,
                           args.products,
                           args.orders,
                           args.density])))

    pass

