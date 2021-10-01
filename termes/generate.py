#!/usr/bin/env python

import argparse
import random

from solve import solve


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

    parser.add_argument("init", type=argparse.FileType("r"))
    parser.add_argument("goal", type=argparse.FileType("r"))
    parser.add_argument("--reduce_height_init", type=int, default=0)
    parser.add_argument("--reduce_height_goal", type=int, default=0)
    parser.add_argument("--grid_slack", type=int, default=0)
    parser.add_argument("--multi_robot", action="store_true")
    parser.add_argument("--ensure_plan", action="store_true")
    parser.add_argument("--store_plan", action="store_true")
    parser.add_argument("--dont_remove_slack", action="store_true")

    # parser.add_argument("random_seed", type=int)
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

    i_grid = [
        list(map(lambda x: max(0, int(x) - args.reduce_height_init), l.strip().split()))
        for l in args.init.readlines()
    ]
    g_grid = [
        list(map(lambda x: max(0, int(x) - args.reduce_height_goal), l.strip().split()))
        for l in args.goal.readlines()
    ]

    if not args.dont_remove_slack:
        i_grid = remove_slack(i_grid)
        g_grid = remove_slack(g_grid)

    grid_size_x = max(len(i_grid[0]), len(g_grid[0])) + args.grid_slack
    grid_size_y = max(len(i_grid), len(g_grid)) + args.grid_slack

    increase_x = grid_size_x < grid_size_y
    depots = None
    while not depots:
        dxinit = (grid_size_x - len(i_grid[0])) // 2 + (
            (grid_size_x - len(i_grid[0])) % 2
        )
        dyinit = (grid_size_y - len(i_grid)) // 2 + ((grid_size_y - len(i_grid)) % 2)

        dxgoal = (grid_size_x - len(g_grid[0])) // 2 + (
            (grid_size_x - len(g_grid[0])) % 2
        )
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
            if increase_x:
                grid_size_x += 1
            else:
                grid_size_y += 1
            increase_x = not increase_x

    robots = [depots[0]]  # A single robot, placed in the first depot

    max_height = max(list(map(max, init_grid)) + list(map(max, goal_grid)))

    problem_size = grid_size_x * grid_size_y * max_height

    if args.ensure_plan:
        plan = solve(
            grid_size_x, grid_size_y, max_height, robots, depots, init_grid, goal_grid
        )
        while not plan:
            if increase_x:
                grid_size_x += 1
            else:
                grid_size_y += 1
            increase_x = not increase_x

            dxinit = (grid_size_x - len(i_grid[0])) // 2 + (
                (grid_size_x - len(i_grid[0])) % 2
            )
            dyinit = (grid_size_y - len(i_grid)) // 2 + (
                (grid_size_y - len(i_grid)) % 2
            )

            dxgoal = (grid_size_x - len(g_grid[0])) // 2 + (
                (grid_size_x - len(g_grid[0])) % 2
            )
            dygoal = (grid_size_y - len(g_grid)) // 2 + (
                (grid_size_y - len(g_grid)) % 2
            )

            init_grid = [
                [get_value(i_grid, x, y, dxinit, dyinit) for x in range(0, grid_size_x)]
                for y in range(0, grid_size_y)
            ]
            goal_grid = [
                [get_value(g_grid, x, y, dxgoal, dygoal) for x in range(0, grid_size_x)]
                for y in range(0, grid_size_y)
            ]

            depots = place_depots(grid_size_x, grid_size_y, init_grid, goal_grid)
            robots = depots

            plan = solve(
                grid_size_x,
                grid_size_y,
                max_height,
                robots,
                depots,
                init_grid,
                goal_grid,
            )

    instance_name_list = ["termes"]

    if args.ensure_plan:
        instance_name_list.append(f"{len(plan):05d}")

    instance_name_list += [
        f"{problem_size:04d}",
        f"{grid_size_x}x{grid_size_y}x{max_height}",
    ]

    if not args.init.name.split("/")[-1].startswith("empty"):
        instance_name_list += [args.init.name.split("/")[-1].replace(".txt", "")]

    instance_name_list += [args.goal.name.split("/")[-1].replace(".txt", "")]

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
            args.multi_robot,
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


if __name__ == "__main__":
    main()
