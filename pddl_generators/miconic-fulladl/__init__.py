

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


parser.add_argument("floors", type=int, help="the number of floors (minimal 2)")
parser.add_argument("passengers", type=int, help="the number of passengers (minimal 1)")

parser.add_argument("-u", default=20, type=int, help="percentage of up_down s (preset: 20)")
parser.add_argument("-v", default=5, type=int, help="percentage of vip s (preset: 5)")
parser.add_argument("-g", default=5, type=int, help="percentage of going_nonstop s (preset: 5)")
parser.add_argument("-a", default=60, type=int, help="percentage of attendant s (preset: 60)")
parser.add_argument("-n", default=10, type=int, help="percentage of never_alone s (preset: 10)")
parser.add_argument("-A", default=20, type=int, help="percentage of conflict_A s (preset: 20)")
parser.add_argument("-B", default=80, type=int, help="percentage of conflict_B s (preset: 80)")
parser.add_argument("-N", default=50, type=int, help="percentage of people with no-access (preset: 50)")
parser.add_argument("-F", default=5, type=int, help="percentage of floors not to be accessed by those (preset: 5)")



def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("miconic"),
                               "-f", args2.floors,
                               "-p", args2.passengers,
                               "-r", args.seed,
                               "-u", args2.u,
                               "-v", args2.v,
                               "-g", args2.g,
                               "-a", args2.a,
                               "-n", args2.n,
                               "-A", args2.A,
                               "-B", args2.B,
                               "-N", args2.N,
                               "-F", args2.F,])),
            stdout=f)

    pass

