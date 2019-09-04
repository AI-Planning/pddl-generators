#! /usr/bin/env python

"""File:        generator.py
   Author:      Nathan Robinson
   Contact:     nathan.m.robinson@gmail.com
   Date:        2013-11-13

   Desctiption: Generate cave diver PDDL problems.

   Lisence:     Copyright (c) Year 2013, Nathan Robinson <nathan.m.robinson@gmail.com>
                                         Christian Muise <christian.muise@gmail.com>
                                         Charles Gretton <charles.gretto@gmail.com>

                Permission to use, copy, modify, and/or distribute this software for any
                purpose with or without fee is hereby granted, provided that the above
                copyright notice and this permission notice appear in all copies.

                THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
                WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
                MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
                SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
                WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
                ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
                IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

            To do:
            1. partial order reduction option
            2. Make it easier to specify what tunnels to make
            3. document and send in
"""

import itertools
import random

import generator_cmd_line

eps = 0.01


def make_caves(branch_depths):
    """ Generate a random tree with the branches of the given lengths.
        Return the nodes, the node depths, and the leaf nodes.

        ([int]) -> [(int, int)], [int], [int]
    """
    edges = [(x, x + 1) for x in range(branch_depths[0])]
    node_depths = list(range(branch_depths[0] + 1))
    nodes = branch_depths[0] + 1
    leaves = [nodes - 1]
    for branch in branch_depths[1:]:
        junction = random.choice([x for x in range(nodes) if node_depths[x] < branch])
        length = branch - node_depths[junction]
        edges.append((junction, nodes))
        node_depths.append(node_depths[junction] + 1)
        for nid, new_node in enumerate(range(nodes, nodes + length - 1)):
            edges.append((new_node, new_node + 1))
            node_depths.append(node_depths[junction] + 2 + nid)
        nodes += length
        leaves.append(nodes - 1)

    return edges, node_depths, leaves


def make_objectives(objective_depths, node_depths, leaves):
    """ Make num_objectives objectives at leaves with the specified depths.

        ([int], [int]) -> [int]
    """
    objectives = []
    for obj_d in objective_depths:
        candidates = [
            n for n in leaves if node_depths[n] == obj_d and n not in objectives
        ]
        if not candidates:
            raise Exception(
                "Not enough leaf nodes with depth " + str(obj_d) + " for objective."
            )
        objectives.append(random.choice(candidates))

    return objectives


def make_tanks_and_divers(
    node_depths, objectives, num_tank_adjustment, num_diver_adjustment, ordered_tanks
):
    """ Make the required number of tank and diver objects.
        |Tanks| = 2^(depth+1). (1 dummy (if ordered))
        |Divers| = 2^(depth-1)

        Adjust the number of tanks and divers by num_tank_adjustment and
        num_diver_adjustment. If either of these numbers is negative, the
        problem will be unsatisfiable.

        ([int], [int], int, int, bool) -> [str], [str]
    """
    num_tanks = num_tank_adjustment - 1
    num_divers = num_diver_adjustment
    for obj in objectives:
        num_tanks += pow(2, node_depths[obj] + 1)
        num_divers += pow(2, node_depths[obj] - 1)
    tanks = ["t" + str(x) for x in range(num_tanks)]
    if ordered_tanks:
        tanks.append("dummy")
    divers = ["d" + str(x) for x in range(num_divers)]

    return tanks, divers


def make_positive_relationships(objectives, node_depths):
    """ Create a (transitively closed) graph of relationships showing which
        divers depend on other divers to deliver them tanks to satisfy the
        objectives.

        Process - Start at each objective and walk back to the entrance keeping
        a list of divers as we go. At each step we need 1 additional diver to
        service each diver in our list.

        ([int], [int]) -> set([(int, int)])
    """
    cur_d = 0
    pos_relationships = set()
    for obj in objectives:
        obj_divers = [cur_d]
        cur_d += 1
        depth = node_depths[obj] - 1
        while depth > 0:
            new_divers = list(range(cur_d, cur_d + len(obj_divers)))
            for diver in obj_divers:
                for n_diver in new_divers:
                    pos_relationships.add((n_diver, diver))
            obj_divers.extend(new_divers)
            cur_d += len(new_divers)
            depth -= 1

    return pos_relationships


def make_negative_relationships(pos_relationships, num_divers, neg_link_prob):
    """ Make a set of negative relationships where divers preclude each other.

        For the problem to be satisfiable there must just be an ordering over
        the divers that works. Lets assume that the positive relationships
        represent this order. We are then able to rule out everything else.

        In fact, we we have a neg_link_prob chance of ruling out a non-positive
        link.

        (set([(int, int)]), int, ...) -> { int : [int] }
    """
    neg_relationships = dict([(x, list()) for x in range(num_divers)])
    for (diver1, diver2) in itertools.combinations(range(num_divers), 2):
        if (
            diver1,
            diver2,
        ) not in pos_relationships and random.random() < neg_link_prob:
            neg_relationships[diver1].append(diver2)

    return neg_relationships


def add_neg_cycle(neg_relationships, num_divers, neg_cycle_frac):
    """ Adds a negative cycle to the diver relationships, making the problem
        have no solutions. num_divers * neg_cycle_frac (min 2) divers are
        involved.

        ({ int  : [int] }, int, float) -> None
    """
    divers = random.sample(range(num_divers), max(2, int(num_divers * neg_cycle_frac)))
    for did, diver1 in enumerate(divers):
        diver2 = divers[(did + 1) % len(divers)]
        if diver2 not in neg_relationships[diver1]:
            neg_relationships[diver1].append(diver2)


def make_hiring_costs(neg_relationships, min_cost, max_cost, perturb):
    """ Make the hiring costs for the divers. The costs are inversely
        proportional to the number of negative relation ships a diver has.
        They are perturbed by perturb.

        ({ int  : [int] }, int, int, float) -> { int : int }
    """
    divers = list(neg_relationships.keys())
    num_rels = [len(x) for x in list(neg_relationships.values())]
    min_rels, max_rels = min(num_rels), max(num_rels)
    rel_range = max_rels - min_rels
    cost_range = max_cost - min_cost

    if cost_range == 0:
        return dict([(d, min_cost) for d in divers])

    if rel_range == 0:
        mid_cost = int(min_cost + cost_range / 2.0)
        return dict([(d, mid_cost) for d in divers])

    rel_dict = dict([(n, list()) for n in num_rels])
    for nid, num in enumerate(num_rels):
        rel_dict[num].append(divers[nid])
    sorted_rels = sorted(zip(list(rel_dict.keys()), list(rel_dict.values())))
    sorted_rels.reverse()

    hiring_costs = {}
    cost_inc = cost_range / float(len(sorted_rels))
    for rid, (nr, nr_d) in enumerate(sorted_rels):
        for d in nr_d:
            base_cost = min_cost + cost_inc * rid
            base_cost += random.random() * 2 * perturb * base_cost - perturb * base_cost
            base_cost = max(min_cost, min(max_cost, int(base_cost)))
            hiring_costs[d] = base_cost

    return hiring_costs


def write_domain_file(file_name, divers, neg_relationships, strips, ordered_tanks):
    """ Write the PDDL domain file to file_name.

        (str, [int], { int : [int] }, bool, bool) -> None
    """
    try:
        output_file = open(file_name, "w")

        if strips:
            output_file.write(";; Cave Diving STRIPS\n")
        else:
            output_file.write(";; Cave Diving ADL\n")
        output_file.write(";; Authors: Nathan Robinson,\n")
        output_file.write(";;          Christian Muise, and\n")
        output_file.write(";;          Charles Gretton\n\n")

        if strips:
            output_file.write("(define (domain cave-diving-strips)\n")
        else:
            output_file.write("(define (domain cave-diving-adl)\n")
        output_file.write("  (:requirements :typing :action-costs :adl)\n")
        output_file.write("  (:types location diver tank quantity)\n")
        output_file.write("  (:predicates\n")
        output_file.write("    (at-tank ?t - tank ?l - location)\n")
        output_file.write("    (in-storage ?t - tank)\n")
        output_file.write("    (full ?t - tank)\n")
        if ordered_tanks:
            output_file.write("    (next-tank ?t1 - tank ?t2 - tank)\n")
        output_file.write("    (at-diver ?d - diver ?l - location)\n")
        output_file.write("    (available ?d - diver)\n")
        output_file.write("    (at-surface ?d - diver)\n")
        output_file.write("    (decompressing ?d - diver)\n")
        if not strips:
            output_file.write("    (precludes ?d1 - diver ?d2 - diver)\n")
        output_file.write("    (cave-entrance ?l - location)\n")
        output_file.write("    (connected ?l1 - location ?l2 - location)\n")
        output_file.write("    (next-quantity ?q1 - quantity ?q2 - quantity)\n")
        output_file.write("    (holding ?d - diver ?t - tank)\n")
        output_file.write("    (capacity ?d - diver ?q - quantity)\n")
        output_file.write("    (have-photo ?l - location)\n")
        output_file.write("    (in-water )\n")

        output_file.write("  )\n\n")

        output_file.write("  (:functions\n")
        output_file.write("    (hiring-cost ?d - diver) - number\n")
        output_file.write("    (other-cost) - number\n")
        output_file.write("    (total-cost) - number\n")
        output_file.write("  )\n\n")

        if not strips:
            output_file.write("  (:action hire-diver\n")
            output_file.write("    :parameters (?d1 - diver)\n")
            output_file.write("    :precondition (and      (available ?d1)\n")
            output_file.write("                       (not (in-water)) \n")
            output_file.write("                  )\n")
            output_file.write("    :effect (and (at-surface ?d1)\n")
            output_file.write("                 (not (available ?d1))\n")
            output_file.write("                 (forall (?d2 - diver)\n")
            output_file.write(
                "                     (when (precludes ?d1 ?d2) (not (available ?d2))))\n"
            )
            output_file.write("                 (in-water)\n")
            output_file.write(
                "                 (increase (total-cost) (hiring-cost ?d1))\n"
            )
            output_file.write("            )\n")
            output_file.write("  )\n\n")
        else:
            for did, diver1 in enumerate(divers):
                output_file.write("  (:action hire-diver-" + diver1 + "\n")
                output_file.write("    :parameters ( )\n")
                output_file.write(
                    "    :precondition (and (available " + diver1 + "))\n"
                )
                output_file.write("    :effect (and (at-surface " + diver1 + ")\n")
                output_file.write("                 (not (available " + diver1 + "))\n")
                for diver2 in neg_relationships[did]:
                    output_file.write(
                        "                 (not (available " + divers[diver2] + "))\n"
                    )
                output_file.write(
                    "                 (increase (total-cost) (hiring-cost "
                    + diver1
                    + "))\n"
                )
                output_file.write("            )\n")
                output_file.write("  )\n\n")

        output_file.write("  (:action prepare-tank\n")
        if ordered_tanks:
            output_file.write(
                "    :parameters (?d - diver ?t1 ?t2 - tank ?q1 ?q2 - quantity)\n"
            )
        else:
            output_file.write(
                "    :parameters (?d - diver ?t1 - tank ?q1 ?q2 - quantity)\n"
            )
        output_file.write("    :precondition (and (at-surface ?d)\n")
        output_file.write("                       (in-storage ?t1)\n")
        output_file.write("                       (next-quantity ?q1 ?q2)\n")
        output_file.write("                       (capacity ?d ?q2)\n")
        if ordered_tanks:
            output_file.write("                       (next-tank ?t1 ?t2)\n")
        output_file.write("                  )\n")
        output_file.write("    :effect (and (not (in-storage ?t1))\n")
        output_file.write("                 (not (capacity ?d ?q2))\n")
        if ordered_tanks:
            output_file.write("                      (in-storage ?t2)\n")
        output_file.write("                      (full ?t1)\n")
        output_file.write("                      (capacity ?d ?q1)\n")
        output_file.write("                      (holding ?d ?t1)\n")
        output_file.write("                 (increase (total-cost) (other-cost ))\n")
        output_file.write("            )\n")
        output_file.write("  )\n\n")

        output_file.write("  (:action enter-water\n")
        output_file.write("    :parameters (?d - diver ?l - location)\n")
        output_file.write("    :precondition (and (at-surface ?d)\n")
        output_file.write("                       (cave-entrance ?l)\n")
        output_file.write("                  )\n")
        output_file.write("    :effect (and (not (at-surface ?d))\n")
        output_file.write("                      (at-diver ?d ?l)\n")
        output_file.write("                 (increase (total-cost) (other-cost ))\n")
        output_file.write("            )\n")
        output_file.write("  )\n\n")

        output_file.write("  (:action pickup-tank\n")
        output_file.write(
            "    :parameters (?d - diver ?t - tank ?l - location ?q1 ?q2 - quantity)\n"
        )
        output_file.write("    :precondition (and (at-diver ?d ?l)\n")
        output_file.write("                       (at-tank ?t ?l)\n")
        output_file.write("                       (next-quantity ?q1 ?q2)\n")
        output_file.write("                       (capacity ?d ?q2)\n")
        output_file.write("                  )\n")
        output_file.write("    :effect (and (not (at-tank ?t ?l))\n")
        output_file.write("                 (not (capacity ?d ?q2))\n")
        output_file.write("                      (holding ?d ?t)\n")
        output_file.write("                      (capacity ?d ?q1)\n")
        output_file.write("                 (increase (total-cost) (other-cost ))\n")
        output_file.write("            )\n")
        output_file.write("  )\n\n")

        output_file.write("  (:action drop-tank\n")
        output_file.write(
            "    :parameters (?d - diver ?t - tank ?l - location ?q1 ?q2 - quantity)\n"
        )
        output_file.write("    :precondition (and (at-diver ?d ?l)\n")
        output_file.write("                       (holding ?d ?t)\n")
        output_file.write("                       (next-quantity ?q1 ?q2)\n")
        output_file.write("                       (capacity ?d ?q1)\n")
        output_file.write("                  )\n")
        output_file.write("    :effect (and (not (holding ?d ?t))\n")
        output_file.write("                 (not (capacity ?d ?q1))\n")
        output_file.write("                      (at-tank ?t ?l)\n")
        output_file.write("                      (capacity ?d ?q2)\n")
        output_file.write("                 (increase (total-cost) (other-cost ))\n")
        output_file.write("            )\n")
        output_file.write("  )\n\n")

        output_file.write("  (:action swim\n")
        output_file.write("    :parameters (?d - diver ?t - tank ?l1 ?l2 - location)\n")
        output_file.write("    :precondition (and (at-diver ?d ?l1)\n")
        output_file.write("                       (holding ?d ?t)\n")
        output_file.write("                       (full ?t)\n")
        output_file.write("                       (connected ?l1 ?l2)\n")
        output_file.write("                  )\n")
        output_file.write("    :effect (and (not (at-diver ?d ?l1))\n")
        output_file.write("                 (not (full ?t))\n")
        output_file.write("                      (at-diver ?d ?l2)\n")
        output_file.write("                 (increase (total-cost) (other-cost ))\n")
        output_file.write("            )\n")
        output_file.write("  )\n\n")

        output_file.write("  (:action photograph\n")
        output_file.write("    :parameters (?d - diver ?l - location ?t - tank)\n")
        output_file.write("    :precondition (and (at-diver ?d ?l)\n")
        output_file.write("                       (holding ?d ?t)\n")
        output_file.write("                       (full ?t)\n")
        output_file.write("                  )\n")
        output_file.write("    :effect (and (not (full ?t))\n")
        output_file.write("                      (have-photo ?l)\n")
        output_file.write("                 (increase (total-cost) (other-cost ))\n")
        output_file.write("            )\n")
        output_file.write("  )\n\n")

        output_file.write("  (:action decompress\n")
        output_file.write("    :parameters (?d - diver ?l - location)\n")
        output_file.write("    :precondition (and (at-diver ?d ?l)\n")
        output_file.write("                       (cave-entrance ?l)\n")
        output_file.write("                  )\n")
        output_file.write("    :effect (and (not (at-diver ?d ?l))\n")
        output_file.write("                      (decompressing ?d)\n")
        output_file.write("                 (not (in-water))\n")
        output_file.write("                 (increase (total-cost) (other-cost ))\n")
        output_file.write("            )\n")
        output_file.write("  )\n\n")

        output_file.write(")\n")
        output_file.close()
    except IOError:
        print("Error: could not write to the domain file:", file_name)


def write_problem_file(
    file_name,
    problem_name,
    num_locations,
    tanks,
    divers,
    objectives,
    edges,
    neg_relationships,
    hiring_costs,
    other_action_cost,
    strips,
    ordered_tanks,
):
    """ Write the PDDL problem file to file_name.

        (str, str, int, [str], [str], [int], [(int, int)], { int : [int] },
            { int : int }, int, bool, bool) -> None
    """
    try:
        output_file = open(file_name, "w")

        if strips:
            output_file.write(";; Cave Diving STRIPS\n")
        else:
            output_file.write(";; Cave Diving ADL\n")
        output_file.write(";; Authors: Nathan Robinson,\n")
        output_file.write(";;          Christian Muise, and\n")
        output_file.write(";;          Charles Gretton\n\n")

        if strips:
            output_file.write(
                "(define (problem cave-diving-strips-" + problem_name + ")\n"
            )
            output_file.write("  (:domain cave-diving-strips)\n")
        else:
            output_file.write(
                "(define (problem cave-diving-adl-" + problem_name + ")\n"
            )
            output_file.write("  (:domain cave-diving-adl)\n")

        output_file.write("  (:objects\n")
        output_file.write(
            "    "
            + " ".join(["l" + str(x) for x in range(num_locations)])
            + " - location\n"
        )
        num_diver_lines = len(divers) // 20 + 1
        ordered_divers = ["d" + str(x) for x in range(len(divers))]
        for d_line in range(num_diver_lines):
            output_file.write(
                "    "
                + " ".join(ordered_divers[(d_line * 20): (d_line * 20 + 20)])
                + " - diver\n"
            )
        num_tank_lines = len(tanks) // 20 + 1
        for t_line in range(num_tank_lines):
            output_file.write(
                "    "
                + " ".join(tanks[(t_line * 20): (t_line * 20 + 20)])
                + " - tank\n"
            )
        output_file.write("    zero one two three four - quantity\n")
        output_file.write("  )\n\n")

        output_file.write("  (:init\n")
        for diver in ordered_divers:
            output_file.write("    (available " + diver + ")\n")
        for diver in ordered_divers:
            output_file.write("    (capacity " + diver + " four)\n")

        if ordered_tanks:
            output_file.write("    (in-storage " + tanks[0] + ")\n")
            for tid, tank in enumerate(tanks[:-1]):
                output_file.write(
                    "    (next-tank " + tank + " " + tanks[tid + 1] + ")\n"
                )
        else:
            for tank in tanks:
                output_file.write("    (in-storage " + tank + ")\n")

        output_file.write("    (cave-entrance l0)\n")

        for edge in edges:
            output_file.write(
                "    (connected l" + str(edge[0]) + " l" + str(edge[1]) + ")\n"
            )
            output_file.write(
                "    (connected l" + str(edge[1]) + " l" + str(edge[0]) + ")\n"
            )

        output_file.write("    (next-quantity zero one)\n")
        output_file.write("    (next-quantity one two)\n")
        output_file.write("    (next-quantity two three)\n")
        output_file.write("    (next-quantity three four)\n")

        if not strips:
            for did1, diver1 in enumerate(divers):
                for diver2 in neg_relationships[did1]:
                    output_file.write(
                        "    (precludes " + diver1 + " " + divers[diver2] + ")\n"
                    )

        for did, diver in enumerate(divers):
            output_file.write(
                "    (= (hiring-cost " + diver + ") " + str(hiring_costs[did]) + ")\n"
            )

        output_file.write("    (= (other-cost ) " + str(other_action_cost) + ")\n")

        output_file.write("    (= (total-cost) 0)\n")
        output_file.write("  )\n\n")

        output_file.write("  (:goal\n")
        output_file.write("    (and\n")
        for obj in objectives:
            output_file.write("      (have-photo l" + str(obj) + ")\n")
        for diver in divers:
            output_file.write("      (decompressing " + diver + ")\n")
        output_file.write("    )\n  )\n\n")
        output_file.write("  (:metric minimize (total-cost))\n\n")
        output_file.write(")\n")

        output_file.close()
    except IOError:
        print("Error: could not write to the problem file:", file_name)


def main():
    args = generator_cmd_line.process_args()
    random.seed(args.seed)

    edges, node_depths, leaves = make_caves(args.cave_branches)

    objectives = make_objectives(args.objectives, node_depths, leaves)

    tanks, divers = make_tanks_and_divers(
        node_depths,
        objectives,
        args.num_tank_adjustment,
        args.num_diver_adjustment,
        args.order_tanks,
    )

    pos_relationships = make_positive_relationships(objectives, node_depths)

    neg_relationships = make_negative_relationships(
        pos_relationships, len(divers), args.neg_link_prob
    )

    if args.neg_cycle_length:
        add_neg_cycle(neg_relationships, len(divers), args.neg_cycle_length)

    hiring_costs = make_hiring_costs(
        neg_relationships,
        args.minimum_hiring_cost,
        args.maximum_hiring_cost,
        args.perturb_hiring_costs,
    )

    random.shuffle(divers)

    if not args.quiet:
        print()
        print("Edges: ", ", ".join(map(str, edges)))
        print("Depths:", ", ".join(map(str, node_depths)))
        print("Objectives:", ", ".join(map(str, objectives)))
        print("Tanks:", ", ".join(map(str, tanks)))
        print("Divers:", ", ".join(map(str, divers)))
        print("Positive relationships:", ", ".join(map(str, pos_relationships)))
        print("Negative relationships:", neg_relationships)
        print("Hiring costs:", hiring_costs)

    if args.domain_file_name:
        write_domain_file(
            args.domain_file_name,
            divers,
            neg_relationships,
            args.strips,
            args.order_tanks,
        )

    write_problem_file(
        args.problem_file_name,
        args.problem_name,
        len(node_depths),
        tanks,
        divers,
        objectives,
        edges,
        neg_relationships,
        hiring_costs,
        args.other_action_cost,
        args.strips,
        args.order_tanks,
    )


if __name__ == "__main__":
    main()
