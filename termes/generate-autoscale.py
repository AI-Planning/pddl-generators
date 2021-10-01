#!/usr/bin/env python

# This generator is very similar to the one used in IPC'18 for the TERMES domain. The main differences are:
#
# 1) We restrict ourselves to problems that go from an empty to a random board (in the IPC
#    all selected instances started in an empty board, but some instances were based on
#    manually-designed boards for the goal).
#
# 2) We add an extra parameter to control the instances generated: number of blocks.
#
# 3) We modify the output of a plan, so that it is included in the problem file.


import argparse
import random
import numpy
from solve import solve

import gen_random_tower_boards


def generate_readable(
    instance_name, grid_size_x, grid_size_y, max_height, robots, depots, i_grid, g_grid
):
    assert len(i_grid) == grid_size_y
    assert len(i_grid[0]) == grid_size_x
    lines = [instance_name]
    lines += ["Initial state:"]
    for y in range(grid_size_y):
        cells = []
        for x in range(grid_size_x):
            r = "R" if (x, y) in robots else " "
            h = str(i_grid[y][x])
            d = "D" if (x, y) in depots else " "
            cells.append(r + h + d)
        lines += [" ".join(cells)]
    lines += ["Goal state:"]
    for y in range(grid_size_y):
        cells = []
        for x in range(grid_size_x):
            if g_grid[y][x] is None:
                cells += [" * "]
            else:
                cells += [" %s " % g_grid[y][x]]
        lines += [" ".join(cells)]
    lines += ["Maximal height: %d" % max_height]
    return lines


def generate_for_pddl(
    instance_name,
    grid_size_x,
    grid_size_y,
    max_height,
    robots,
    depots,
    i_grid,
    g_grid,
    MULTI_ROBOT,
):
    comments = generate_readable(
        instance_name,
        grid_size_x,
        grid_size_y,
        max_height,
        robots,
        depots,
        i_grid,
        g_grid,
    )

    objects = []
    objects += ["n%d - numb" % (n) for n in range(max_height + 1)]
    objects += [
        "pos-%d-%d - position" % (x, y)
        for x in range(grid_size_x)
        for y in range(grid_size_y)
    ]

    facts = []
    facts += [
        "(height pos-%d-%d n%d)" % (x, y, i_grid[y][x])
        for x in range(grid_size_x)
        for y in range(grid_size_y)
    ]

    if MULTI_ROBOT:
        objects += ["r-%s - robot" % robot_id for robot_id in range(len(robots))]
        facts += ["(occupied pos-%d-%d)" % (x, y) for (x, y) in robots]
        facts += [
            "(at r-%d pos-%d-%d)" % (robot_id, x, y)
            for robot_id, (x, y) in enumerate(robots)
        ]
    else:
        assert len(robots) == 1
        facts += ["(at pos-%d-%d)" % (x, y) for (x, y) in robots]

    # Static predicates
    facts += ["(SUCC n%d n%d)" % (n + 1, n) for n in range(max_height)]
    facts += [
        "(NEIGHBOR pos-%d-%d pos-%d-%d)" % (x, y, x + dx, y + dy)
        for x in range(grid_size_x)
        for y in range(grid_size_y)
        for dx, dy in [(-1, 0), (1, 0), (0, -1), (0, 1)]
        if 0 <= x + dx < grid_size_x and 0 <= y + dy < grid_size_y
    ]
    facts += ["(IS-DEPOT pos-%d-%d)" % (x, y) for (x, y) in depots]

    goals = [
        "(height pos-%d-%d n%d)" % (x, y, g_grid[y][x])
        for x in range(grid_size_x)
        for y in range(grid_size_y)
        if g_grid[y][x] is not None
    ]
    goals += ["(not (has-block))"]

    lines = []
    lines += ["(define (problem %s)" % instance_name]
    lines += ["(:domain termes)"]
    lines += ["; %s" % c for c in comments]
    lines += ["(:objects"]
    lines += ["    %s" % o for o in objects]
    lines += [")", "(:init"]
    lines += ["    %s" % f for f in facts]
    lines += [")", "(:goal (and"]
    lines += ["    %s" % g for g in goals]
    lines += ["))"]
    lines += [")"]
    return lines


def remove_slack(grid):
    num_rows = len(grid)
    num_columns = len(grid[0])
    non_null_rows = sorted(
        {y for x in range(num_columns) for y in range(num_rows) if grid[y][x] > 0}
    )

    if not non_null_rows:
        return [[0]]

    grid = grid[non_null_rows[0] : non_null_rows[-1] + 1]

    num_rows = len(grid)
    num_columns = len(grid[0])

    non_null_columns = sorted(
        {x for x in range(num_columns) for y in range(num_rows) if grid[y][x] > 0}
    )

    grid = list(map(lambda x: x[non_null_columns[0] : non_null_columns[-1] + 1], grid))

    return grid


def get_value(grid, x, y, dx, dy):
    rx = x - dx
    ry = y - dy

    if ry < 0 or ry >= len(grid) or rx < 0 or rx >= len(grid[ry]):
        return 0

    return grid[ry][rx]


def parse():
    parser = argparse.ArgumentParser()

    parser.add_argument("--size_x", type=int, default=4)
    parser.add_argument("--size_y", type=int, default=4)
    parser.add_argument("--min_height", type=int, default=1)
    parser.add_argument("--max_height", type=int, default=4)
    parser.add_argument("--num_towers", type=int, default=4)

    parser.add_argument("--ensure_plan_tries", type=int, default=20)

    parser.add_argument("--ensure_plan", action="store_true")
    parser.add_argument("--dont_remove_slack", action="store_true")
    parser.add_argument("--store_plan", action="store_true")

    parser.add_argument("random_seed", type=int)
    parser.add_argument("output_type", choices=["pddl", "pddlfile", "readable"])
    return parser.parse_args()


def place_depots(grid_size_x, grid_size_y, init_grid, goal_grid):
    candidate_cells = [
        (y, abs(x - grid_size_x / 2), x, y)
        for x in range(grid_size_x)
        for y in range(grid_size_y)
        if init_grid[y][x] == 0 and goal_grid[y][x] == 0
    ]
    if not candidate_cells:
        return None
    selected = min(candidate_cells)
    return [(selected[2], selected[3])]


def main():
    args = parse()

    assert args.min_height <= args.max_height

    rng = numpy.random.default_rng(args.random_seed)

    increase_x = args.size_x < args.size_y

    plan = None
    tries = 0
    while not plan:
        tries += 1
        if (
            tries > args.ensure_plan_tries
            or args.num_towers > args.size_x * args.size_y
        ):
            tries = 0
            if increase_x:
                args.size_x += 1
            else:
                args.size_y += 1
            increase_x = not increase_x
            continue

        problem = gen_problem(args, rng)
        if problem:
            (
                grid_size_x,
                grid_size_y,
                max_height,
                init_grid,
                goal_grid,
                depots,
                robots,
            ) = problem
        else:
            continue

        if not args.ensure_plan:
            break

        plan = solve(
            grid_size_x,
            grid_size_y,
            max_height,
            robots,
            depots,
            init_grid,
            goal_grid,
            minimize=False,
        )

        # tries = 0
        # orig_size_x = grid_size_x
        # orig_size_y = grid_size_y

        # while not plan:
        #     print(tries)
        #     tries += 1
        #     # if tries == args.ensure_plan_tries:
        #     #     tries = 0

        #     if grid_size_x - orig_size_x <= grid_size_y  - orig_size_y:
        #         grid_size_x += 1
        #     else:
        #         grid_size_y += 1

    instance_name_list = ["termes"]

    # if args.ensure_plan:
    #     instance_name_list.append("{:05d}".format(len(plan)))
    # We do not consider plan length, as we are not optimizing
    problem_size = grid_size_x * grid_size_y * max_height

    instance_name_list += [
        f"{problem_size:04d}",
        "{}x{}x{}".format(
            grid_size_x, grid_size_y, max_height, args.min_height, args.num_towers
        ),
    ]

    instance_name = "-".join(instance_name_list)

    if args.output_type.startswith("pddl"):
        lines = generate_for_pddl(
            instance_name,
            grid_size_x,
            grid_size_y,
            max_height,
            robots,
            depots,
            init_grid,
            goal_grid,
            False,
        )
        if args.output_type == "pddl":
            print("\n".join(lines))
        else:
            f = open(f"instances/p-{instance_name}.pddl", "w")
            f.write("\n".join(lines))
            f.close()
    elif args.output_type == "readable":
        lines = generate_readable(
            instance_name,
            grid_size_x,
            grid_size_y,
            max_height,
            robots,
            depots,
            init_grid,
            goal_grid,
        )
        print("\n".join(lines))

    if args.store_plan:
        if not plan:
            plan = solve(
                grid_size_x,
                grid_size_y,
                max_height,
                robots,
                depots,
                init_grid,
                goal_grid,
            )
        if plan:
            f = open(f"plans/p-{instance_name}.pddl", "w")
            f.write("\n".join(plan))
            f.close()


def gen_problem(args, rng):

    grid_size_x = args.size_x
    grid_size_y = args.size_y
    max_height = args.max_height

    i_grid = [[0]]
    g_grid = gen_random_tower_boards.gen_board_autoscale(
        grid_size_x, grid_size_y, args.min_height, args.max_height, args.num_towers, rng
    )

    if not args.dont_remove_slack:
        i_grid = remove_slack(i_grid)
        g_grid = remove_slack(g_grid)

    dxinit = (grid_size_x - len(i_grid[0])) // 2 + ((grid_size_x - len(i_grid[0])) % 2)
    dyinit = (grid_size_y - len(i_grid)) // 2 + ((grid_size_y - len(i_grid)) % 2)

    dxgoal = (grid_size_x - len(g_grid[0])) // 2 + ((grid_size_x - len(g_grid[0])) % 2)
    dygoal = (grid_size_y - len(g_grid)) // 2 + ((grid_size_y - len(g_grid)) % 2)

    init_grid = [
        [get_value(i_grid, x, y, dxinit, dyinit) for x in range(0, grid_size_x)]
        for y in range(0, grid_size_y)
    ]
    goal_grid = [
        [get_value(g_grid, x, y, dxgoal, dygoal) for x in range(0, grid_size_x)]
        for y in range(0, grid_size_y)
    ]

    depots = place_depots(grid_size_x, grid_size_y, init_grid, goal_grid)
    if not depots:
        return None

    robots = [depots[0]]  # A single robot, placed in the first depot

    return grid_size_x, grid_size_y, max_height, init_grid, goal_grid, depots, robots


if __name__ == "__main__":
    main()
