#! /usr/bin/env python

from __future__ import print_function

import math
import random


USAGE = """usage:
    %s MIN_SIZE MAX_SIZE STEP_SIZE MIN_ID MAX_ID DENSITY
where
    MIN_SIZE   the minimum problem size (number of products and orders)
    MAX_SIZE   the maximum problem size (number of products and orders)
    STEP_SIZE  the increment on problem size
    MIN_ID     the start number of problems of the same size
    MAX_ID     the end number of problems of the same size
    DENSITY    the approximate percentage of products included in each order"""


def main(args):
    if len(args) != 7:
        raise SystemExit(USAGE % args[0])
    min_size, max_size, step_size, min_id, max_id, density = list(map(int, args[1:]))
    for size in range(min_size, max_size + 1, step_size):
        for counter in range(min_id, max_id + 1):
            filename = "p%d_%d.txt" % (size, counter)
            generate_problem(filename, size, size, density)


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


def generate_problem(filename, num_products, num_orders, density):
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

    with open(filename, "w") as outfile:
        print(num_orders, num_products, file=outfile)
        for order in orders:
            row = [matrix[order][product] for product in products]
            print(" ".join(map(str, row)), file=outfile)
        print(" ".join(map(str, weights)), file=outfile)


if __name__ == "__main__":
    import sys

    main(sys.argv)
