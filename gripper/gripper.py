#!/usr/bin/env python

import argparse
import random

parser = argparse.ArgumentParser()

parser.add_argument("balls", type=int, help="the number of balls")
parser.add_argument("--seed", "-s", type=int, help="random seed")
parser.add_argument("--randomize", "-r", action="store_true", help="randomize the initial state and the goal if present")

args = parser.parse_args()

random.seed(args.seed)

def init():
    # static
    facts = ["(room rooma)","(room roomb)", "(gripper left)", "(gripper right)"]
    for i in range(args.balls):
        facts.append(f"(ball ball{i})")

    # fluents
    facts.extend(["(free left)", "(free right)"])

    if args.randomize:
        for i in range(args.balls):
            room = random.choice(["rooma","roomb"])
            facts.append(f"(at ball{i} {room})")
        room = random.choice(["rooma","roomb"])
        facts.append(f"(at-robby {room})")
    else:
        for i in range(args.balls):
            facts.append(f"(at ball{i} rooma)")
        facts.append(f"(at-robby rooma)")

    return " ".join(facts)


def goal():
    facts = []
    if args.randomize:
        for i in range(args.balls):
            room = random.choice(["rooma","roomb"])
            facts.append(f"(at ball{i} {room})")
    else:
        for i in range(args.balls):
            facts.append(f"(at ball{i} roomb)")

    return " ".join(facts)


print("(define (problem gripper)")
print(" (:domain gripper-strips)")
print(" (:objects rooma roomb left right " + " ".join([f"ball{i}" for i in range(args.balls)]) + ")")
print(" (:init " + init() + ")")
print(" (:goal (and " + goal() + ")))")
