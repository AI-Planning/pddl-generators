#! /usr/bin/env python

import math
import euclidean_graph
import sys
import random

sys.path.append("../")
sys.path.append("../../")

MAX_SEED = 10000000


def road(f, t, length):
    length = int(math.ceil(length / 10.0))
    print("  (road %s %s)" % (f, t))
    print("  (= (road-length %s %s) %d)" % (f, t, length))


def symmetric_road(f, t, length):
    road(f, t, length)
    road(t, f, length)


def shortest_route(ca, cb, ox, oy):
    # connect the two cities
    min_connect_distance = -1.0
    for v in city_a.vertices:
        for u in city_b.vertices:
            if (
                v.distance(euclidean_graph.Point(u.name, u.x + ox, u.y + oy))
                < min_connect_distance
                or min_connect_distance == -1.0
            ):
                min_tuple = (v, u)
                min_connect_distance = v.distance(
                    euclidean_graph.Point(u.name, u.x + 2 * size, u.y)
                )

    return min_tuple, min_connect_distance


if len(sys.argv) != 8:
    raise SystemExit(
        "Usage: <nodes> <size^(1/2)> <degree> <mindistance> <nr-trucks> <nr-packages> <seed>"
    )

nodes = int(sys.argv[1])
size = int(sys.argv[2])
degree = float(sys.argv[3])
epsilon = float(sys.argv[4])
trucks = int(sys.argv[5])
packages = int(sys.argv[6])
seed = float(sys.argv[7])

max_capacity = 4  # maximum number of packages in one truck
assert max_capacity > 2

if not seed:
    seed = random.randrange(MAX_SEED) + 1
random.seed(seed)

#         deg * width * height
# ratio = ---------------------------
#         nodes * pi * Connect^2
connect_distance = math.sqrt((degree * size * size) / (nodes * math.pi * 0.694))

city_a = euclidean_graph.generate_connected(
    nodes, size, size, connect_distance, epsilon
)
city_b = euclidean_graph.generate_connected(
    nodes, size, size, connect_distance, epsilon
)
city_c = euclidean_graph.generate_connected(
    nodes, size, size, connect_distance, epsilon
)


id = (
    "three-cities-sequential-%dnodes-%dsize-%ddegree-%dmindistance-%dtrucks-%dpackages-%dseed"
    % (nodes, size, degree, epsilon, trucks, packages, seed)
)

print("; Transport %s" % id)
print()

print("(define (problem transport-%s)" % id)
print(" (:domain transport)")
print(" (:objects")

for i in range(nodes):
    print("  city-1-loc-%d - location" % (i + 1))
    print("  city-2-loc-%d - location" % (i + 1))
    print("  city-3-loc-%d - location" % (i + 1))
for i in range(trucks):
    print("  truck-%d - vehicle" % (i + 1))

for i in range(packages):
    print("  package-%d - package" % (i + 1))

for i in range(max_capacity + 1):
    print("  capacity-%d - capacity-number" % i)

print(" )")
print(" (:init")

print("  (= (total-cost) 0)")


for i in range(max_capacity):
    print("  (capacity-predecessor capacity-%d capacity-%d)" % (i, i + 1))

for u, v in city_a.edges:
    print("  ; %d,%d -> %d,%d" % (u.x, u.y, v.x, v.y))

    dist = u.round_distance(v)
    road("city-1-" + u.name, "city-1-" + v.name, dist)
for u, v in city_b.edges:
    print("  ; %d,%d -> %d,%d" % (u.x + 2 * size, u.y, v.x + 2 * size, v.y))

    dist = u.round_distance(v)
    road("city-2-" + u.name, "city-2-" + v.name, dist)
for u, v in city_c.edges:
    print(
        "  ; %d,%d -> %d,%d" % (u.x + size, u.y + 2 * size, v.x + size, v.y + 2 * size)
    )

    dist = u.round_distance(v)
    road("city-3-" + u.name, "city-3-" + v.name, dist)

connect_ab, dist_ab = shortest_route(city_a, city_b, 2 * size, 0)
print(
    "  ; %d,%d <-> %d,%d"
    % (connect_ab[0].x, connect_ab[0].y, connect_ab[1].x + 2 * size, connect_ab[1].y)
)
symmetric_road("city-1-" + connect_ab[0].name, "city-2-" + connect_ab[1].name, dist_ab)
connect_ac, dist_ac = shortest_route(city_a, city_c, size, 2 * size)
symmetric_road("city-1-" + connect_ac[0].name, "city-3-" + connect_ac[1].name, dist_ac)
connect_bc, dist_bc = shortest_route(city_b, city_c, size, -2 * size)
symmetric_road("city-2-" + connect_bc[0].name, "city-3-" + connect_bc[1].name, dist_bc)

package_loc = {}
for i in range(packages):
    package_loc["package-%d" % (i + 1)] = (
        random.randint(1, 3),
        random.choice(city_a.vertices).name,
    )
    print(
        "  (at package-%d city-%d-%s)"
        % (
            i + 1,
            package_loc["package-%d" % (i + 1)][0],
            package_loc["package-%d" % (i + 1)][1],
        )
    )

for i in range(trucks):
    print(
        "  (at truck-%d city-%d-%s)"
        % (i + 1, random.randint(1, 3), random.choice(city_a.vertices).name)
    )
    capacity = random.randint(2, 4)
    print("  (capacity truck-%d capacity-%d)" % (i + 1, capacity))


print(" )")
print(" (:goal (and")

for i in range(packages):
    rc = random.randint(1, 3)
    rl = random.choice(city_b.vertices).name
    while (
        rc == package_loc["package-%d" % (i + 1)][0]
        and rl == package_loc["package-%d" % (i + 1)][1]
    ):
        rc = random.randint(1, 3)
        rl = random.choice(city_b.vertices).name
    print("  (at package-%d city-%d-%s)" % (i + 1, rc, rl))

print(" ))")

print(" (:metric minimize (total-cost))")

print(")")
