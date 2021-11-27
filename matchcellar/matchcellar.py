#!/usr/bin/env python
import sys
import random

NUM_MATCHES = 1

# *****************#
# Functions declarations
# *****************#


def get_objects(fuses):
    str_objects = "\n   "
    for i in range(0, NUM_MATCHES):
        str_objects = str_objects + " match" + str(i)
    str_objects = str_objects + " - match"

    str_objects = str_objects + "\n   "

    for i in range(0, fuses):
        str_objects = str_objects + " fuse" + str(i)
    str_objects = str_objects + " - fuse\n"

    return str_objects


# *****************#


def get_init():
    str_init = "\n"
    str_init = str_init + "  (handfree)\n"

    for i in range(0, NUM_MATCHES):
        str_init = str_init + "  (unused match" + str(i) + ")\n"

    return str_init


# *****************#


def get_goals():
    str_goal = ""
    str_goal = str_goal + "\n  (and\n"
    for i in range(0, NUM_FUSES):
        str_goal = str_goal + "     (mended fuse" + str(i) + ")\n"

    str_goal = str_goal + ")"
    return str_goal


# *****************#
# MAIN
# *****************#
# Reading the command line arguments
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("matches", type=int, help="number of matches")
parser.add_argument("--seed", type=int, help="random seed")
args = parser.parse_args()
NUM_MATCHES = args.matches
if args.seed is not None:
    random.seed(args.seed)


NUM_FUSES = NUM_MATCHES * 2

print("(define (problem prob)")
print(" (:domain matchcellar)")
print(" (:objects " + get_objects(NUM_FUSES) + ")")
print(" (:init " + get_init() + ")")
print(" (:goal" + get_goals() + ")")
print(" (:metric minimize (total-time))")
print(")")

sys.exit(0)
