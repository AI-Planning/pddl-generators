#!/usr/bin/env python
import sys
import random

NUM_KILN8 = 1
NUM_KILN20 = 1

NUM_P1 = 1
NUM_P2 = 1
NUM_P3 = 1

# *****************#
# Functions declarations
# *****************#


def get_objects():
    str_objects = "\n"
    for i in range(0, NUM_KILN8):
        str_objects = str_objects + " kiln" + str(i)
    str_objects = str_objects + " - kiln8\n"
    for i in range(0, NUM_KILN20):
        str_objects = str_objects + " kiln" + str(i)
    str_objects = str_objects + " - kiln20\n"

    for i in range(0, NUM_P1):
        str_objects = str_objects + " pone" + str(i)
    str_objects = str_objects + " - piecetype1\n"

    for i in range(0, NUM_P2):
        str_objects = str_objects + " ptwo" + str(i)
    str_objects = str_objects + " - piecetype2\n"

    for i in range(0, NUM_P3):
        str_objects = str_objects + " pthree" + str(i)
    str_objects = str_objects + " - piecetype3\n"

    return str_objects


# *****************#


def get_init():
    str_init = "\n"
    str_init = str_init + "  (energy)\n"
    return str_init


# *****************#


def get_goals():
    str_goal = ""
    str_goal = str_goal + "\n  (and\n"

    p = []
    for i in range(0, NUM_P1):
        p.append("pone" + str(i))

    for i in range(0, NUM_P2):
        p.append("ptwo" + str(i))

    for i in range(0, NUM_P3):
        p.append("pthree" + str(i))

    while len(p) >= 2:
        p1 = p[random.randint(0, len(p) - 1)]
        p.remove(p1)
        p2 = p[random.randint(0, len(p) - 1)]
        p.remove(p2)
        str_goal = str_goal + "     (baked-structure " + p1 + " " + p2 + ")\n"

    str_goal = str_goal + ")"
    return str_goal


# *****************#
# MAIN
# *****************#
# Reading the command line arguments

import argparse
parser = argparse.ArgumentParser()
parser.add_argument("name", help="problem name")
parser.add_argument("p1", type=int, help="number of pieces of type 1")
parser.add_argument("p2", type=int, help="number of pieces of type 2")
parser.add_argument("p3", type=int, help="number of pieces of type 3")
parser.add_argument("--seed", type=int)

args = parser.parse_args()

random.seed(args.seed)

NUM_P1 = args.p1
NUM_P2 = args.p2
NUM_P3 = args.p3


print("(define (problem " + args.name + ")")
print(" (:domain domain-tms-2-3-light)")
print(" (:objects " + get_objects() + ")")
print(" (:init " + get_init() + ")")
print(" (:goal" + get_goals() + ")")
print(" (:metric minimize (total-time))")
print(")")

sys.exit(0)
