#!/usr/bin/env python

import random
import itertools

# Generate all combinations from empty to each board.
# Usage ./generate-ipc-instances.py | bash


def get_slack(board):
    if board.startswith("empire"):
        return 1
    elif board.startswith("tower") and not "4" in board:
        return 2
    return 0


def get_height(board):
    if board.startswith("empire"):
        return 1
    elif board.startswith("tower") and not "4" in board:
        return 2
    return 0


random.seed(10)
# Map from board name to slack and max height reduction
# all_boards = {'eiffel.txt' : (2),
#               'empire.txt' : (6),
#               'giza.txt' : (6),
#               'taj_mahal.txt' : (5),
#               'tower.txt' : (7),
#               'tower2.txt': (7) ,
#               'tower4.txt': (7),
#               'wall5x5.txt' : (6)}

all_boards = {
    "eiffel.txt": range(0, 3),
    "empire.txt": range(2, 7),
}


scenarios = [
    ("empty.txt", goal, 0, gh)
    for (goal, range_gh) in all_boards.items()
    for gh in range_gh
]


# + \
# [(init, goal, gh, gh) for ((init, (max_gh1)), (goal, (max_gh2))) in random.sample(sorted(set(itertools.permutations(all_boards.items(), 2))), 10)
#  for gh in range (0, min(max_gh1, max_gh2))
#  ]


for sc in scenarios:
    print(
        "./generate.py boards/{} boards/{} pddlfile --reduce_height_init {} --reduce_height_goal {} --ensure_plan --store_plan".format(
            *sc
        )
    )
