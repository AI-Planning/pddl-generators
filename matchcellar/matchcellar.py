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
try:
    name = sys.argv[1]
    NUM_MATCHES = int(sys.argv[2])
except BaseException:
    print("Usage: " + sys.argv[0] + " <name> <num_matches>")
    sys.exit(1)

random.seed()


NUM_FUSES = NUM_MATCHES * 2

print(("(define (problem " + name + ")"))
print(" (:domain matchcellar)")
print((" (:objects " + get_objects(NUM_FUSES) + ")"))
print((" (:init " + get_init() + ")"))
print((" (:goal" + get_goals() + ")"))
print(" (:metric minimize (total-time))")
print(")")

sys.exit(0)
