#!/usr/bin/env python

import random
import itertools
import gen_random_tower_boards

# Generate all combinations from empty to each board.
# Usage ./generate-ipc-instances.py | bash

seed = 1
# Optimal
(size_x, size_y) = (4, 3)
for range_num_towers, range_height in [
    ([1, 2], range(3, 8)),
    ([3, 4], range(3, 6)),
    ([5, 6], range(3, 5)),
]:
    for height in range_height:
        for num_towers in range_num_towers:
            for i in range(4):
                scenario = gen_random_tower_boards.gen_board(
                    size_x, size_y, height, num_towers, seed
                )
                print(
                    "./generate.py boards/empty.txt boards/{} pddlfile --ensure_plan --store_plan --dont_remove_slack".format(
                        scenario
                    )
                )
                seed += 1
