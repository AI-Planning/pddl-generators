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

city = euclidean_graph.generate_connected(nodes, size, size, connect_distance, epsilon)


id = (
    "city-sequential-%dnodes-%dsize-%ddegree-%dmindistance-%dtrucks-%dpackages-%dseed"
    % (nodes, size, degree, epsilon, trucks, packages, seed)
)

city.dump_tikz(open("%s.tex" % id, "w"))

print("; Transport %s" % id)
print()

print("(define (problem transport-%s)" % id)
print(" (:domain transport)")
print(" (:objects")

for i in range(nodes):
    print("  city-loc-%d - location" % (i + 1))
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

for u, v in city.edges:
    print("  ; %d,%d -> %d,%d" % (u.x, u.y, v.x, v.y))

    dist = u.round_distance(v)
    road("city-" + u.name, "city-" + v.name, dist)

packages_position = {}
for i in range(packages):
    packages_position["package-%d" % (i + 1)] = random.choice(city.vertices).name
    print(
        "  (at package-%d city-%s)" % (i + 1, packages_position["package-%d" % (i + 1)])
    )

for i in range(trucks):
    capacity = random.randint(2, max_capacity)
    print("  (at truck-%d city-%s)" % (i + 1, random.choice(city.vertices).name))
    print("  (capacity truck-%d capacity-%d)" % (i + 1, capacity))


print(" )")
print(" (:goal (and")
for i in range(packages):
    target_pos = random.choice(city.vertices).name
    while target_pos == packages_position["package-%d" % (i + 1)]:
        target_pos = random.choice(city.vertices).name
    print("  (at package-%d city-%s)" % (i + 1, target_pos))
print(" ))")

print(" (:metric minimize (total-cost))")
print(")")
