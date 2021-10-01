#! /usr/bin/python3

"""
Wrapper for Pathways.

Creates domain.pddl and problem.pddl files in the current directory.
"""

import argparse
from collections import defaultdict
import os.path
from pathlib import Path
import re
import subprocess
import sys

DIR = Path(__file__).parent.resolve()

DOMAIN_HEADER = """\
(define (domain Pathways-Propositional)
(:requirements :typing)

(:types
    level molecule - object
    simple complex - molecule
)
"""

PREDICATES = """\
(:predicates
    (association-reaction ?x1 ?x2 - molecule ?x3 - complex)
    (catalyzed-association-reaction ?x1 ?x2 - molecule ?x3 - complex)
    (synthesis-reaction ?x1 ?x2 - molecule)
    (possible ?x - molecule)
    (available ?x - molecule)
    (chosen ?s - simple)
    (next ?l1 ?l2 - level)
    (num-subs ?l - level)
{goal_predicates}
)
"""

ACTIONS = """\
(:action choose
 :parameters (?x - simple ?l1 ?l2 - level)
 :precondition (and (possible ?x) (not (chosen ?x))
                    (num-subs ?l2) (next ?l1 ?l2))
 :effect (and (chosen ?x) (not (num-subs ?l2)) (num-subs ?l1)))

(:action initialize
  :parameters (?x - simple)
  :precondition (and (chosen ?x))
  :effect (and (available ?x)))

(:action associate
 :parameters (?x1 ?x2 - molecule ?x3 - complex)
 :precondition (and (association-reaction ?x1  ?x2  ?x3)
                    (available ?x1) (available ?x2))
 :effect (and  (not (available ?x1)) (not (available ?x2)) (available ?x3)))

(:action associate-with-catalyze
 :parameters (?x1 ?x2 - molecule ?x3 - complex)
 :precondition (and (catalyzed-association-reaction ?x1 ?x2 ?x3)
                    (available ?x1) (available ?x2))
 :effect (and (not (available ?x1)) (available ?x3)))

(:action synthesize
 :parameters (?x1 ?x2 - molecule)
 :precondition (and (synthesis-reaction ?x1 ?x2) (available ?x1))
 :effect (and (available ?x2)))
"""


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--goals", type=int, default=1)
    parser.add_argument("--reactions", type=int, default=1)
    parser.add_argument("--initial-substances", type=int, default=1)
    parser.add_argument("--seed", type=int, default=2004)
    parser.add_argument("domain")
    parser.add_argument("problem")
    return parser.parse_args()


def get_simplesubs():
    with open(DIR / "Pathways-SimpleSubs") as f:
        return set(f.read().splitlines())


def extract_molecules(problem_file):
    molecules = defaultdict(list)
    new_lines = []
    with open(problem_file) as f:
        for line in f:
            match = re.match(r"\t(.+) - (simple|complex)\n", line)
            if match:
                molecules[match.group(2)].append(match.group(1))
            else:
                new_lines.append(line)
    with open(problem_file, "w") as f:
        f.writelines(new_lines)
    return molecules


def get_goals(dummy_actions):
    molecules = re.findall(r"\(available ([^\)]+)\)", dummy_actions)
    goals = re.findall(r":effect \(and \((.+)\)\)\)", dummy_actions)
    assert len(molecules) % 2 == 0
    assert len(goals) == len(molecules) / 2
    return list(zip(molecules[::2], molecules[1::2], goals))


def make_strips_dummy_action(molecule, action_index, goal):
    return f"""\
(:action DUMMY-STRIPS-ACTION-{action_index}
 :parameters ()
 :precondition (available {molecule})
 :effect (and ({goal})))
"""


def main():
    args = parse_args()

    simplesubs = get_simplesubs()

    problem_file = os.path.abspath(args.problem)

    p = subprocess.run(
        [
            os.path.join(DIR, "pathways"),
            "--seed",
            str(args.seed),
            "-out",
            problem_file,
            "-R",
            str(args.reactions),
            "-G",
            str(args.goals),
            "-L",
            str(args.initial_substances),
            "-n",
            "Pathways-Problem",
        ],
        stdout=subprocess.PIPE,
        universal_newlines=True,
        check=True,
    )
    dummy_actions_string_original = p.stdout.strip().replace("\t", "    ")

    # Define molecules in domain file instead of problem file. Not sure if this is necessary.
    molecules = extract_molecules(problem_file)
    duplicate_molecules = set(molecules["simple"]) & set(molecules["complex"])
    if duplicate_molecules:
        sys.exit(f"Error: simple and complex molecules overlap: {duplicate_molecules}")

    goals = get_goals(dummy_actions_string_original)

    for mol1, mol2, _goal in goals:
        for molecule in [mol1, mol2]:
            if molecule not in set(molecules["simple"]) | set(molecules["complex"]):
                print(f"Molecule {molecule} is undefined --> will define it now")
                if molecule in simplesubs:
                    molecules["simple"].append(molecule)
                else:
                    molecules["complex"].append(molecule)

    # Turn each disjunctive dummy action into two STRIPS actions with a single precondition.
    dummy_strips_actions = []
    for mol1, mol2, goal in goals:
        for molecule in [mol1, mol2]:
            dummy_strips_actions.append(
                make_strips_dummy_action(molecule, len(dummy_strips_actions), goal)
            )

    constants_pddl = "(:constants\n    {} - simple\n\n    {} - complex)\n".format(
        " ".join(molecules["simple"]), " ".join(molecules["complex"])
    )

    goal_predicates = "\n".join(
        [f"    (goal{goal})" for goal in range(1, args.goals + 1)]
    )
    dummy_actions_string = "\n".join([f"{action}" for action in dummy_strips_actions])

    domain_parts = [
        DOMAIN_HEADER,
        constants_pddl,
        PREDICATES.format(**locals()),
        ACTIONS,
        f"{dummy_actions_string}\n)",
        "",
    ]

    with open(args.domain, "w") as f:
        f.write("\n".join(domain_parts))


main()
