#!/usr/bin/env python

import argparse
import random
import itertools

parser = argparse.ArgumentParser()

parser.add_argument("balls", type=int, help="the number of balls")
parser.add_argument("robots", type=int, help="the number of robots")
parser.add_argument("grippers", type=int, help="the number of grippers per robot")
parser.add_argument("rooms", type=int, help="the number of rooms")
parser.add_argument("--seed", "-s", type=int, help="random seed")

args = parser.parse_args()

random.seed(args.seed)

robots = [f"robot{i}" for i in range(args.robots)]
rooms = [f"room{i}" for i in range(args.rooms)]
grippers = [f"gripper-{i}-{j}" for i,j in itertools.product(range(args.robots),range(args.grippers))]
balls = [f"ball{i}" for i in range(args.balls)]



def init():
    facts = [f"(has robot{i} gripper-{i}-{j}) (free gripper-{i}-{j})"
             for i,j in itertools.product(range(args.robots),range(args.grippers))]

    for ball in balls:
        room = random.choice(rooms)
        facts.append(f"(at-ball {ball} {room})")
    for robot in robots:
        room = random.choice(rooms)
        facts.append(f"(at-robot {robot} {room})")

    return " ".join(facts)


def goal():
    facts = []

    for ball in balls:
        room = random.choice(rooms)
        facts.append(f"(at-ball {ball} {room})")

    return " ".join(facts)


print("(define (problem gripper)")
print(" (:domain gripper-ex-typed)")
print(" (:objects "
      + " ".join(
          robots   + ["-", "robot"] +
          rooms    + ["-", "room"] +
          grippers + ["-", "gripper"] +
          balls    + ["-", "ball"])
      + ")")
print(" (:init " + init() + ")")
print(" (:goal " + goal() + "))")
