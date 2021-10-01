#!/usr/bin/env python3

import argparse
from itertools import product
import random
import itertools

import sys


def print_pddl(instance_name, num_floors, elevators, passengers, cost_slow, cost_fast):

    num_counts = max(num_floors, max(e.capacity + 1 for e in elevators))

    count_objects = [f"n{i}" for i in range(num_counts)]
    next_facts = "\n       ".join(
        [
            f"(next {count_objects[i]} {count_objects[i+1]})"
            for i in range(num_floors - 1)
        ]
    )
    above_facts = "\n       ".join(
        [
            f"(above {count_objects[i]} {count_objects[j]})"
            for i in range(num_floors - 1)
            for j in range(i + 1, num_floors)
        ]
    )

    elevators_facts = "\n       ".join([e.get_init() for e in elevators])
    passenger_facts = "\n       ".join([p.get_init() for p in passengers])
    goal_facts = "\n       ".join([p.get_goal() for p in passengers])

    passenger_objects = " ".join([p.name for p in passengers])
    fast_elevator_objects = " ".join(
        [e.name for e in elevators if e.name.startswith("fast")]
    )
    slow_elevator_objects = " ".join(
        [e.name for e in elevators if e.name.startswith("slow")]
    )

    slow_floors = set()
    fast_floors = set()
    for e in elevators:
        if e.name.startswith("slow"):
            slow_floors.update(itertools.combinations(e.floors, 2))
        else:
            fast_floors.update(itertools.combinations(e.floors, 2))

    cost_facts = "\n       ".join(
        [
            f"(= (travel-slow n{i} n{j}) {cost_slow.get_cost(i,j)})"
            for i, j in sorted(slow_floors)
        ]
        + [
            f"(= (travel-fast n{i} n{j}) {cost_fast.get_cost(i,j)})"
            for i, j in sorted(fast_floors)
        ]
    )

    print(
        f"""(define (problem {instance_name})
(:domain elevators-sequencedstrips)
    (:objects
       {" ".join(count_objects)} - count
       {passenger_objects} - passenger
       {fast_elevator_objects} - fast-elevator
       {slow_elevator_objects} - slow-elevator
    )

    (:init
       {next_facts}

       {above_facts}

       {elevators_facts}

       {passenger_facts}

       {cost_facts}

       (= (total-cost) 0)
    )

    (:goal (and
       {goal_facts}
    ))

    (:metric minimize (total-cost))
)
    """
    )


class Elevator:
    def __init__(self, name, capacity, first_floor, last_floor, step_size):

        self.name = name
        self.capacity = capacity
        self.floors = range(first_floor, last_floor, step_size)

        self.init_floor = random.choice(self.floors)

    def get_init(self):
        can_hold = " ".join(
            [f"(can-hold {self.name} n{i+1})" for i in range(self.capacity)]
        )
        reachable_floor = " ".join(
            [f"(reachable-floor {self.name} n{i})" for i in self.floors]
        )

        return f"""(lift-at {self.name} n{self.init_floor})
       (passengers {self.name} n0)
       {can_hold}
       {reachable_floor}
    """


class Passenger:
    def __init__(self, identifier, num_floors):
        self.name = f"p{identifier}"
        self.init_floor = random.randint(0, num_floors - 1)
        self.dest_floor = random.randint(0, num_floors - 1)
        while self.init_floor == self.dest_floor:
            self.dest_floor = random.randint(0, num_floors - 1)

    def get_init(self):
        return f"(passenger-at {self.name} n{self.init_floor})"

    def get_goal(self):
        return f"(passenger-at {self.name} n{self.dest_floor})"


class Cost:
    def __init__(self, cost_stop, cost_floor):
        self.cost_stop = cost_stop
        self.cost_floor = cost_floor

    def get_cost(self, i, j):
        return self.cost_stop + abs(i - j) * self.cost_floor


def parse():
    parser = argparse.ArgumentParser()

    parser.add_argument("areas", type=int, help="how many areas")
    parser.add_argument("area_size", type=int, help="")

    parser.add_argument("passengers", type=int, help="how many floors")

    parser.add_argument("fast_elevators", type=int, help="")
    parser.add_argument("slow_elevators", type=int, help="")

    parser.add_argument("--seed", type=int, default=1)

    parser.add_argument("--fast_capacity", type=int, default=6, help="")
    parser.add_argument("--slow_capacity", type=int, default=4, help="")
    parser.add_argument("--slow_cost", type=int, default=1, help="")
    parser.add_argument("--stop_slow_cost", type=int, default=5, help="")
    parser.add_argument("--fast_cost", type=int, default=3, help="")
    parser.add_argument("--stop_fast_cost", type=int, default=1, help="")

    parser.add_argument("--output", default=None)

    return parser.parse_args()


def main():
    args = parse()
    random.seed(args.seed)

    assert args.area_size >= 2

    floors = args.areas * args.area_size + 1
    fast_step_size = max(1, args.area_size // 2)
    slow_step_size = 1

    elevators = [
        Elevator(f"fast{i}", args.fast_capacity, 0, floors, fast_step_size)
        for i in range(args.fast_elevators)
    ] + [
        Elevator(
            f"slow{j}-{i}",
            args.slow_capacity,
            j * args.area_size,
            min(floors, (j + 1) * (args.area_size) + 1),
            slow_step_size,
        )
        for i in range(args.slow_elevators)
        for j in range(args.areas)
    ]

    passengers = [Passenger(i, floors) for i in range(args.passengers)]

    instance_name = "elevators"

    slow_cost = Cost(args.stop_slow_cost, args.slow_cost)
    fast_cost = Cost(args.stop_fast_cost, args.fast_cost)

    if args.output:
        sys.stdout = open(f"{args.output}/{instance_name}.pddl", "w")

    print(
        f"""; {floors} floors, area size: {args.area_size}, areas: {args.areas}, passengers: {args.passengers}"""
    )

    print_pddl(instance_name, floors, elevators, passengers, slow_cost, fast_cost)


if __name__ == "__main__":
    main()
