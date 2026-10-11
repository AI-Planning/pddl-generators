#!/usr/bin/env python3

"""Compile examples of a string transformation (see generate-example.py) into
a planning task whose solutions are programs with the given number of lines
that solve all examples.
"""

import argparse
import os
import re
import subprocess
import tempfile

DIR = os.path.dirname(os.path.abspath(__file__))
CHARS = "A B C D E F G H I J K L M N N0 N1 N2 N3 N4 N5 N6 N7 N8 N9 O P Q R S T U V W X Y Z".split()
LIMITERS = "COLON DOT HYPH LPAR RPAR SPACE".split()
REQUIREMENTS = "( :REQUIREMENTS :ACTION-COSTS :CONDITIONAL-EFFECTS :TYPING )"


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("lines", type=int, help="number of program lines")
    parser.add_argument("examples", nargs="+", help="example files")
    parser.add_argument("--domain", required=True, help="output domain file")
    parser.add_argument("--problem", required=True, help="output problem file")
    return parser.parse_args()


def compile_task(lines, examples):
    """Return the domain and problem written by the compiler."""
    cmd = [os.path.join(DIR, "compile"), "PLPR", os.path.join(DIR, "domain_variables.pddl"), str(len(examples))]
    cmd += [os.path.abspath(example) for example in examples] + [str(lines), str(lines)]
    cmd += "0 1 INPUT-ASSIGNMENT 1 INPUT-VARIABLE 2 1 0 0 0".split()
    # Run the compiler in a temporary directory since it writes the file act2ins.txt.
    with tempfile.TemporaryDirectory() as tmp:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True, cwd=tmp)
    return result.stdout, result.stderr


def remove_unused_constants(domain, problem, constants):
    """Only declare the constants that the domain or problem use."""
    declaration = " ".join(constants)
    lines = domain.splitlines() + problem.splitlines()
    used = set(re.split(r"\W+", "\n".join(line for line in lines if declaration not in line)))
    return domain.replace(declaration, " ".join(c for c in constants if c in used))


def main():
    args = parse_args()
    domain, problem = compile_task(args.lines, args.examples)
    domain = remove_unused_constants(domain, problem, CHARS)
    domain = remove_unused_constants(domain, problem, LIMITERS)
    domain = domain.replace(REQUIREMENTS, REQUIREMENTS[:-1] + ":NEGATIVE-PRECONDITIONS :ADL)")
    with open(args.domain, "w") as f:
        f.write(domain)
    with open(args.problem, "w") as f:
        f.write(problem)


if __name__ == "__main__":
    main()
