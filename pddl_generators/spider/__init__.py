

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
    formatter_class = argparse.ArgumentDefaultsHelpFormatter,
    description = "Spider solitaire game.",
)


parser.add_argument(
    "num_decks", type=int, help="Number of decks of cards used for the game."
)
parser.add_argument(
    "num_suits", type=int, help="Number of different suits of cards in each deck."
)
parser.add_argument(
    "num_values",
    type=int,
    help="Number of different values of each suit in each deck.",
)
parser.add_argument("num_piles", type=int, help="Number of piles.")
parser.add_argument(
    "num_deals",
    type=int,
    help="Number of deals (each deal contains one card for each pile, remaining cards are distributed to the piles).",
)
# parser.add_argument("random_seed", type=int)
# parser.add_argument("output_type", choices=["pddl", "raw", "readable"])


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("generate.py"), *args.rest, args.seed, "pddl"])),
            stdout=f)

    pass

