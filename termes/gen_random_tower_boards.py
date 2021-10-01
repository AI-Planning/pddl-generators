#! /usr/bin/env python

import numpy
import argparse


def gen_board(size_x, size_y, height, num_towers, seed):
    numpy.random.seed(seed)

    board = [[0 for x in range(size_x)] for y in range(size_y)]

    cells = [(x, y) for x in range(size_x) for y in range(size_y)]

    first = True
    for i in numpy.random.choice(range(len(cells)), num_towers, replace=False):
        (x, y) = cells[i]
        col_height = height if first else numpy.random.choice(range(2, height + 1))
        first = False
        board[y][x] = col_height

    board_name = "random_towers_{}x{}_{}_{}_{}.txt".format(
        size_x, size_y, height, num_towers, seed
    )
    f = open("boards/" + board_name, "w")
    for row in board:
        # print (" ".join(map(str, row)))
        f.write(" ".join(map(str, row)) + "\n")
    f.close()

    return board_name


# Board generation procedure that has more control than the one used for IPC'18 about the
# number of blocks in the resulting towers. We introduce an extra parameter, num_blocks.
# The total number_of blocks will be (num_towers-1)*min_height + max_height +
# extra_blocks. The reasoning is that we will have a tower of max_height, the rest of the
# towers with 1 block, and then the extra_blocks are assigned randomly among the other
# towers.
def gen_board_autoscale(size_x, size_y, min_height, max_height, num_towers, rng):

    board = [[0 for x in range(size_x)] for y in range(size_y)]

    cells = [(x, y) for x in range(size_x) for y in range(size_y)]

    first = True
    for i in rng.choice(range(len(cells)), num_towers, replace=False):
        (x, y) = cells[i]
        col_height = (
            max_height if first else int(rng.choice(range(min_height, max_height + 1)))
        )
        first = False
        board[y][x] = col_height

    # board_name = 'random_towers_{}x{}_{}_{}_{}.txt'.format(size_x, size_y, min_height, max_height, num_towers, seed)

    return board
    # f = open('boards/' + board_name, 'w')
    # for row in board:
    #     #print (" ".join(map(str, row)))
    #     f.write(" ".join(map(str, row)) + '\n')
    # f.close()

    # return board_name


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--size_x", type=int, default=4)
    parser.add_argument("--size_y", type=int, default=4)
    parser.add_argument("--height", type=int, default=4)
    parser.add_argument("--num_towers", type=int, default=4)
    parser.add_argument("--seed", type=int, default=0)

    args = parser.parse_args()

    gen_board(args.size_x, args.size_y, args.height, args.num_towers, args.seed)


if __name__ == "__main__":
    main()
