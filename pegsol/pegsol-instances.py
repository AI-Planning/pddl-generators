#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Gives statistics for the instances of the Peg Solitaire suite.
# We use this to pick the 30 instances to use for the competition.
##
# Note that this script does not read the sources provided in
# instances.rb, so needs to be changed if different instances are
# used.
##
# The list of "unsolvable" instances doesn't match what is declared
# in the "diagonal moves" info for some instances, but has been
# obtained through running the planners. Apparently, the info in the
# instance descriptions has a few errors. ("Unsolvable" instances that
# aren't.)

pegs = [
    11,
    5,
    6,
    7,
    11,
    10,
    8,
    8,
    8,
    9,
    7,
    8,
    10,
    6,
    7,
    7,
    10,
    9,
    12,
    12,
    7,
    8,
    8,
    8,
    7,
    7,
    8,
    8,
    8,
    16,
    19,
    11,
    14,
    10,
    12,
    15,
    11,
    10,
    13,
    15,
    14,
    13,
    10,
    17,
    10,
    8,
    18,
    9,
    9,
    9,
    15,
    10,
    11,
    12,
    14,
    11,
    8,
    9,
    9,
    16,
    11,
    13,
    16,
    12,
    17,
    12,
    13,
    12,
    18,
    15,
    19,
    19,
    16,
    25,
    9,
    12,
    10,
    12,
    8,
    17,
    13,
    10,
    13,
    23,
    16,
    12,
    16,
    13,
    11,
    22,
    19,
    17,
    17,
    12,
    17,
    15,
    13,
    21,
    28,
    16,
    10,
    12,
    9,
    20,
    32,
]

unsolvable = [
    22,
    23,
    25,
    27,
    28,
    29,
    35,
    38,
    43,
    45,
    49,
    50,
    57,
    59,
    69,
    72,
    76,
    77,
    79,
    81,
    83,
    86,
    89,
    90,
    91,
    94,
    95,
    96,
    98,
    100,
    103,
]


def report(instances):
    instances_and_pegs = [(instance, pegs[instance - 1]) for instance in instances]
    instances_and_pegs.sort(key=lambda x: (x[1], x[0]))
    for pair in instances_and_pegs:
        print("instance #%03d: %2d pegs" % pair)


solvable = set(range(1, len(pegs) + 1)) - set(unsolvable)
print("Solvable instances (%d):" % len(solvable))
report(solvable)
print()
print("Unsolvable instances (%d):" % len(unsolvable))
report(unsolvable)
