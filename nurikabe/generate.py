#!/usr/bin/env python

import argparse
from itertools import product
import random
from collections import Counter
import sys


def print_solver(instance_name, width, height, s):
    print(width, height)
    for (x, y, n) in s:
        print(x, y, n)


def adjacent(width, height, x, y):
    res = []
    if x < width - 1:
        res.append((x + 1, y))
    if y < height - 1:
        res.append((x, y + 1))
    if x > 0:
        res.append((x - 1, y))
    if y > 0:
        res.append((x, y - 1))
    return res


def get_adjacent_pairs(width, height):
    res = []
    for x in range(0, width):
        for y in range(0, height):
            if x < width - 1:
                res.append((x, y, x + 1, y))
            if y < height - 1:
                res.append((x, y, x, y + 1))
            if x > 0:
                res.append((x, y, x - 1, y))
            if y > 0:
                res.append((x, y, x, y - 1))
    return res


def print_pddl(instance_name, width, height, s):
    max_number = max(n for (x, y, n) in s)
    number_objects = " ".join(["n%d" % n for n in range(1, max_number + 1)])

    positions = list(product(range(width), range(height)))
    available_positions = list(product(range(width), range(height)))
    blocked_positions = []
    partof_positions = []
    for i in range(len(s)):
        (x, y, n) = s[i]
        available_positions.remove((x, y))
        adj_list = adjacent(width, height, x, y)
        partof_positions += [(x, y, i) for (x, y) in adj_list]
        for p in adj_list:
            if p in available_positions:
                available_positions.remove(p)
            elif p not in blocked_positions:
                blocked_positions.append(p)

    partof_positions = [
        (x, y, z) for (x, y, z) in partof_positions if (x, y) not in blocked_positions
    ]

    # print available_positions
    # print blocked_positions
    # print partof_positions

    pos_objects = " ".join(["pos-%s-%s" % pos for pos in positions])

    group_objects = " ".join(["g%d" % n for n in range(0, len(s))])

    goal_facts = "\n ".join(["(group-painted g%d)" % n for n in range(0, len(s))])

    next_facts = "\n ".join(
        ["(NEXT n%d n%d)" % (n, n + 1) for n in range(0, max_number)]
    )

    is_adjacent_facts = "\n ".join(
        [
            "(CONNECTED pos-%s-%s pos-%s-%s)" % adj
            for adj in get_adjacent_pairs(width, height)
        ]
    )

    source_facts = "\n ".join(
        ["(SOURCE pos-%s-%s g%d)" % (s[i][0], s[i][1], i) for i in range(0, len(s))]
    )

    available_facts = "\n ".join(
        [f"(available pos-{x}-{y})" for (x, y) in available_positions]
    )

    blocked_facts = "\n ".join(
        [f"(blocked pos-{x}-{y})" for (x, y) in blocked_positions]
    )

    part_of_facts = "\n ".join(
        ["(part-of pos-%s-%s g%d)" % (x, y, i) for (x, y, i) in partof_positions]
    )

    remaining_cell_facts = "\n ".join(
        ["(remaining-cells g%d n%d)" % (i, s[i][2]) for i in range(0, len(s))]
    )

    print(
        """(define (problem {instance_name})
(:domain paint-nurikabe)
(:objects
  {pos_objects} - cell
  {number_objects} - num
  {group_objects} - group
)
(:init
 {next_facts}

 {is_adjacent_facts}

 (robot-pos pos-0-0)
 (moving)

 {source_facts}

 {available_facts}
 {blocked_facts}
 {part_of_facts}
 {remaining_cell_facts}
)
(:goal (and
 {goal_facts}
))
)
""".format(
            **locals()
        )
    )


def print_solution(width, height, m):
    print(width, height)
    for row in m:
        for cell in row:
            if cell == " ":
                cell = "."
            print(cell, end=" ")
        print


def is_number(x):
    try:
        d = int(x)
        return True
    except ValueError:
        return False


def is_empty(cell):
    return cell == " " or is_number(cell)


def reconstruct_islands(colored_map):
    result = [list(row) for row in colored_map]
    height = len(result)
    width = len(result[0])
    islands = []
    for y, row in enumerate(result):
        for x, cell in enumerate(row):
            if is_empty(cell):
                queue = [(x, y)]
                result[y][x] = "X"
                island_cells = []
                island_start = (x, y)
                while queue:
                    cx, cy = queue.pop()
                    island_cells.append((cx, cy))
                    for nx, ny in [
                        (cx + 1, cy),
                        (cx - 1, cy),
                        (cx, cy + 1),
                        (cx, cy - 1),
                    ]:
                        if 0 <= ny < height and 0 <= nx < width:
                            if is_number(result[ny][nx]):
                                island_start = (nx, ny)
                            if is_empty(result[ny][nx]):
                                queue.append((nx, ny))
                                result[ny][nx] = "X"
                islands.append((island_start, island_cells))
    for (sx, sy), cells in islands:
        n = len(cells)
        for x, y in cells:
            result[y][x] = " "
        result[sy][sx] = str(n)
    return result


def scale_down_instance(colored_map, target_width, target_height):
    scaled_map = [list(row[:target_width]) for row in colored_map[:target_height]]
    return reconstruct_islands(scaled_map)


def print_map(colored_map):
    for row in colored_map:
        print(" ".join(row))


def map_to_instance(colored_map):
    s = []
    for y, row in enumerate(colored_map):
        for x, cell in enumerate(row):
            if is_number(cell):
                s.append((x, y, int(cell)))
    return s


def random_map(width, height):
    STOPCHANCE = 0.01
    BRANCHCHANCE = 0.5
    cmap = [[" " for x in range(width)] for y in range(height)]
    ends = [(random.randint(0, width - 1), random.randint(0, height - 1))]
    cmap[ends[0][1]][ends[0][0]] = "#"
    next_ends = []
    while ends:
        for x, y in ends:
            if random.random() < STOPCHANCE:
                continue
            neighbors = []
            for nx, ny in [(x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)]:
                if 0 <= ny < height and 0 <= nx < width and cmap[ny][nx] == " ":
                    for nnx, nny in [
                        (nx + 1, ny),
                        (nx - 1, ny),
                        (nx, ny + 1),
                        (nx, ny - 1),
                    ]:
                        if (nnx, nny) == (x, y):
                            continue
                        if (
                            0 <= nny < height
                            and 0 <= nnx < width
                            and cmap[nny][nnx] == "#"
                        ):
                            break
                    else:
                        neighbors.append((nx, ny))
            if neighbors:
                successors = {random.choice(neighbors)}
                if random.random() < BRANCHCHANCE:
                    successors.add(random.choice(neighbors))
                for nx, ny in successors:
                    cmap[ny][nx] = "#"
                    next_ends.append((nx, ny))
        ends = next_ends
        next_ends = []
    return reconstruct_islands(cmap)


def reasonable_random_map(width, height):
    size = max(width, height)
    for i in range(1000):
        m = random_map(width, height)
        groups = Counter([s for x, y, s in map_to_instance(m)])
        if groups.get(1, 0) <= size / 2 and sorted(groups.items())[-1][0] < size:
            return m


def generate_state_lpo(args):
    url = "http://www.logicgamesonline.com/nurikabe/archive.php?pid=%d" % args.pid
    instance_name = "nurikabe-lpo%d-%dx%d" % (args.pid, args.width, args.height)
    from urllib.error import URLError
    from urllib.request import urlopen

    try:
        response = urlopen(url, timeout=5)
        content = response.read().decode("utf-8")
    except URLError as e:
        sys.exit(e)

    puzzle = ""
    for line in content.splitlines():
        if line.startswith('var solpuz = "'):
            puzzle = line[len('var solpuz = "') : -2]
            break
    if len(puzzle) != 9 * 9:
        sys.exit("Cannot find or parse puzzle")
    colored_map = [list(puzzle[i * 9 : (i + 1) * 9]) for i in range(9)]
    colored_map = scale_down_instance(colored_map, args.width, args.height)
    return instance_name, colored_map


def generate_state_random(args):
    instance_name = "random-%dx%d-%d" % (args.width, args.height, args.random_seed)
    colored_map = reasonable_random_map(args.width, args.height)
    return instance_name, colored_map


def parse():
    parser = argparse.ArgumentParser()
    parser.add_argument("source", choices=["lpo", "random"])
    # TODO: this should only be an option when using lpo
    parser.add_argument("pid", type=int, help="problem ID on logicgamesonline.com")
    parser.add_argument("width", type=int, help="Width of the board.")
    parser.add_argument("height", type=int, help="Height of the board.")
    parser.add_argument("random_seed", type=int)
    parser.add_argument("output_type", choices=["pddl", "raw", "solution"])
    return parser.parse_args()


def main():
    args = parse()
    random.seed(args.random_seed)

    if args.source == "lpo":
        instance_name, m = generate_state_lpo(args)
    elif args.source == "random":
        instance_name, m = generate_state_random(args)

    s = map_to_instance(m)

    if args.output_type == "raw":
        print_solver(instance_name, args.width, args.height, s)
    elif args.output_type == "pddl":
        print_pddl(instance_name, args.width, args.height, s)
    elif args.output_type == "solution":
        print_solution(args.width, args.height, m)


if __name__ == "__main__":
    main()
