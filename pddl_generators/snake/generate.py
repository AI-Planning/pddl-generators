#!/usr/bin/env python

import argparse
from itertools import product
import random

import sys


# Board notation:
#
class Board:
    def __init__(self, board_name):
        if board_name.startswith("empty"):
            width, height = (
                board_name.replace("empty-", "").replace("empty", "").split("x")
            )
            self.board = [
                ["_" for _ in range(0, int(height))] for _ in range(0, int(width))
            ]
        else:
            f = open(board_name)
            self.board = [[a for a in l if a != "\n"] for l in f.readlines()]
            f.close()

    def __repr__(self):
        return "\n".join(map(lambda x: "".join(x), self.board))

    def width(self):
        return len(self.board)

    def height(self):
        return len(self.board[0])

    def num_clear_positions(self):
        positions = [
            (i, j)
            for (i, j) in product(range(self.width()), range(self.height()))
            if self.board[i][j] == "_"
        ]

        return len(positions)

    def random_clear_positions(self):
        positions = [
            (i, j)
            for (i, j) in product(range(self.width()), range(self.height()))
            if self.board[i][j] == "_"
        ]
        random.shuffle(positions)
        return positions

    def blocked_positions(self):
        positions = [
            (i, j)
            for (i, j) in product(range(self.width()), range(self.height()))
            if self.board[i][j] == "S"
            or self.board[i][j] == "H"
            or self.board[i][j] == "*"
        ]
        return positions

    def generate_apples(self, num_apples, mark="A"):
        # apples = [(i, j) for (i, j) in product(range(self.width()), range(self.height())) if self.board[i][j] == "A"]
        new_apples = self.random_clear_positions()[0:num_apples]
        for (x, y) in new_apples:
            self.board[x][y] = mark
        return new_apples

    def is_empty(self, x, y):
        if x < 0 or x >= self.width():
            return False
        if y < 0 or y >= self.height():
            return False

        return self.board[x][y] == "_"

    def generate_snake_aux(self, x, y, snake_size, snake):

        if not self.is_empty(x, y):
            return False

        self.board[x][y] = "S"

        if snake_size == 0:
            snake += [(x, y)]
            return True

        pos_list = [(x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)]
        random.shuffle(pos_list)
        for pos in pos_list:
            if self.generate_snake_aux(pos[0], pos[1], snake_size - 1, snake):
                snake += [(x, y)]
                return True
        self.board[auxx][auxy] = "_"
        return False

    def generate_snake(self, snake_size):
        snake = []
        for (x, y) in self.random_clear_positions():
            if self.generate_snake_aux(x, y, snake_size, snake):
                self.board[x][y] = "H"
                return snake[::-1]

            # next_pos = []
            # if auxx > 0 and auxx <= x  and auxy == y:
            #     auxx -= 1
            # elif auxy >= y and auxy <= self.height() - 1  and auxx == self.width() - 1:
            #     auxy += 1

            # elif auxy <= y:
            #     if auxx == 0 and auxy > 0:
            #         auxy -= 1
            #     elif auxy % 2 == 0 and auxx <= self.width() - 2:
            #         auxx += 1
            #     elif auxy % 2 == 1 and auxx > 1:
            #         auxx -= 1
            #     else:
            #         auxy += 1
            # else:
            #     if auxy % 2 == self.height() % 2 and auxx > 0:
            #         auxx -= 1
            #     elif auxy % 2 != self.height() % 2 and auxx < self.width()  -2:
            #         auxx += 1
            #     else:
            #         auxy -= 1
            # assert (not (auxx, auxy) in snake)
            # snake.append((auxx, auxy))
            # self.board[auxx][auxy] = 'S'
        return snake


def print_list(l):
    print(" ".join([str(len(l))] + [f"{a} {b}" for (a, b) in l]))


def print_solver(instance_name, board, snake, apples, spawn_apples):
    print(f"{board.width()} {board.height()}")
    # print_list (board.blocked_positions())
    print_list(snake)
    print_list(apples)
    print_list(spawn_apples)


def print_readable(instance_name, board, snake, apples, spawn_apples):
    print(instance_name)
    print("Blocked: ", board.blocked_positions())
    print("Snake: ", snake)
    print("Apples: ", apples)
    print("Spawn: ", spawn_apples)


def get_adjacent_pairs(width, height):
    res = []
    for x in range(0, width):
        for y in range(0, height):
            if x < width - 1:
                res.append(((x, y), (x + 1, y)))
            if y < height - 1:
                res.append(((x, y), (x, y + 1)))
            if x > 0:
                res.append(((x, y), (x - 1, y)))
            if y > 0:
                res.append(((x, y), (x, y - 1)))
    return res


def pos_name(p):
    return "pos{}-{}".format(*p)


def print_pddl(instance_name, board, snake, apples, spawn_apples):
    positions = list(product(range(board.width()), range(board.height())))
    pos_objects = " ".join([pos_name(pos) for pos in positions])
    is_adjacent_facts = "\n ".join(
        [
            f"(isAdjacent {pos_name(adj1)} {pos_name(adj2)})"
            for (adj1, adj2) in get_adjacent_pairs(board.width(), board.height())
        ]
    )

    head_snake = f"(headSnake {pos_name(snake[0])})"
    tail_snake = f"(tailSnake {pos_name(snake[-1])})"

    next_snake_facts = "\n ".join(
        [
            f"(nextSnake {pos_name(snake[i])} {pos_name(snake[i + 1])})"
            for i in range(0, len(snake) - 1)
        ]
    )
    blocked_facts = "\n ".join(
        [f"(blocked {pos_name(sn)})" for sn in board.blocked_positions()]
    )

    is_point_facts = "\n ".join([f"(isPoint {pos_name(a)})" for a in apples])

    spawn_apples_facts = []
    if spawn_apples:
        spawn_apples_facts = [f"(spawn {pos_name(spawn_apples[0])})"] + [
            f"(nextSpawn {pos_name(spawn_apples[-1])} dummyPoint)"
        ]
        if len(spawn_apples) > 1:
            spawn_apples_facts += [
                "(nextSpawn {} {})".format(
                    pos_name(spawn_apples[i]), pos_name(spawn_apples[i + 1])
                )
                for i in range(0, len(spawn_apples) - 1)
            ]
    else:
        spawn_apples_facts = ["(spawn dummyPoint)"]

    next_point_facts = "\n ".join(spawn_apples_facts)

    goal_facts = "\n ".join(
        [f"(not (isPoint {pos_name(a)}))" for a in apples + spawn_apples]
    )

    print(
        """(define (problem {instance_name})
(:domain snake)
(:objects
    {pos_objects}
)
(:init
    {is_adjacent_facts}
    {tail_snake}
    {head_snake}
    {next_snake_facts}
    {blocked_facts}
    {next_point_facts}
    {is_point_facts}
)
(:goal (and
    {goal_facts}
))
)
""".format(
            **locals()
        )
    )


def parse():
    parser = argparse.ArgumentParser()
    parser.add_argument("board", help="What kind of board.")
    parser.add_argument("snake_size", type=int, help="Initial size of the snake.")
    parser.add_argument(
        "num_initial_apples",
        type=int,
        help="Number of apples that are present at the beginning.",
    )
    parser.add_argument(
        "num_spawn_apples", help="Number of apples that will spawn at random locations."
    )
    parser.add_argument("random_seed", type=int)
    parser.add_argument("output_type", choices=["pddl", "raw", "readable", "draw"])

    parser.add_argument("--output", default=None)

    return parser.parse_args()


def main():
    args = parse()
    board_name = args.board.replace(".txt", "")

    random.seed(args.random_seed)
    board = Board(args.board)

    if "/" in board_name:
        board_name = board_name.split("/")[-1]

    num_initial_apples = args.num_initial_apples
    if "%" in args.num_spawn_apples:
        percentage = int(args.num_spawn_apples[:-1]) / 100.0
        num_spawn_apples = (
            int(board.num_clear_positions() * percentage)
            - 1
            - args.snake_size
            - args.num_initial_apples
        )
        if num_spawn_apples < 1:  # At least one apple should be spawn
            num_initial_apples += num_spawn_apples - 1
            num_spawn_apples = 1

        # There must be at least one apple
        if num_initial_apples < 1:
            num_initial_apples = 1
    else:
        num_spawn_apples = int(args.num_spawn_apples)

    if (
        board.num_clear_positions() % 2 == 1
        and num_spawn_apples
        == int(board.num_clear_positions() * percentage)
        - 1
        - args.snake_size
        - args.num_initial_apples
    ):
        num_spawn_apples -= 1

    instance_name = "snake-%s-%s-%s-%d-%s" % (
        board_name,
        args.snake_size,
        num_initial_apples,
        num_spawn_apples,
        args.random_seed,
    )

    assert (
        board.num_clear_positions()
        - args.snake_size
        - num_initial_apples
        - num_spawn_apples
        > 0
    ), instance_name

    snake = board.generate_snake(args.snake_size)
    apples = board.generate_apples(num_initial_apples)
    spawn_apples = board.generate_apples(num_spawn_apples, "B")

    # Not repeated
    assert len(apples) == len(set(apples))
    assert len(spawn_apples) == len(set(spawn_apples))
    assert not set(apples) & set(spawn_apples)

    assert len(apples) == num_initial_apples
    assert len(spawn_apples) == num_spawn_apples, instance_name
    assert len(snake) == args.snake_size + 1

    if args.output:
        sys.stdout = open(f"{args.output}/{instance_name}.pddl", "w")
    if args.output_type == "raw":
        print_solver(instance_name, board, snake, apples, spawn_apples)
    elif args.output_type == "readable":
        print_readable(instance_name, board, snake, apples, spawn_apples)
    elif args.output_type == "pddl":
        print_pddl(instance_name, board, snake, apples, spawn_apples)
    elif args.output_type == "draw":
        print(board)


if __name__ == "__main__":
    main()
