#!/usr/bin/env python3

"""Generate a random example input/output pair for one of the string
transformation tasks. The examples are the input for generate.py.

Tasks:
  add-paren SIZE:            (BOB => (BOB)
  extract-minutes:           8:23.65 => 23
  reverse NAME SURNAME:      BOB DOE => DOE BOB
  first-last-initial NAME SURNAME:  BOB DOE => BOB D
  initials NAME SURNAME:     BOB DOE => B D
"""

import argparse
import random
import string

TASKS = {"add-paren": 1, "extract-minutes": 0, "reverse": 2, "first-last-initial": 2, "initials": 2}


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("task", choices=TASKS)
    parser.add_argument("sizes", type=int, nargs="*", help="string sizes")
    parser.add_argument("--seed", type=int, default=0, help="random seed (default: %(default)s)")
    args = parser.parse_args()
    if len(args.sizes) != TASKS[args.task]:
        parser.error(f"{args.task} needs {TASKS[args.task]} size(s)")
    return args


def problem(name, inputs, num_indices, init, goal):
    variables = " ".join(f"{i}-VAR" for i in inputs)
    indices = "".join(f" I{i}" for i in range(2, num_indices + 1))
    return (
        f"(DEFINE (PROBLEM {name})\n"
        "  (:DOMAIN EXCEL)\n"
        "  (:OBJECTS\n"
        "\tSPACE HYPH COLON DOT LPAR RPAR - LIMITER\n"
        "\tA B C D E F G H I J K L M N O P Q R S T U V W X Y Z N0 N1 N2 N3 N4 N5 N6 N7 N8 N9 - CHAR\n"
        f"\t{' '.join(inputs)} - INPUT\n"
        "\tRES - OUTPUT\n"
        f"\t{variables} - INPUT-VARIABLE\n"
        f"\t{indices} - INDEX \n"
        "  )\n"
        "  (:INIT" + "".join(f"\n\t( {atom} )" for atom in init) + "\n  )\n"
        "  (:GOAL\n\t( AND" + "".join(f"\n\t( {atom} )" for atom in goal) + "\n\t)\n  )\n)"
    )


def one_string(name, size, assignments, goal):
    init = [f"HIINDEX STR I{size}", "LOINDEX STR I0", f"SIZE STR I{size}", "SIZE RES I0", "INPUT-ASSIGNMENT STR-VAR STR"]
    init += [f"NEXT I{i} I{i + 1}" for i in range(size + 1)]
    init += [f"ASSIGNMENT STR I{i} {c}" for i, c in assignments]
    return problem(name, ["STR"], size + 1, init, goal)


def two_strings(name, name_size, surname_size, rng, make_goal):
    first = [rng.choice(string.ascii_uppercase) for _ in range(name_size)]
    last = [rng.choice(string.ascii_uppercase) for _ in range(surname_size)]
    init = [
        f"HIINDEX STR I{name_size}",
        "LOINDEX STR I0",
        f"HIINDEX STR2 I{surname_size}",
        "LOINDEX STR2 I0",
        f"SIZE STR I{name_size}",
        f"SIZE STR2 I{surname_size}",
        "SIZE RES I0",
        "INPUT-ASSIGNMENT STR-VAR STR",
        "INPUT-ASSIGNMENT STR2-VAR STR2",
    ]
    init += [f"NEXT I{i} I{i + 1}" for i in range(name_size + surname_size + 1)]
    init += [f"ASSIGNMENT STR I{i} {c}" for i, c in enumerate(first)]
    init += [f"ASSIGNMENT STR2 I{i} {c}" for i, c in enumerate(last)]
    return problem(name, ["STR", "STR2"], name_size + surname_size + 1, init, make_goal(first, last))


def add_paren(name, size, rng):
    chars = ["LPAR"] + [rng.choice(string.ascii_uppercase) for _ in range(1, size)]
    goal = [f"ASSIGNMENT RES I{i} {c}" for i, c in enumerate(chars)] + [f"ASSIGNMENT RES I{size} RPAR"]
    return one_string(name, size, enumerate(chars), goal)


def extract_minutes(name, rng):
    hour = rng.randint(0, 9)
    minutes = rng.randint(0, 59)
    seconds = rng.randint(0, 59)
    m1, m2 = divmod(minutes, 10)
    s1, s2 = divmod(seconds, 10)
    # Like the original generator, we also assign LPAR to the first position.
    chars = [(0, "LPAR"), (0, f"N{hour}"), (1, "COLON"), (2, f"N{m1}"), (3, f"N{m2}"), (4, "DOT"), (5, f"N{s1}"), (6, f"N{s2}")]
    goal = [f"ASSIGNMENT RES I0 N{m1}", f"ASSIGNMENT RES I1 N{m2}"]
    return one_string(name, 6, chars, goal)


def reverse(first, last):
    goal = [f"ASSIGNMENT RES I{i} {c}" for i, c in enumerate(last)]
    goal.append(f"ASSIGNMENT RES I{len(last)} SPACE")
    goal += [f"ASSIGNMENT RES I{len(last) + 1 + i} {c}" for i, c in enumerate(first)]
    return goal


def first_last_initial(first, last):
    goal = [f"ASSIGNMENT RES I{i} {c}" for i, c in enumerate(first)]
    goal += [f"ASSIGNMENT RES I{len(first)} SPACE", f"ASSIGNMENT RES I{len(first) + 1} {last[0]}"]
    goal.append(f"SIZE RES I{len(first) + 2}")
    return goal


def initials(first, last):
    return [f"ASSIGNMENT RES I0 {first[0]}", "ASSIGNMENT RES I1 SPACE", f"ASSIGNMENT RES I2 {last[0]}", "SIZE RES I3"]


def main():
    args = parse_args()
    rng = random.Random(args.seed)
    name = "-".join([args.task.upper()] + [str(x) for x in args.sizes + [args.seed]])
    if args.task == "add-paren":
        print(add_paren(name, args.sizes[0], rng))
    elif args.task == "extract-minutes":
        print(extract_minutes(name, rng))
    else:
        make_goal = {"reverse": reverse, "first-last-initial": first_last_initial, "initials": initials}[args.task]
        print(two_strings(name, *args.sizes, rng, make_goal))


if __name__ == "__main__":
    main()
