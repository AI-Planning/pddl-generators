#!/usr/bin/env python3

import argparse
import random
import sys


def has_fixed_point(perm):
    return any(idx == num for idx, num in enumerate(perm))


def opposite_dir(direction):
    if direction == "in":
        return "out"
    else:
        return "in"


def create_pddl(half_segment_ids, problem_type, size, prob_no):
    objects = []
    init = []
    goal = []

    if problem_type == "simple":
        for seg in range(1, 2 * size + 1):
            for half in half_segment_ids:
                objects.append("seg%d%s - segment" % (seg, half))
                objects.append("car%d%s - car" % (seg, half))
                init.append("(on car%d%s seg%d%s)" % (seg, half, seg, half))
                goal.append("(on car%d%s seg%d%s)" % (seg, half, seg, half))
                goal.append("(analyzed car%d%s)" % (seg, half))

            if len(half_segment_ids) == 1:
                init.append("(CYCLE-2-WITH-ANALYSIS seg%d segback)" % seg)
            else:
                init.append(
                    "(CYCLE-4-WITH-ANALYSIS seg%da seg%db segbacka segbackb)"
                    % (seg, seg)
                )

        init.append("(= (total-cost) 0)")

        if len(half_segment_ids) == 1:
            objects.append("empty - car")
            objects.append("segback - segment")
            init.append("(on empty segback)")
        else:
            for half in half_segment_ids:
                objects.append("empty%s - car" % half)
                objects.append("segback%s - segment" % half)
                init.append(f"(on empty{half} segback{half})")
    else:

        if len(half_segment_ids) == 2:
            perms = {
                "in": [0] + list(range(size - 1, 0, -1)),
                "out": [0] + list(range(size - 1, 0, -1)),
            }

        for seg in range(1, size + 1):
            for direction in ["in", "out"]:
                for half in half_segment_ids:
                    objects.append("seg-%s-%d%s - segment" % (direction, seg, half))
                    objects.append("car-%s-%d%s - car" % (direction, seg, half))
                    init.append(
                        "(on car-%s-%d%s seg-%s-%d%s)"
                        % (direction, seg, half, direction, seg, half)
                    )
                    goal.append("(analyzed car-%s-%d%s)" % (direction, seg, half))

                if len(half_segment_ids) == 1:
                    for half in half_segment_ids:
                        goal.append(
                            "(on car-%s-%d%s seg-%s-%d%s)"
                            % (direction, seg, half, direction, seg, half)
                        )
                else:
                    goal.append(
                        "(on car-%s-%db seg-%s-%da)" % (direction, seg, direction, seg)
                    )
                    goal.append(
                        "(on car-%s-%da seg-%s-%db)"
                        % (
                            direction,
                            seg,
                            opposite_dir(direction),
                            perms[direction][seg - 1] + 1,
                        )
                    )

            for seg2 in range(1, size + 1):
                if len(half_segment_ids) == 1:
                    init.append("(CYCLE-2 seg-in-%d seg-out-%d)" % (seg, seg2))
                else:
                    init.append(
                        "(CYCLE-4 seg-in-%da seg-in-%db seg-out-%da seg-out-%db)"
                        % (seg, seg, seg2, seg2)
                    )

        for in_seg in range(1, problem_type[0] + 1):
            for out_seg in range(1, problem_type[1] + 1):
                if len(half_segment_ids) == 1:
                    init.append(
                        "(CYCLE-2-WITH-ANALYSIS seg-in-%d seg-out-%d)"
                        % (in_seg, out_seg)
                    )
                else:
                    init.append(
                        "(CYCLE-4-WITH-ANALYSIS seg-in-%da seg-in-%db seg-out-%da seg-out-%db)"
                        % (in_seg, in_seg, out_seg, out_seg)
                    )

        init.append("(= (total-cost) 0)")

    print(f"(define (problem scanalyzer3d-{prob_no})")
    print("  (:domain scanalyzer3d)")
    print("  (:objects")
    for objdesc in sorted(objects):
        print("    %s" % objdesc)
    print("  )")
    print("  (:init")
    for initdesc in sorted(init):
        print("    %s" % initdesc)
    print("  )")
    print("  (:goal (and")
    for goaldesc in sorted(goal):
        print("    %s" % goaldesc)
    print("  ))")
    print("  (:metric minimize (total-cost))")
    print(")")


def parse():
    parser = argparse.ArgumentParser()

    parser.add_argument("size", type=int, help="")
    parser.add_argument("segment_type", type=str, help="")
    parser.add_argument("inout", type=str, help="")
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--output", default=None)

    return parser.parse_args()


def main():
    args = parse()
    random.seed(args.seed)

    if args.output:
        sys.stdout = open(args.output, "w")

    prob_id = f"{args.size}-{args.segment_type}-{args.inout}"

    size = args.size
    if args.segment_type == "ab":
        half_segment_ids = ["a", "b"]
    else:
        half_segment_ids = [""]

    if args.inout == "in":
        (insize, outsize) = (size, 1)
    elif args.inout == "both":
        (insize, outsize) = (size, size)
    else:
        assert args.inout == "none"
        (insize, outsize) = (1, 1)

    create_pddl(half_segment_ids, (insize, outsize), size, prob_id)


if __name__ == "__main__":
    main()
