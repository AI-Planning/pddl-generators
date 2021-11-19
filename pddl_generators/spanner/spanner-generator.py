#!/usr/bin/env python

import argparse
import random


def get_objects(args):
    str_objects = "\n"

    # -- man
    str_objects = str_objects + "     bob - man\n    "

    # -- spanner
    for i in range(1, args.spanners + 1):
        str_objects = str_objects + " spanner" + str(i)
    str_objects = str_objects + " - spanner\n    "

    # -- nuts
    for i in range(1, args.nuts + 1):
        str_objects = str_objects + " nut" + str(i)
    str_objects = str_objects + " - nut\n    "

    # -- locations
    for i in range(1, args.locations + 1):
        str_objects = str_objects + " location" + str(i)
    str_objects = str_objects + " - location\n    "

    str_objects = str_objects + " shed gate - location\n    "

    return str_objects


def get_init(args):
    str_init = "\n"
    str_init = str_init + "    (at bob shed)\n"

    for i in range(1, args.spanners + 1):
        str_init = (
            str_init
            + "    (at spanner"
            + str(i)
            + " location"
            + str(random.randint(1, args.locations))
            + ")\n"
        )
        str_init = str_init + "    (useable spanner" + str(i) + ")\n"

    for i in range(1, args.nuts + 1):
        str_init = str_init + "    (loose nut" + str(i) + ")\n"
        str_init = str_init + "    (at nut" + str(i) + " gate)\n"

    str_init = str_init + "    (link shed location1)\n"
    str_init = str_init + "    (link location" + str(args.locations) + " gate)\n"

    for i in range(1, args.locations):
        str_init = (
            str_init + "    (link location" + str(i) + " location" + str(i + 1) + ")\n"
        )

    return str_init


def get_goals(args):
    str_goal = ""
    str_goal = str_goal + "\n  (and\n"

    for i in range(1, args.nuts + 1):
        str_goal = str_goal + "   (tightened nut" + str(i) + ")\n"

    str_goal = str_goal + ")"
    return str_goal


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("spanners", type=int, help="number of spanners (min 1)")
    parser.add_argument("nuts", type=int, help="number of nuts (min 1 and <= spanners)")
    parser.add_argument("locations", type=int, help="number of locations (min 1)")
    parser.add_argument("--seed", type=int, default=None, help="random seed (default: use time-based seed)")
    parser.add_argument("--problem-name", default="prob", help="number of locations (min 1)")
    return parser.parse_args()


def main():
    args = parse_args()

    random.seed(args.seed)

    print("(define (problem " + args.problem_name + ")")
    print(" (:domain spanner)")
    print(" (:objects " + get_objects(args) + ")")
    print(" (:init " + get_init(args) + ")")
    print(" (:goal" + get_goals(args) + "))")


main()
