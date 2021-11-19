#! /usr/bin/env python3

"""
Openstacks generator for sequential STRIPS with negative preconditions.
"""

import argparse
import math
import random


class Task:
    def __init__(self, orders, durations):
        self.orders = orders
        self.durations = durations
        self.num_orders = len(self.orders)
        self.num_products = len(self.orders[0])

    def get_order_ids(self):
        return range(1, self.num_orders + 1)

    def get_product_ids(self):
        return range(1, self.num_products + 1)


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("products", type=int, help="Number of products")
    parser.add_argument("orders", type=int, help="Number of orders")
    parser.add_argument(
        "density",
        type=int,
        help="approximate percentage of products included in an order",
    )
    parser.add_argument("--domain", default="domain.pddl")
    parser.add_argument("--problem", default="problem.pddl")
    parser.add_argument("--seed", type=int, default=1)
    args = parser.parse_args()

    if not 0 <= args.density <= 100:
        raise argparse.ArgumentTypeError("density must be between 0 and 100")

    return args


def include_product_for_order(product, order, num_products, density):
    # This is based on Ioannis's code. It is designed so that products
    # are more likely to be included in orders if the distance between
    # the product and order id (before the reshuffling that happens at
    # the end) is small.
    #
    # Compared to an independent identically distributed choice for
    # each product/order pair, this should tend to favour instances
    # with small (= few stacks) solutions since it adds some locality
    # to the problem: the matrix entries (before shuffling) will tend
    # to cluster around the diagonal.

    d = abs(product - order)
    s = num_products / 6.0
    # The next line is the density function of the normal distribution
    # with mu = 0 and sigma = s for x = d.
    p = (1 / (s * (2 * math.pi) ** 0.5)) * math.exp(-(d * d) / (2 * s * s))
    # The next line is rather magical to me. If there is an
    # explanation for what this formula signifies, I don't know it.
    chance = p * density * num_products / (2.7 * (num_products / 5.0) ** 0.5)
    return random.randrange(100) <= chance


def generate_task(num_products, num_orders, density):
    products = list(range(num_products))
    orders = list(range(num_orders))
    matrix = [[0 for product in products] for order in orders]
    for order in orders:
        for product in products:
            if include_product_for_order(product, order, num_products, density):
                matrix[order][product] = 1
    for order in orders:
        if all(matrix[order][product] == 0 for product in products):
            matrix[order][random.choice(products)] = 1
    for product in products:
        if all(matrix[order][product] == 0 for order in orders):
            matrix[random.choice(orders)][product] = 1
    weights = [random.randrange(10, 100 + 1, 10) for product in products]

    random.shuffle(products)
    # Ioannis's original code, used for generating the competition
    # instances, only shuffles the orders but not the products, which
    # means that planners that process the products in numerical order
    # (or whatever) are somewhat biased. (Depending on the planner
    # and/or track, I guess this can be either positive or negative.)
    random.shuffle(orders)

    return Task(matrix, weights)


def write_problem(task, filename):
    def write(*args, **kwargs):
        print(*args, file=outfile, **kwargs)

    with open(filename, "w") as outfile:
        write("(define (problem os)")

        write("(:domain openstacks-sequencedstrips-nonADL)")

        write("(:objects")
        max_number = max(task.num_orders, task.num_products)
        for i in range(max_number + 1):
            write(f"n{i}", end=" ")
        write(" - count")
        write(")")

        write("\n(:init")

        for i in range(max_number):
            write(f"(next-count n{i} n{i + 1})", end=" ")
        write()

        write("(stacks-avail n0)")

        for i in range(1, task.num_orders + 1):
            write(f"\n(waiting o{i})")
            for j in range(1, task.num_products + 1):
                if task.orders[i - 1][j - 1] == 1:
                    write(f"(includes o{i} p{j})", end="")
            write()
        write("\n(= (total-cost) 0)")

        write(")")

        write("(:goal\n(and")
        for i in range(1, task.num_orders + 1):
            write(f"(shipped o{i})")

        write("))")
        write("(:metric minimize (total-cost))\n\n)\n")


def write_domain(task, filename):
    def write(*args, **kwargs):
        print(*args, file=outfile, **kwargs)

    with open(filename, "w") as outfile:
        write("(define (domain openstacks-sequencedstrips-nonADL)")
        write("(:requirements :typing :action-costs)")
        write("(:types order product count)")

        write("(:constants")
        for j in range(1, task.num_products + 1):
            write(f" p{j}", end="")
        write(" - product")

        for i in range(1, task.num_orders + 1):
            write(f" o{i}", end="")
        write(" - order")
        write(")\n")

        write("(:predicates")
        write("\t(includes ?o - order ?p - product)")
        write("\t(waiting ?o - order)")
        write("\t(started ?o - order)")
        write("\t(shipped ?o - order)")
        write("\t(made ?p - product)")
        write("\t(stacks-avail ?s - count)")
        write("\t(next-count ?s ?ns - count)")
        write(")\n")

        write("(:functions")
        write("(total-cost)")
        write(")\n")

        write("(:action open-new-stack")
        write(":parameters (?open ?new-open - count)")
        write(":precondition (and (stacks-avail ?open)(next-count ?open ?new-open))")
        write(":effect (and (not (stacks-avail ?open))(stacks-avail ?new-open)")
        write(" (increase (total-cost) 1))")
        write(")\n")

        # START-ORDER
        write("(:action start-order")
        write(":parameters (?o - order ?avail ?new-avail - count)")
        write(":precondition ")
        write("(and (waiting ?o)")
        write("(stacks-avail ?avail)(next-count ?new-avail ?avail))")
        write(":effect (and (not (waiting ?o))(started ?o)")
        write("(not (stacks-avail ?avail))(stacks-avail ?new-avail))")
        write(")\n")

        # MAKE-PRODUCT
        for j in task.get_product_ids():
            write(f"(:action make-product-p{j}")
            write(":parameters ()")
            write(":precondition ")

            write(f"(and (not (made p{j}))")

            for i in task.get_order_ids():
                if task.orders[i - 1][j - 1] == 1:
                    write(f"(started o{i})")
            write(")")

            write(f":effect (and (made p{j}))")
            write(")\n")

        # SHIP-ORDER
        for i in task.get_order_ids():
            write(f"(:action ship-order-o{i}")
            write(":parameters (?avail ?new-avail - count)")
            write(f":precondition (and (started o{i})", end="")
            for j in task.get_product_ids():
                if task.orders[i - 1][j - 1] == 1:
                    write(f"(made p{j})", end="")
            write("(stacks-avail ?avail)(next-count ?avail ?new-avail))")

            write(f":effect (and (not (started o{i}))(shipped o{i})", end="")
            write("(not (stacks-avail ?avail))(stacks-avail ?new-avail)))\n")

        write(")\n")


def main():
    args = parse_args()
    random.seed(args.seed)
    task = generate_task(args.products, args.orders, args.density)
    write_problem(task, args.problem)
    write_domain(task, args.domain)


main()
