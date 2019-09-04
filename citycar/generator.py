#! /usr/bin/env python

# CITYCAR DOMAIN GENERATOR.
# IPC 2014
# author: Mauro Vallati -- University of Huddersfield

from __future__ import print_function

import sys
import random


def help():
    print("usage: generator.py <rows> <columns> <n_cars> <n_garage> <sparse> [<seed>]")
    print("\t row and columns indicate the grid size. They should be > 1 ")
    print("\t n_cars indicates how many cars have to go through the network")
    print("\t n_garage indicates the number of starting garages")
    print(
        "\t if sparse != 0, the network has 'holes', i.e. junctions that cannot be used"
    )
    print("\t seed is the (optional) random seed for the instance")
    sys.exit(2)


to_print_car_pos = ""

if len(sys.argv) not in [6, 7]:
    help()
    sys.exit(2)

row = int(sys.argv[1])
column = int(sys.argv[2])
car = int(sys.argv[3])
garage = int(sys.argv[4])
sparse = int(sys.argv[5])

if len(sys.argv) == 7:
    seed = int(sys.argv[6])
else:
    seed = None

random.seed(seed)

road = int(sys.argv[1]) + 2

matrix = [["free" for i in range(column)] for j in range(row)]


for posizionate in range(car):
    x = random.randint(0, garage - 1)
    to_print_car_pos = (
        to_print_car_pos
        + "(starting car"
        + str(posizionate)
        + " garage"
        + str(x)
        + ")\n"
    )


# Finally, time for printing everything out.

print("(define (problem citycar-" + str(row) + "-" + str(column) + "-" + str(car) + ")")
print("(:domain citycar)")
print("(:objects ", end=" ")

for i in range(row):
    print("")
    for j in range(column):
        x = "junction" + str(i) + "-" + str(j)
        print(x, end=" ")
print("- junction")

for i in range(car):
    x = "car" + str(i)
    print(x, end=" ")
print("- car")

for i in range(garage):
    x = "garage" + str(i)
    print(x, end=" ")
print("- garage")

for i in range(road):
    x = "road" + str(i)
    print(x, end=" ")
print("- road")

print(")\n(:init")

# connected cells on the same line
for i in range(row):
    for j in range(column - 1):
        print(
            "(same_line junction"
            + str(i)
            + "-"
            + str(j)
            + " junction"
            + str(i)
            + "-"
            + str(j + 1)
            + ")"
        )
        print(
            "(same_line junction"
            + str(i)
            + "-"
            + str(j + 1)
            + " junction"
            + str(i)
            + "-"
            + str(j)
            + ")"
        )

# connected cells on the same column
for i in range(column):
    for j in range(row - 1):
        print(
            "(same_line junction"
            + str(j)
            + "-"
            + str(i)
            + " junction"
            + str(j + 1)
            + "-"
            + str(i)
            + ")"
        )
        print(
            "(same_line junction"
            + str(j + 1)
            + "-"
            + str(i)
            + " junction"
            + str(j)
            + "-"
            + str(i)
            + ")"
        )

# make the diagonal connections
for i in range(row - 1):
    for j in range(column - 1):
        print(
            "(diagonal junction"
            + str(i)
            + "-"
            + str(j)
            + " junction"
            + str(i + 1)
            + "-"
            + str(j + 1)
            + ")"
        )
        print(
            "(diagonal junction"
            + str(i + 1)
            + "-"
            + str(j + 1)
            + " junction"
            + str(i)
            + "-"
            + str(j)
            + ")"
        )
        print(
            "(diagonal junction"
            + str(i)
            + "-"
            + str(j + 1)
            + " junction"
            + str(i + 1)
            + "-"
            + str(j)
            + ")"
        )
        print(
            "(diagonal junction"
            + str(i + 1)
            + "-"
            + str(j)
            + " junction"
            + str(i)
            + "-"
            + str(j + 1)
            + ")"
        )

# free cells
for i in range(row):
    for j in range(column):
        if sparse != 0 and i != 0 and i < row - 1 and j != 0 and j < column - 1:
            x = random.randint(0, 10)
            if x < 8:
                print("(clear junction" + str(i) + "-" + str(j) + ")")
        else:
            print("(clear junction" + str(i) + "-" + str(j) + ")")

for i in range(garage):
    x = random.randint(0, (column - 1))
    print("(at_garage garage" + str(i) + " junction0" + "-" + str(x) + ")")

# occupied cells
print(to_print_car_pos, end=" ")
print("(= (total-cost) 0)")
print(")\n(:goal\n(and")

for i in range(car):
    x = random.randint(0, (column - 1))
    print("(arrived car" + str(i) + " junction" + str(row - 1) + "-" + str(x) + ")")

print(")\n)\n(:metric minimize (total-cost))\n)")
