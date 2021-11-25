

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

parser.add_argument("satellites", type=int, help="the number of satellites")
parser.add_argument("instruments", type=int, help="the maximum number of instruments per a satellite")
parser.add_argument("modes", type=int, help="the number of modes")
parser.add_argument("targets", type=int, help="the number of targets")
parser.add_argument("observations", type=int, help="the number of observations")


# -u : untyped -- default
# -t : timed
# -s : simpletime
# -n : numeric
# -c : complex
# -h : hard , but from the source code it seems unused
# -T : tightness ---

# parser.add_argument("--untyped", "-u", action="store_true", help="produces an untyped problem")
# parser.add_argument("--tightness", "-T", type=float, help="tightness of data in store capacities [0,1)")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("satgen"), args.seed, *args.rest])),
            stdout=f)

    pass

