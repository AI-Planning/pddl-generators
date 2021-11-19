#!/usr/bin/env python

import argparse
from collections import defaultdict
from z3 import *
from copy import deepcopy


def get_location(x, y):
    return "pos-%d-%d" % (x, y)


def place_block(grid, loc, loc2):
    height = grid[loc[1]][loc[0]]
    new_height = grid[loc2[1]][loc2[0]]

    return "(place-block {} {} n{} n{})".format(
        get_location(*loc), get_location(*loc2), new_height, new_height + 1
    )


def remove_block(grid, loc, loc2):
    height = grid[loc[1]][loc[0]]
    new_height = grid[loc2[1]][loc2[0]]

    return "(remove-block {} {} n{} n{})".format(
        get_location(*loc), get_location(*loc2), new_height, new_height - 1
    )


def move(grid, loc, loc2):
    height = grid[loc[1]][loc[0]]
    new_height = grid[loc2[1]][loc2[0]]

    if height > new_height:
        return "(move-down {} n{} {} n{})".format(
            get_location(*loc), height, get_location(*loc2), new_height
        )
    elif height < new_height:
        return "(move-up {} n{} {} n{})".format(
            get_location(*loc), height, get_location(*loc2), new_height
        )
    else:
        return "(move {} {} n{})".format(
            get_location(*loc), get_location(*loc2), new_height
        )


def print_grid(grid):
    for row in grid:
        print(" ".join(map(str, row)))


def accessible(i_grid, x, y, place_block):
    for (xp, yp) in [(1, 0), (-1, 0), (0, 1), (0, -1)]:
        if (
            x + xp < 0
            or x + xp > len(i_grid[0]) - 1
            or y + yp < 0
            or y + yp > len(i_grid) - 1
        ):
            continue

        if not place_block:
            # The goal must be reached from a location at one height below
            if i_grid[y][x] == i_grid[y + yp][x + xp] + 1:
                return True

        if place_block:
            # The goal must be reached from a location at the same height
            if i_grid[y][x] == i_grid[y + yp][x + xp]:
                return True

    return False


def get_shortest_path(i_grid, ini_loc, goal_loc, place_block, order_grid):
    assert accessible(i_grid, goal_loc[0], goal_loc[1], place_block)
    parent_ptr = {}
    closed, queue = set(), [ini_loc]
    while queue:
        vertex = queue.pop(0)
        if vertex == goal_loc:
            solution = []
            while vertex != ini_loc:
                solution.append(vertex)
                vertex = parent_ptr[vertex]
            solution.append(vertex)
            return solution[::-1]
        if vertex not in closed:
            closed.add(vertex)
            (x, y) = vertex
            for (xp, yp) in [(1, 0), (-1, 0), (0, 1), (0, -1)]:

                if (
                    x + xp < 0
                    or x + xp > len(i_grid[0]) - 1
                    or y + yp < 0
                    or y + yp > len(i_grid) - 1
                ):
                    continue

                if (
                    place_block
                    and abs(i_grid[y][x] - i_grid[y + yp][x + xp]) == 1
                    and (x + xp, y + yp) == goal_loc
                ):
                    continue  # The goal must be reached from a location at the same height

                if (
                    not place_block
                    and i_grid[y][x] - i_grid[y + yp][x + xp] >= 0
                    and (x + xp, y + yp) == goal_loc
                ):
                    continue  # The goal must be reached from a location at one height below

                if (
                    abs(i_grid[y][x] - i_grid[y + yp][x + xp]) <= 1
                    and (x + xp, y + yp) not in parent_ptr
                ):
                    parent_ptr[(x + xp, y + yp)] = (x, y)
                    queue.append((x + xp, y + yp))

    print("Error: no path found")
    print_grid(i_grid)
    print(ini_loc)
    print(goal_loc)
    print(place_block)

    print(closed)

    return None


def solve(
    grid_size_x,
    grid_size_y,
    max_height,
    robots,
    depots,
    i_grid,
    g_grid,
    minimize=True,
    timeout=100000,
):
    i_grid = deepcopy(i_grid)
    # print ("Solving: ",  grid_size_x, grid_size_y, max_height, robots, depots, i_grid, g_grid)
    assert grid_size_y == len(i_grid)

    adjacent = defaultdict(list)

    # A cell X is supporter of another Y if we will use X in order to access Y
    for x in range(grid_size_x):
        for y in range(grid_size_y):
            if x > 0:
                adjacent[(x, y)].append((x - 1, y))

            if y > 0:
                adjacent[(x, y)].append((x, y - 1))

            if x < grid_size_x - 1:
                adjacent[(x, y)].append((x + 1, y))

            if y < grid_size_y - 1:
                adjacent[(x, y)].append((x, y + 1))

    intermediate_board = [
        [Int("intermediate%d-%d" % (c, r)) for c in range(grid_size_x)]
        for r in range(grid_size_y)
    ]
    target_board = [
        [Int("target%d-%d" % (c, r)) for c in range(grid_size_x)]
        for r in range(grid_size_y)
    ]

    order = [
        [Int("order%d-%d" % (c, r)) for c in range(grid_size_x)]
        for r in range(grid_size_y)
    ]
    is_supporter = [
        [Bool("supporter%d-%d" % (c, r)) for c in range(grid_size_x)]
        for r in range(grid_size_y)
    ]
    is_supported = [
        [Bool("supported%d-%d" % (c, r)) for c in range(grid_size_x)]
        for r in range(grid_size_y)
    ]

    supporter_variables = {}
    for x in range(grid_size_x):
        for y in range(grid_size_y):
            for (xp, yp) in adjacent[(x, y)]:
                supporter_variables[(x, y, xp, yp)] = Bool(
                    "supporter%d-%d-%d-%d" % (x, y, xp, yp)
                )

    all_intermediate_board = [cell for column in intermediate_board for cell in column]
    s = Optimize()
    # s.set("timeout", timeout)

    for (y, row) in enumerate(g_grid):
        for (x, tile) in enumerate(row):
            if (x, y) in depots:
                s.add(
                    intermediate_board[y][x] == 0
                )  # The depot location must always have height 0
                s.add(order[y][x] == 0)  # The depot location must always have order 0
            else:
                s.add(order[y][x] > 0)
                s.add(target_board[y][x] == tile)

                s.add(
                    intermediate_board[y][x] <= max_height
                )  # Intermediate_Board cannot have more height than max_height
                s.add(
                    intermediate_board[y][x] >= max(tile, i_grid[y][x])
                )  # Intermediate_Board cannot have negative height

                # All intermediate_board supporter or greater than 0 must have a supporter
                s.add(Implies(intermediate_board[y][x] > 0, is_supported[y][x] == True))

                s.add(
                    is_supported[y][x]
                    == Or(
                        *[
                            supporter_variables[(x, y, xp, yp)] == True
                            for (xp, yp) in adjacent[(x, y)]
                        ]
                    )
                )
                s.add(
                    is_supporter[y][x]
                    == Or(
                        *[
                            supporter_variables[(xp, yp, x, y)] == True
                            for (xp, yp) in adjacent[(x, y)]
                        ]
                    )
                )
                s.add(Implies(is_supporter[y][x], is_supported[y][x]))
                for (xp, yp) in adjacent[(x, y)]:
                    # A cell cannot support its supporter (there is a total order of supporters)
                    s.add(
                        Implies(
                            supporter_variables[(x, y, xp, yp)],
                            order[yp][xp] < order[y][x],
                        )
                    )

                    # The supporter must give access to the cell
                    s.add(
                        Or(
                            supporter_variables[(x, y, xp, yp)] == False,
                            intermediate_board[y][x] == intermediate_board[yp][xp],
                            intermediate_board[y][x] == intermediate_board[yp][xp] + 1,
                        )
                    )

                    # The supporter must give access to remove the cell
                    s.add(
                        Implies(
                            supporter_variables[(x, y, xp, yp)],
                            target_board[y][x] >= target_board[yp][xp],
                        )
                    )

                    # s.add(Or(is_supporter[y][x] == False, is_supported[y][x] == False, is_supporter[yp][xp] == False, is_supported[yp][xp] == False, order[y][x] == order[yp][xp], order[y][x] == order[yp][xp] + 1, order[y][x] == order[yp][xp] - 1))

                    # s.add (intermediate_board[0][0] == 0)
    if minimize:
        h = s.minimize(
            Sum(*all_intermediate_board) * 1000
            + Sum([cell for column in order for cell in column])
        )
    s_result = s.check()
    # print(s)
    # print(s_result)
    if str(s_result).strip() == "unsat":
        # print(s)
        # exit()
        return None
    if str(s_result).strip() == "unknown":
        return None
    m = s.model()

    # for r in range(grid_size_y):
    #     sys.stdout.write ("; ")

    #     for c in range(grid_size_x):
    #         sys.stdout.write (str(m[cells[r][c]])+" ")
    #     print ("")

    target_grid = [
        [int(m[intermediate_board[r][c]].as_long()) for c in range(grid_size_x)]
        for r in range(grid_size_y)
    ]
    order_grid = [
        [int(m[order[r][c]].as_long()) for c in range(grid_size_x)]
        for r in range(grid_size_y)
    ]
    # print ("Init:")
    # print_grid(i_grid)

    # print ("Target:")
    # print_grid(target_grid)

    # print ("Goal:")
    # print_grid(g_grid)

    # print ("Order:")
    # print_grid(order_grid)

    # for x in range(grid_size_x):
    #     for y in range (grid_size_y):
    #         for (xp, yp) in adjacent[(x, y)]:
    #             if m[supporter_variables[(x, y, xp, yp)]]:
    #                 print ((x, y), " is supported by ",  (xp, yp))
    # exit()

    depot_loc = get_location(*depots[0])
    plan = []
    while i_grid != target_grid:
        # Pick lowest position that is different. In case of tie, pick the one highest in order
        different_positions = [
            (x, y)
            for x in range(grid_size_x)
            for y in range(grid_size_y)
            if i_grid[y][x] < target_grid[y][x]
            if accessible(i_grid, x, y, True)
        ]

        if not different_positions:
            print("Error: I cannot place the next position")
            print_grid(i_grid)
            print()
            print_grid(target_grid)
            exit()
        (x, y) = min(
            different_positions,
            key=lambda w: (i_grid[w[1]][w[0]], -order_grid[w[1]][w[0]]),
        )

        plan.append(f"(create-block {depot_loc})")

        path = get_shortest_path(i_grid, depots[0], (x, y), True, order_grid)
        if not path:
            return None
        prev_loc = path[0]
        for loc in path[1:-1]:
            plan.append(move(i_grid, prev_loc, loc))
            prev_loc = loc

        plan.append(place_block(i_grid, path[-2], path[-1]))

        for loc in path[::-1][2:]:
            plan.append(move(i_grid, prev_loc, loc))
            prev_loc = loc

        i_grid[y][x] += 1

    while i_grid != g_grid:

        # Pick highest position that is different
        different_positions = [
            (x, y)
            for x in range(grid_size_x)
            for y in range(grid_size_y)
            if i_grid[y][x] != g_grid[y][x]
            if accessible(i_grid, x, y, False)
        ]

        if not different_positions:
            print("Error: I cannot remove a block from the next position")
            print_grid(i_grid)
            print()
            print_grid(g_grid)
            exit()

        (x, y) = max(
            different_positions,
            key=lambda w: (i_grid[w[1]][w[0]], -order_grid[w[1]][w[0]]),
        )
        # print("Current grid")
        # print_grid(i_grid)
        # print("Goal grid")
        # print_grid(g_grid)
        # print("Candidates: ", different_positions)
        # print ("Remove", (x, y))

        path = get_shortest_path(i_grid, depots[0], (x, y), False, order_grid)
        if not path:
            return None
        prev_loc = path[0]
        for loc in path[1:-1]:
            plan.append(move(i_grid, prev_loc, loc))
            prev_loc = loc

        plan.append(remove_block(i_grid, path[-2], path[-1]))

        for loc in path[::-1][2:]:
            plan.append(move(i_grid, prev_loc, loc))
            prev_loc = loc

        i_grid[y][x] -= 1

        plan.append(f"(destroy-block {depot_loc})")

    while plan[-1].startswith("(move"):
        plan.pop()

    return plan


def parse_grid_line(line, robots, depots, y):
    content = (
        line.replace("  ", " ").replace("  ", " ").replace(" \n", "")[1:].split(" ")
    )

    for (x, t) in enumerate(content):
        if "R" in t:
            robots.append((x, y))

        if "D" in t:
            depots.append((x, y))

    return map(lambda x: int(x.replace("R", "").replace("D", "")), content)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("raw_instance", type=argparse.FileType("r"))
    args = parser.parse_args()

    i_grid = []
    g_grid = []
    robots = []
    depots = []

    lines = args.raw_instance.readlines()
    instance_name = lines[0][:-1]

    i = 0
    while not lines[i].startswith("Initial state:"):
        i += 1

    i += 1
    y = 0
    while not lines[i].startswith("Goal state:"):
        i_grid.append(parse_grid_line(lines[i], robots, depots, y))
        y += 1
        i += 1

    i += 1

    while not lines[i].startswith("Maximal height:"):
        g_grid.append(parse_grid_line(lines[i], [], [], 0))
        i += 1

    grid_size_y = len(i_grid)
    grid_size_x = len(i_grid[0])
    max_height = int(lines[i].split(":")[1].strip())

    assert len(robots) == 1
    assert len(depots) == 1
    assert robots[0] == depots[0]

    plan = solve(grid_size_x, grid_size_y, max_height, robots, depots, i_grid, g_grid)
    print("\n".join(plan))


if __name__ == "__main__":
    main()
