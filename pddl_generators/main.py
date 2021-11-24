
import os
import os.path
import glob
import argparse
import textwrap





parser = argparse.ArgumentParser(
    formatter_class = argparse.RawTextHelpFormatter,
    description =
    textwrap.fill(
        "Generates a problem file for a given domain and a set of hyperparameters. "
        "Depending on the domain, it also produces a domain file (e.g., ADL domain compiled to strips). "
        "In the default behavior, it generates the file(s) to a specified location, then print "
        "the absolute pathname of the problem file and the domain file in two separate lines. "
        "The second line (the domain file) can be a file generated by the generator, "
        "or a fixed pddl file that is shared by problems. "
        "When --output STDOUT is given, the output is printed to the standard output (see options for --output). ", 100)
)


parser.add_argument("domain",
                    metavar="domain",
                    nargs="?",  # note: for --help and --helpall to work
                    choices=domains,
                    help="domain name, one of: \n" + textwrap.fill(", ".join(domains), 100))

parser.add_argument("--output-directory","-d",
                    default="output",
                    metavar="DIRNAME",
                    help=
                    "Directory to store the generated files. \n"
                    "If the directory does not exist, it creates it first (including the parents). \n"
                    )

parser.add_argument("--seed","-s",
                    type=int,
                    default=42,
                    help="Random seed.")

parser.add_argument("--helpall",
                    action="store_true",
                    help="Generate help messages for all domains.")

parser.add_argument("rest",
                    nargs="*",
                    help="Remaining command line arguments for each domain.")


def main(args):
    pass



if __name__ == "__main__":
    main(parser.parse_args())

