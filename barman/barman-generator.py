#!/usr/bin/env python

import random
import sys

MAX_LEVELS = 2


def print_usage():
    print(
        (
            "Usage: "
            + sys.argv[0]
            + " <num_cocktails> <num_ingredients> <num_shots> [<random_seed>]"
        )
    )
    print("  num_cocktails (min 1)")
    print("  num_ingredients (min 2)")
    print("  num_shots (min max[1,num_cocktails+1])")
    print("  random_seed (min 1, optional)")


if len(sys.argv) not in [4, 5]:
    print_usage()
    sys.exit(2)

num_cocktails = int(sys.argv[1])
num_ingredients = int(sys.argv[2])
num_shots = int(sys.argv[3])

if len(sys.argv) == 5:
    seed = int(sys.argv[4])
else:
    seed = None


def get_objects():
    str_objects = "\n"

    # -- shaker
    str_objects = str_objects + "      shaker1 - shaker\n"

    # -- robot hands
    str_objects = str_objects + "      left right - hand\n     "

    # -- shots
    for i in range(1, num_shots + 1):
        str_objects = str_objects + " shot" + str(i)
    str_objects = str_objects + " - shot\n     "

    # -- ingredients
    for i in range(1, num_ingredients + 1):
        str_objects = str_objects + " ingredient" + str(i)
    str_objects = str_objects + " - ingredient\n     "

    # -- mixs
    for i in range(1, num_cocktails + 1):
        str_objects = str_objects + " cocktail" + str(i)
    str_objects = str_objects + " - cocktail\n     "

    # -- dispensers
    for i in range(1, num_ingredients + 1):
        str_objects = str_objects + " dispenser" + str(i)
    str_objects = str_objects + " - dispenser\n     "

    # -- levels
    for i in range(MAX_LEVELS + 1):
        str_objects = str_objects + " l" + str(i)
    str_objects = str_objects + " - level\n"

    return str_objects


def get_init():
    str_init = "\n"
    str_init = str_init + "  (ontable shaker1)\n"

    for i in range(1, num_shots + 1):
        str_init = str_init + "  (ontable shot" + str(i) + ")\n"

    for i in range(1, num_ingredients + 1):
        str_init = (
            str_init
            + "  (dispenses dispenser"
            + str(i)
            + " ingredient"
            + str(i)
            + ")\n"
        )

    str_init = str_init + "  (clean shaker1)\n"

    for i in range(1, num_shots + 1):
        str_init = str_init + "  (clean shot" + str(i) + ")\n"

    str_init = str_init + "  (empty shaker1)\n"

    for i in range(1, num_shots + 1):
        str_init = str_init + "  (empty shot" + str(i) + ")\n"

    str_init = str_init + "  (handempty left)\n  (handempty right)\n"

    str_init = str_init + "  (shaker-empty-level shaker1 l0)\n"
    str_init = str_init + "  (shaker-level shaker1 l0)\n"
    for i in range(MAX_LEVELS):
        str_init = str_init + "  (next l" + str(i) + " l" + str(i + 1) + ")\n"

    for i in range(1, num_cocktails + 1):
        parts = random.sample(list(range(1, num_ingredients + 1)), 2)
        str_init = (
            str_init
            + "  (cocktail-part1 cocktail"
            + str(i)
            + " ingredient"
            + str(parts[0])
            + ")\n"
        )
        str_init = (
            str_init
            + "  (cocktail-part2 cocktail"
            + str(i)
            + " ingredient"
            + str(parts[1])
            + ")\n"
        )
    return str_init


def get_goals():
    str_goal = ""
    str_goal = str_goal + "\n  (and\n"

    serving = random.sample(list(range(1, num_cocktails + 1)), num_cocktails)
    for i in range(1, num_cocktails + 1):
        str_goal = (
            str_goal
            + "      (contains shot"
            + str(i)
            + " cocktail"
            + str(serving[i - 1])
            + ")\n"
        )

    # there is at least one shot for not serving
    for j in range(i + 1, num_shots):
        flip = random.randint(0, 1)
        if flip == 1:
            str_goal = (
                str_goal
                + "      (contains shot"
                + str(j)
                + " cocktail"
                + str(random.randint(1, num_cocktails))
                + ")\n"
            )
        else:
            str_goal = (
                str_goal
                + "      (contains shot"
                + str(j)
                + " ingredient"
                + str(random.randint(1, num_ingredients))
                + ")\n"
            )

    str_goal = str_goal + ")"
    return str_goal


def main():
    name = "prob"

    random.seed(seed)

    print(("(define (problem " + name + ")"))
    print(" (:domain barman)")
    print((" (:objects " + get_objects() + ")"))
    print((" (:init " + get_init() + ")"))
    print((" (:goal" + get_goals() + "))"))


main()
