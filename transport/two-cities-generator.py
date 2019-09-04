#! /usr/bin/env python

import copy
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


def min_required_fuel(graph, nodes, size, start):
    dist = {}
    for v in graph.vertices:
        dist[v] = size * size * nodes * 4  # upper bound ...

    dist[start] = 0
    queue = copy.copy(graph.vertices)
    assert start in queue

    while len(queue) > 0:
        mindist = size * size * nodes * 4
        for x in queue:
            if dist[x] < mindist:
                mindist = dist[x]
                minelement = x
        queue.remove(minelement)
        for v, u in graph.edges:
            if u == minelement and v in queue:
                ndist = (
                    dist[minelement] + minelement.round_distance(v) * 2
                )  # *2; conversion from distance to fuel req
                if ndist < dist[v]:
                    dist[v] = ndist

    ret = 0
    for x in list(dist.keys()):
        if dist[x] > ret:
            ret = dist[x]
    return ret * 2


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


id = (
    "two-cities-sequential-%dnodes-%dsize-%ddegree-%dmindistance-%dtrucks-%dpackages-%dseed"
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

# connect the two cities
min_connect_distance = 4 * size
for v in city_a.vertices:
    for u in city_b.vertices:
        if (
            v.distance(euclidean_graph.Point(u.name, u.x + 2 * size, u.y))
            < min_connect_distance
        ):
            min_tuple = (v, u)
            min_connect_distance = v.distance(
                euclidean_graph.Point(u.name, u.x + 2 * size, u.y)
            )

print(
    "  ; %d,%d <-> %d,%d"
    % (min_tuple[0].x, min_tuple[0].y, min_tuple[1].x + 2 * size, min_tuple[1].y)
)

symmetric_road(
    "city-1-" + min_tuple[0].name, "city-2-" + min_tuple[1].name, min_connect_distance
)

for i in range(packages):
    print("  (at package-%d city-1-%s)" % (i + 1, random.choice(city_a.vertices).name))

required_fuel = max(
    min_required_fuel(city_a, nodes, size, min_tuple[0]),
    min_required_fuel(city_b, nodes, size, min_tuple[1]),
    min_connect_distance / 2,
)
for i in range(trucks):
    print("  (at truck-%d city-2-%s)" % (i + 1, random.choice(city_a.vertices).name))
    capacity = random.randint(2, 4)
    print("  (capacity truck-%d capacity-%d)" % (i + 1, capacity))


print(" )")
print(" (:goal (and")

for i in range(packages):
    print("  (at package-%d city-2-%s)" % (i + 1, random.choice(city_b.vertices).name))

print(" ))")

print(" (:metric minimize (total-cost))")

print(")")
