

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
    description = pathlib.Path(rel("Readme")).read_text(),
)


parser.add_argument("days", type=int, help="number of day: plan horizon")
parser.add_argument("crews", type=int, help="number of crew members")
parser.add_argument("util", type=int, help="utilization level. 2: 50%%, 3: 75%%, 4: 100%%")


def main(args : argparse.Namespace, args2 : argparse.Namespace):

    with open(args.output, mode="w") as f:
        run(args,list(map(str,[rel("main.native"),
                               "-seed", args.seed,
                               "-day", args2.days,
                               "-crew", args2.crews,
                               "-util", args2.util,
                               ])),
            stdout=f)

    pass

