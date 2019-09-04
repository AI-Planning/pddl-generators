#! /usr/bin/env python

import sys
import random


def help():
    print("usage: generator.py <n_couples> <n_cars> <n_places> [<seed>]")
    print("\t for solvability, cars should be at list n_couples + 1.")
    sys.exit(2)


if len(sys.argv) not in [4, 5]:
    help()

couples = int(sys.argv[1])
cars = int(sys.argv[2])
places = int(sys.argv[3])

if len(sys.argv) == 5:
    seed = int(sys.argv[4])
else:
    seed = None

random.seed(seed)

# hey, everything is ok. Let's start to do something.

print(
    "(define (problem Hiking-"
    + str(couples)
    + "-"
    + str(cars)
    + "-"
    + str(places)
    + ")"
)
print("(:domain hiking)")
print("(:objects ")

for i in range(cars):
    sys.stdout.write(" car" + str(i))

print(" - car")

for i in range(couples):
    sys.stdout.write(" tent" + str(i))

print(" - tent")

for i in range(couples):
    sys.stdout.write(" couple" + str(i))

print(" - couple")

for i in range(places):
    sys.stdout.write(" place" + str(i))

print(" - place")

for i in range(couples):
    sys.stdout.write(" guy" + str(i) + " girl" + str(i))

print(" - person")

print(")\n(:init")

for i in range(couples):
    print("(partners couple" + str(i) + " guy" + str(i) + " girl" + str(i) + ")")
    print("(at_person guy" + str(i) + " place0)")
    print("(at_person girl" + str(i) + " place0)")
    print("(walked couple" + str(i) + " place0)")
    print("(at_tent tent" + str(i) + " place0)")

    # random
    if random.randint(1, 2) == 2:
        print("(up tent" + str(i) + ")")
    else:
        print("(down tent" + str(i) + ")")

for i in range(cars):
    print("(at_car car" + str(i) + " place0)")

for i in range(places - 1):
    print("(next place" + str(i) + " place" + str(i + 1) + ")")

print(")\n(:goal\n(and")

for i in range(couples):
    print("(walked couple" + str(i) + " place" + str(places - 1) + ")")

print(")\n)\n)")
