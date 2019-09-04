#! /usr/bin/env python

# TETRIS DOMAIN GENERATOR.
# IPC 2014
# author: Mauro Vallati -- University of Huddersfield

from __future__ import print_function

import sys
import random

to_stamp_init = ""
to_stamp_square = ""
to_stamp_straight = ""
to_stamp_L_right = ""
rows = 4


def help():
    print("usage: generator.py <grid_size> <conf_blocks>")
    print(
        "\t line numbers is used for defining number of lines of the screen. "
        "The number of column is fixed at 4. E.g., 8-> 8x4 grid. "
        "Only odd numbers accepted."
    )
    print("\t conf_blocks:")
    print("\t\t 1 -> only 1x1 square blocks")
    print("\t\t 2 -> only 2x1 blocks")
    print("\t\t 3 -> only L-shaped blocks")
    print("\t\t 4 -> mix of blocks")
    sys.exit(2)


def put_squares(real_number_square):
    global to_stamp_square
    global to_stamp_init
    global matrix
    squares = [i for i in range(real_number_square)]
    posizionati = 0
    while posizionati < real_number_square:
        x = random.randint(0, (size_grid / 2))
        y = random.randint(0, rows - 1)
        if matrix[x][y] == "free":
            matrix[x][y] = "square" + str(squares[posizionati])
            to_stamp_square = to_stamp_square + matrix[x][y] + " "
            to_stamp_init = (
                to_stamp_init
                + "(at_square square"
                + str(squares[posizionati])
                + " f"
                + str(x)
                + "-"
                + str(y)
                + "f)"
                + "\n"
            )
            posizionati = posizionati + 1


if len(sys.argv) != 3:
    help()
size_grid = int(sys.argv[1])
conf_blocks = int(sys.argv[2])
if size_grid < 2:
    print("grid_size too small. Try something bigger than 2.")
    help()
if size_grid % 2 != 0:
    print("only odd values for grid_size are accepted")
    help()
if conf_blocks < 1 and conf_blocks > 4:
    print("invalid block configuration. Valid configurations 1 -- 4.")
    help()

# hey, everything is ok. Let's start to do something.

if size_grid % 2 == 0:
    aggiungi = 0
else:
    aggiungi = 1


matrix = [["free" for i in range(rows)] for j in range(size_grid)]


if conf_blocks == 1:
    # only square 1x1
    max_number_squares = int(size_grid / 2) * rows
    put_squares(random.randint(1, max_number_squares))


if conf_blocks == 2:
    # only 2x1
    max_number_straights = int(size_grid / 2) * int(rows / 2)
    # random... ??
    real_number_straights = random.randint(1, max_number_straights)
    straights = [i for i in range(real_number_straights)]
    posizionati = 0
    while posizionati < real_number_straights:
        x = random.randint(0, (size_grid / 2))
        y = random.randint(0, rows - 1)
        if matrix[x][y] == "free":
            direction = random.randint(1, 4)
            if direction == 1:
                # up
                if x != 0:
                    if matrix[x - 1][y] == "free":
                        matrix[x][y] = "straight" + str(straights[posizionati])
                        matrix[x - 1][y] = "straight" + str(straights[posizionati])
                        to_stamp_straight = to_stamp_straight + matrix[x][y] + " "
                        to_stamp_init = (
                            to_stamp_init
                            + "(at_two straight"
                            + str(straights[posizionati])
                            + " f"
                            + str(x)
                            + "-"
                            + str(y)
                            + "f f"
                            + str(x - 1)
                            + "-"
                            + str(y)
                            + "f)"
                            + "\n"
                        )
                        posizionati = posizionati + 1
                else:
                    direction = 2
            if direction == 2:
                # right
                if y != (rows - 1):
                    if matrix[x][y + 1] == "free":
                        matrix[x][y] = "straight" + str(straights[posizionati])
                        matrix[x][y + 1] = "straight" + str(straights[posizionati])
                        to_stamp_straight = to_stamp_straight + matrix[x][y] + " "
                        to_stamp_init = (
                            to_stamp_init
                            + "(at_two straight"
                            + str(straights[posizionati])
                            + " f"
                            + str(x)
                            + "-"
                            + str(y)
                            + "f f"
                            + str(x)
                            + "-"
                            + str(y + 1)
                            + "f)"
                            + "\n"
                        )
                        posizionati = posizionati + 1
                else:
                    direction = 4
            if direction == 3:
                # down
                if matrix[x + 1][y] == "free":
                    matrix[x][y] = "straight" + str(straights[posizionati])
                    matrix[x + 1][y] = "straight" + str(straights[posizionati])
                    to_stamp_straight = to_stamp_straight + matrix[x][y] + " "
                    to_stamp_init = (
                        to_stamp_init
                        + "(at_two straight"
                        + str(straights[posizionati])
                        + " f"
                        + str(x)
                        + "-"
                        + str(y)
                        + "f f"
                        + str(x + 1)
                        + "-"
                        + str(y)
                        + "f)"
                        + "\n"
                    )
                    posizionati = posizionati + 1
            if direction == 4:
                # left
                if y != 0:
                    if matrix[x][y - 1] == "free":
                        matrix[x][y] = "straight" + str(straights[posizionati])
                        matrix[x][y - 1] = "straight" + str(straights[posizionati])
                        to_stamp_straight = to_stamp_straight + matrix[x][y] + " "
                        to_stamp_init = (
                            to_stamp_init
                            + "(at_two straight"
                            + str(straights[posizionati])
                            + " f"
                            + str(x)
                            + "-"
                            + str(y)
                            + "f f"
                            + str(x)
                            + "-"
                            + str(y - 1)
                            + "f)"
                            + "\n"
                        )
                        posizionati = posizionati + 1

if conf_blocks == 3:
    # only L
    max_number_ls = int(size_grid / 4) * int(rows / 2)
    # random... ??
    real_number_l_right = random.randint(1, max_number_ls)
    l_rights = [i for i in range(real_number_l_right)]
    posizionati = 0
    while posizionati < real_number_l_right:
        x = random.randint(0, (size_grid / 2) - 1)
        y = random.randint(0, rows - 2)
        if (
            matrix[x][y] == "free"
            and matrix[x + 1][y + 1] == "free"
            and matrix[x + 1][y] == "free"
        ):
            matrix[x][y] = "rightl" + str(l_rights[posizionati])
            matrix[x + 1][y] = "rightl" + str(l_rights[posizionati])
            matrix[x + 1][y + 1] = "rightl" + str(l_rights[posizionati])
            to_stamp_L_right = to_stamp_L_right + matrix[x][y] + " "
            to_stamp_init = (
                to_stamp_init
                + "(at_right_l "
                + matrix[x][y]
                + " f"
                + str(x)
                + "-"
                + str(y)
                + "f f"
                + str(x + 1)
                + "-"
                + str(y)
                + "f f"
                + str(x + 1)
                + "-"
                + str(y + 1)
                + "f)"
                + "\n"
            )
            posizionati = posizionati + 1
if conf_blocks == 4:
    max_number_ls = int(size_grid / 4) * int(rows / 2)
    real_number_l_right = random.randint(1, max_number_ls)
    l_rights = [i for i in range(real_number_l_right)]
    posizionati = 0
    while posizionati < real_number_l_right:
        x = random.randint(0, (size_grid / 2) - 1)
        y = random.randint(0, rows - 2)
        if (
            matrix[x][y] == "free"
            and matrix[x + 1][y + 1] == "free"
            and matrix[x + 1][y] == "free"
        ):
            matrix[x][y] = "rightl" + str(l_rights[posizionati])
            matrix[x + 1][y] = "rightl" + str(l_rights[posizionati])
            matrix[x + 1][y + 1] = "rightl" + str(l_rights[posizionati])
            to_stamp_L_right = to_stamp_L_right + matrix[x][y] + " "
            to_stamp_init = (
                to_stamp_init
                + "(at_right_l "
                + matrix[x][y]
                + " f"
                + str(x)
                + "-"
                + str(y)
                + "f f"
                + str(x + 1)
                + "-"
                + str(y)
                + "f f"
                + str(x + 1)
                + "-"
                + str(y + 1)
                + "f)"
                + "\n"
            )
            posizionati = posizionati + 1
    # straights
    linea = 1
    posizionati = 0
    while linea < size_grid / 2:
        colonna = 0
        while colonna < rows - 1:
            if (
                matrix[linea][colonna] == "free"
                and matrix[linea - 1][colonna] == "free"
            ):
                if random.randint(1, 4) > 2:
                    matrix[linea][colonna] = "straight" + str(posizionati)
                    matrix[linea - 1][colonna] = "straight" + str(posizionati)
                    to_stamp_straight = to_stamp_straight + matrix[linea][colonna] + " "
                    to_stamp_init = (
                        to_stamp_init
                        + "(at_two straight"
                        + str(posizionati)
                        + " f"
                        + str(linea - 1)
                        + "-"
                        + str(colonna)
                        + "f f"
                        + str(linea)
                        + "-"
                        + str(colonna)
                        + "f)"
                        + "\n"
                    )
                    posizionati = posizionati + 1
            colonna = colonna + 1
        linea = linea + 1
    # squares
    linea = 0
    posizionati = 0
    while linea < size_grid / 2:
        colonna = 0
        while colonna < rows - 1:
            if matrix[linea][colonna] == "free":
                if random.randint(1, 4) > 2:
                    matrix[linea][colonna] = "square" + str(posizionati)
                    to_stamp_square = to_stamp_square + matrix[linea][colonna] + " "
                    to_stamp_init = (
                        to_stamp_init
                        + "(at_square square"
                        + str(posizionati)
                        + " f"
                        + str(linea)
                        + "-"
                        + str(colonna)
                        + "f)"
                        + "\n"
                    )
                    posizionati = posizionati + 1
            colonna = colonna + 1
        linea = linea + 1

    # mix up!
# Finally, time for printing everything out.

print(
    "(define (problem Tetris-"
    + str(size_grid)
    + "-"
    + str(conf_blocks)
    + "-"
    + str(random.randint(0, 9875232))
    + ")"
)
print("(:domain tetris)")
print("(:objects ", end=" ")

for i in range(size_grid):
    print("")
    for j in range(rows):
        x = "f" + str(i) + "-" + str(j) + "f"
        print(x, end=" ")
print("- position")

# eventually, this be commented away. **

if to_stamp_square == "":
    to_stamp_square = "nothing"
if to_stamp_straight == "":
    to_stamp_straight = "nada"
if to_stamp_L_right == "":
    to_stamp_L_right = "nisba"


# until here **

print(to_stamp_square + "- one_square")
print(to_stamp_straight + "- two_straight")
print(to_stamp_L_right + "- right_l")


print(")\n(:init")

# connected cells on the same line
for i in range(size_grid):
    for j in range(rows - 1):
        print(
            "(connected f"
            + str(i)
            + "-"
            + str(j)
            + "f f"
            + str(i)
            + "-"
            + str(j + 1)
            + "f)"
        )
        print(
            "(connected f"
            + str(i)
            + "-"
            + str(j + 1)
            + "f f"
            + str(i)
            + "-"
            + str(j)
            + "f)"
        )

# connected cells on the same column
for i in range(size_grid - 1):
    for j in range(rows):
        print(
            "(connected f"
            + str(i)
            + "-"
            + str(j)
            + "f f"
            + str(i + 1)
            + "-"
            + str(j)
            + "f)"
        )
        print(
            "(connected f"
            + str(i + 1)
            + "-"
            + str(j)
            + "f f"
            + str(i)
            + "-"
            + str(j)
            + "f)"
        )


# free cells
for i in range(size_grid):
    for j in range(rows):
        if matrix[i][j] == "free":
            print("(clear f" + str(i) + "-" + str(j) + "f)")

# occupied cells
print(to_stamp_init, end=" ")
print(")\n(:goal\n(and")

for i in range(0, int(size_grid / 2) + aggiungi):
    for j in range(rows):
        print("(clear f" + str(i) + "-" + str(j) + "f)")

print(")\n)\n)")


print(";; DESCRIPTION OF THE INITIAL STATE")
for i in range(size_grid):
    print(";; " + str(i) + " ", end=" ")
    for j in range(rows):
        if matrix[i][j] == "free":
            print(" **", end=" ")
        else:
            print(" ##", end=" ")
    print("")
