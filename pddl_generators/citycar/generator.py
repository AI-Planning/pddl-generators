#! /usr/bin/env python

# CITYCAR DOMAIN GENERATOR.
# IPC 2014
# author: Mauro Vallati -- University of Huddersfield


import sys
import random
import argparse


to_print_car_pos = ""

parser = argparse.ArgumentParser()

parser.add_argument("rows",type=int,help="the number of grid rows.")
parser.add_argument("columns",type=int,help="the number of grid columns.")
parser.add_argument("cars",type=int,help="how many cars have to go through the network")
parser.add_argument("garages",type=int,help="the number of starting garages")
parser.add_argument("--density",type=float,default=1.0,help="The ratio of the available roads in the road network (0.0 <= density <= 1.0). Junctions are randomly set obstructed/unavailable for passage")
parser.add_argument("--seed",type=int,help="random seed")

args = parser.parse_args()

row     = args.rows
column  = args.columns
car     = args.cars
garage  = args.garages
density = args.density
seed    = args.seed

if not (0.0 <= density <= 1.0):
    parser.error("density must be a probability (0.0 <= density <= 1.0)")


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
        if density != 1.0 and i != 0 and i < row - 1 and j != 0 and j < column - 1:
            if density > random.random():
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
