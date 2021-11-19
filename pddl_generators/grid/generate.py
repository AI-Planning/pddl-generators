#!/usr/bin/env python3

import argparse
from itertools import product
import random
import itertools

from collections import defaultdict
import sys


def parse():
    parser = argparse.ArgumentParser()

    parser.add_argument("x", type=int, help="x scale (minimal 2)")
    parser.add_argument("y", type=int, help="y scale (minimal 2)")
    parser.add_argument(
        "--shapes", type=int, default=2, help="num different key+lock types"
    )
    parser.add_argument("--keys", type=int, default=2, help="number keys vector")
    parser.add_argument("--locks", type=int, default=2, help="number locks vector")
    parser.add_argument(
        "--prob-goal",
        type=float,
        default=1.0,
        help="probability of any key being mentioned in the goal",
    )

    parser.add_argument("--typed", action="store_true", help="Typed domain")

    parser.add_argument("--seed", type=int, default=1, help="random seed")

    parser.add_argument("--output", default=None)

    return parser.parse_args()


def backslash_join(x):
    return "\n       ".join(x)


def adjacent_positions(x, y, positions):
    return [
        adj_pos
        for adj_pos in [(x + 1, y), (x, y + 1), (x - 1, y), (x, y - 1)]
        if adj_pos in positions
    ]


def pos_name(p):
    return f"pos{p[0]}-{p[1]}"


def main():
    args = parse()
    random.seed(args.seed)

    if args.output:
        sys.stdout = open(f"{args.output}/{instance_name}.pddl", "w")

    positions = [(x, y) for x in range(args.x) for y in range(args.y)]
    assert args.locks < len(positions), "There cannot be more locks than positions"
    assert args.shapes <= args.keys, "There cannot be more shapes than keys"
    assert args.shapes <= args.locks, "There cannot be more shapes than locks"
    assert args.prob_goal <= 1.0, "Goal probability cannot be greater than 1.0"
    assert args.prob_goal > 0.0, "Goal probability cannot be 0.0"

    locked_positions = random.sample([p for p in positions], k=args.locks)
    open_positions = [p for p in positions if p not in locked_positions]
    robot_pos = random.choice(open_positions)

    instance_name = f"grid-{args.x}-{args.y}-{args.shapes}-{args.keys}-{args.locks}"
    nodes = [pos_name(p) for p in positions]
    keys = [f"key{k}" for k in range(args.keys)]
    shapes = [f"shape{k}" for k in range(args.shapes)]

    key_shapes = [
        shapes[k] if k < len(shapes) else random.choice(shapes)
        for k in range(args.keys)
    ]
    locked_shapes = [
        shapes[k] if k < len(shapes) else random.choice(shapes)
        for k in range(len(locked_positions))
    ]

    locked_pos_to_shape = {}
    for i, pos in enumerate(locked_positions):
        locked_pos_to_shape[pos] = locked_shapes[i]

    shape_to_keys = defaultdict(list)
    for i, key in enumerate(keys):
        shape_to_keys[key_shapes[i]].append(key)

    key_positions = {}

    i = 0
    reachable_locked = set()
    reachable_locations = [robot_pos]
    unlocked_positions = [p for p in open_positions]

    while len(reachable_locations) < len(positions):
        while i < len(reachable_locations):
            loc = reachable_locations[i]
            adj = adjacent_positions(loc[0], loc[1], positions)
            for pos in adj:
                if pos in reachable_locations:
                    continue

                if pos in unlocked_positions:
                    reachable_locations.append(pos)
                else:
                    reachable_locked.add(pos)
            i += 1

        if len(reachable_locked) > 0:
            pos = random.choice(list(reachable_locked))
            shape = locked_pos_to_shape[pos]

            all_keys_shape = shape_to_keys[shape]
            sure_key = random.choice(all_keys_shape)
            for key in all_keys_shape:
                assert not key in key_positions
                key_positions[key] = (
                    random.choice(reachable_locations)
                    if key == sure_key
                    else random.choice(positions)
                )

            for pos in positions:
                if pos in locked_pos_to_shape and locked_pos_to_shape[pos] == shape:
                    unlocked_positions.append(pos)

            to_remove = []
            for r in reachable_locked:
                if locked_pos_to_shape[r] == shape:
                    to_remove.append(r)
                    reachable_locations.append(r)

            for r in to_remove:
                reachable_locked.remove(r)

    if args.typed:
        place_facts = []
        shape_facts = []
        key_facts = []
    else:
        place_facts = [f"(place {pos_name(pos)})" for pos in positions]
        shape_facts = [f"(shape {shape})" for shape in shapes]
        key_facts = [f"(key {key})" for key in keys]

    conn_facts = [
        f"(conn {pos_name(p1)} {pos_name(p2)})"
        for p1 in positions
        for p2 in adjacent_positions(p1[0], p1[1], positions)
    ]

    key_shape_facts = [
        f"(key-shape  {keys[i]} {key_shapes[i]})" for i in range(args.keys)
    ]

    open_facts = [f"(open {pos_name(pos)})" for pos in open_positions]
    locked_facts = [f"(locked {pos_name(pos)})" for pos in locked_positions]
    lock_shape_facts = [
        f"(lock-shape  {pos_name(locked_positions[i])} {locked_shapes[i]})"
        for i in range(len(locked_positions))
    ]

    key_at_facts = [f"(at {key} {pos_name(key_positions[key])})" for key in keys]

    at_robot_fact = f"(at-robot {pos_name(robot_pos)})"

    goal_facts = []
    for k in range(args.keys):
        if random.random() < args.prob_goal:
            p = random.choice([p for p in positions if p != key_positions[keys[k]]])
            goal_facts.append(f"(at {keys[k]} {pos_name(p)})")

    if not goal_facts:
        k = random.choice(range(args.keys))
        p = random.choice([p for p in positions if p != key_positions[keys[k]]])
        goal_facts.append(f"(at {keys[k]} {pos_name(p)})")

    print(
        f"""(define (problem {instance_name})
    (:domain grid)
    (:objects
    {" ".join(nodes)}
    {" ".join(shapes)}
    {" ".join(keys)}
    )

    (:init
       (arm-empty)

       {backslash_join(place_facts)}
       {backslash_join(shape_facts)}
       {backslash_join(key_facts)}

       {backslash_join(conn_facts)}
       {backslash_join(locked_facts)}
       {backslash_join(lock_shape_facts)}
       {backslash_join(open_facts)}

       {backslash_join(key_shape_facts)}
       {backslash_join(key_at_facts)}
       {at_robot_fact}
    )

    (:goal (and
       {backslash_join(goal_facts)}
    )))
    """
    )


if __name__ == "__main__":
    main()
