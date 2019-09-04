#!/usr/bin/env python
import sys
import random


def get_tile(row, col):
    return "tile_" + str(row) + "-" + str(col)


# There is an extra row below to place robots at the end


def get_objects():
    #   TILES
    str_tiles = ""
    for row in range(num_rows + 1):
        for column in range(num_columns):
            str_tiles = str_tiles + get_tile(row, column + 1) + " "
        if row != num_rows:
            str_tiles += "\n           "
    str_tiles += "- tile\n"

    # ROBOTS
    str_robots = "           "
    for robot in range(num_robots):
        str_robots += "robot" + str(robot + 1) + " "
    str_robots += "- robot\n"

    # COLORS
    str_colors = "           white black - color\n"
    return str_tiles + str_robots + str_colors


def get_init():
    robot_locations = []
    str_init = "\n"
    if mode_flag != "time":
        str_init += "   (= (total-cost) 0)\n"

    used_cols = list(range(1, num_columns + 1))

    for robot in range(num_robots):
        posx = random.randint(0, num_rows)
        posy = random.choice(used_cols)
        robot_pos = get_tile(posx, posy)
        robot_locations += [robot_pos]
        used_cols.remove(posy)

        str_init += "   (robot-at robot" + str(robot + 1) + " " + robot_pos + ")\n"
        if robot % 2 == 0:
            robot_color = "white"
        else:
            robot_color = "black"
        str_init += "   (robot-has robot" + str(robot + 1) + " " + robot_color + ")\n"

    str_init += "   (available-color white)\n"
    str_init += "   (available-color black)\n"

    for row in range(num_rows + 1):
        for column in range(num_columns):
            if get_tile(row, column + 1) not in robot_locations:
                str_init += "   (clear " + get_tile(row, column + 1) + ")\n"

    for row in range(num_rows):
        for column in range(num_columns):
            str_init += (
                "   (up "
                + get_tile(row + 1, column + 1)
                + " "
                + get_tile(row, column + 1)
                + ")\n"
            )
    for row in range(num_rows):
        for column in range(num_columns):
            str_init += (
                "   (down "
                + get_tile(row, column + 1)
                + " "
                + get_tile(row + 1, column + 1)
                + ")\n"
            )
    for row in range(num_rows + 1):
        for column in range(num_columns - 1):
            str_init += (
                "   (right "
                + get_tile(row, column + 2)
                + " "
                + get_tile(row, column + 1)
                + ")\n"
            )
    for row in range(num_rows + 1):
        for column in range(num_columns - 1):
            str_init += (
                "   (left "
                + get_tile(row, column + 1)
                + " "
                + get_tile(row, column + 2)
                + ")\n"
            )

    return str_init


def get_goals():
    str_goals = "(and\n"
    for row in range(num_rows):
        for column in range(num_columns):
            if (row + column) % 2 == 0:
                str_goals += (
                    "    (painted " + get_tile(row + 1, column + 1) + " white)\n"
                )
            else:
                str_goals += (
                    "    (painted " + get_tile(row + 1, column + 1) + " black)\n"
                )
    return str_goals + ")"


# Reading the command line arguments
try:
    name = sys.argv[1]
    num_rows = int(sys.argv[2])
    num_columns = int(sys.argv[3])
    num_robots = int(sys.argv[4])
    mode_flag = sys.argv[5]

    if len(sys.argv) == 7:
        seed = int(sys.argv[6])
    else:
        seed = None

    random.seed(seed)

except BaseException:
    print(
        "Usage: "
        + sys.argv[0]
        + " <name> <num_rows> <num_columns> <num_robots> <mode_flag(seq|time)> [<seed>]"
    )
    sys.exit(1)

print(("(define (problem " + name + ")"))
print(" (:domain floor-tile)")
print((" (:objects " + get_objects() + ")"))
print((" (:init " + get_init() + ")"))
print((" (:goal " + get_goals() + ")"))

if mode_flag == "time":
    print(" (:metric minimize (total-time))")
else:
    print(" (:metric minimize (total-cost))")

print(")")

sys.exit(0)
