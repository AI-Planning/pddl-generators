#!/usr/bin/env python

from __future__ import print_function

from math import ceil

import random
import sys

_colours = ["black", "white", "red", "green", "blue", "mauve"]
_woods = ["oak", "pine", "cherry", "teak", "mahogany", "beech", "walnut"]
_machines = [
    "highspeed-saw",
    "glazer",
    "grinder",
    "immersion-varnisher",
    "planer",
    "saw",
    "spray-varnisher",
]
_boardsize_prefix = "s"


def default_machines(num_machines):
    return {machine: num_machines for machine in _machines}


class Part(object):
    def __init__(self, index, woods, colours, problemtype=None):
        self.name = "p%d" % index
        self.problemtype = problemtype
        self.generate_random_goal(woods, colours)
        self.generate_random_init(woods, colours)

    def generate_random_goal(self, woods, colours):
        self.goalprops = dict()
        self.goalprops["treatment"] = random.choice(["varnished", "glazed"])
        self.goalprops["surface-condition"] = random.choice(["smooth", "verysmooth"])

        if self.problemtype != "painting":
            colours = colours + ["natural"]

        self.goalprops["colour"] = random.choice(colours)
        self.goalprops["wood"] = random.choice(woods)
        self.generate_goal_selection()

    def generate_goal_selection(self, nr_goals=None):
        nr_goals = random.choice([2, 2, 2, 3, 4])
        self.goalselection = set(random.sample(list(self.goalprops), nr_goals))

    def generate_random_init(self, woods, colours):
        def gen_preprocessing_status():
            self.initprops["treatment"] = random.choice(
                ["varnished", "glazed", "colourfragments"]
            )
            self.initprops["surface-condition"] = random.choice(
                ["rough", "smooth", "verysmooth"]
            )
            self.initprops["wood"] = self.goalprops["wood"]

            poss_colours = set(colours + ["natural"]) - set([self.goalprops["colour"]])
            self.initprops["colour"] = random.choice(list(poss_colours))

        self.initprops = dict()

        status = random.choice(["unused"] * 4 + ["available"])
        if status != "unused":
            while True:
                gen_preprocessing_status()
                goal_initially_satisfied = True
                for prop in self.goalselection:
                    if self.initprops[prop] != self.goalprops[prop]:
                        goal_initially_satisfied = False
                        break

                if not goal_initially_satisfied:
                    break

                self.generate_goal_selection(len(self.goalselection))

        self.size = random.randint(1, 3)

    def dump_init(self, indent="", out=None):
        if self.initprops:
            print("%s(available %s)" % (indent, self.name), file=out)

            for prop, val in self.initprops.items():
                print("%s(%s %s %s)" % (indent, prop, self.name, val), file=out)
        else:
            print("%s(unused %s)" % (indent, self.name), file=out)

        print(
            "%s(goalsize %s %s)"
            % (indent, self.name, ["small", "medium", "large"][self.size - 1]),
            file=out,
        )
        print(
            "%s(= (spray-varnish-cost %s) %s)" % (indent, self.name, self.size * 5),
            file=out,
        )
        print(
            "%s(= (glaze-cost %s) %d)" % (indent, self.name, self.size * 5 + 5),
            file=out,
        )
        print(
            "%s(= (grind-cost %s) %d)" % (indent, self.name, self.size * 15), file=out
        )
        print(
            "%s(= (plane-cost %s) %d)" % (indent, self.name, self.size * 10), file=out
        )

    def dump_goal(self, indent="", out=None):
        print("%s(available %s)" % (indent, self.name), file=out)

        for choice in self.goalselection:
            print(
                "%s(%s %s %s)" % (indent, choice, self.name, self.goalprops[choice]),
                file=out,
            )


class Board(object):
    def __init__(self, index, wood, size):
        self.wood = wood
        self.name = "b%d" % index
        self.size = size
        self.surface = random.choice(["rough", "rough", "rough", "smooth"])

    def dump_init(self, indent="", out=None):
        print(
            "%s(boardsize %s %s%d)" % (indent, self.name, _boardsize_prefix, self.size),
            file=out,
        )
        print("%s(wood %s %s)" % (indent, self.name, self.wood), file=out)
        print(
            "%s(surface-condition %s %s)" % (indent, self.name, self.surface), file=out
        )
        print("%s(available %s)" % (indent, self.name), file=out)


class Machine(object):
    def __init__(self, name, type):
        self.name = name
        self.type = type
        self.colours = set()

    def dump_init(self, indent, out=None):
        if self.type == "highspeed-saw":
            print("%s(empty %s)" % (indent, self.name), file=out)

        for colour in self.colours:
            print("%s(has-colour %s %s)" % (indent, self.name, colour), file=out)


class Task(object):
    def __init__(
        self,
        nr_parts,
        nr_machines,
        wood_factor=1.0,
        problemtype=None,
        **additional_machines
    ):
        self.problemtype = problemtype
        self.wood_factor = wood_factor

        nr_colours = max(int(round(0.7 * nr_parts)), 2)
        self.colours = random.sample(_colours, min(nr_colours, len(_colours)))

        nr_woods = max(int(round(0.25 * nr_parts)), 2)
        self.woods = random.sample(_woods, min(nr_woods, len(_woods)))

        self.parts = [
            Part(nr, self.woods, self.colours, problemtype) for nr in range(nr_parts)
        ]

        self.max_board_size = self._generate_boards()

        self._generate_machines(nr_machines, **additional_machines)
        self._assign_colours_to_machines()

    def _generate_boards(self):
        quantities = dict()

        for part in [p for p in self.parts if not p.initprops]:
            wood = part.goalprops["wood"]
            quantities.setdefault(wood, []).append(part.size)

        self.boards = []
        counter = 0
        maxsize = 0
        for wood, quantity in quantities.items():
            if not quantity:
                self.woods.remove("wood")
            else:
                additional_wood = int(ceil(sum(quantity) * (self.wood_factor - 1)))

                while additional_wood > 3:
                    rand = random.randint(1, 3)
                    quantity.append(rand)
                    additional_wood -= rand

                quantity.append(additional_wood)
                random.shuffle(quantity)
                for amounts in zip(
                    quantity[::4],
                    quantity[1::4] + [0],
                    quantity[2::4] + [0],
                    quantity[3::4] + [0],
                ):
                    size = sum(amounts)
                    maxsize = max(maxsize, size)
                    self.boards.append(Board(counter, wood, size))
                    counter += 1

        return maxsize

    def _generate_machines(self, nr_machines, **changes):
        machines = dict(default_machines(nr_machines))
        machines.update(changes)

        self.machines = dict()
        for type, number in machines.items():
            if number:
                m = [Machine("%s%s" % (type, nr), type) for nr in range(number)]
                self.machines[type] = m

    def _assign_colours_to_machines(self):
        necessary = self._determine_necessary_colours()

        spray_varnishers = self.machines.get("spray-varnisher", [])
        immersion_varnishers = self.machines.get("immersion-varnisher", [])
        glazers = self.machines.get("glazer", [])

        for colour in necessary["varnished"]:
            machines = random.sample(
                spray_varnishers, int(ceil(0.6 * len(spray_varnishers)))
            )
            machines += random.sample(
                immersion_varnishers, int(ceil(0.6 * len(immersion_varnishers)))
            )

            for m in machines:
                m.colours.add(colour)

        for colour in necessary["glazed"]:
            for m in random.sample(glazers, int(ceil(0.6 * len(glazers)))):
                m.colours.add(colour)

        for colour in necessary["unspecified"]:
            machines = random.sample(
                spray_varnishers, int(ceil(0.3 * len(spray_varnishers)))
            )
            machines += random.sample(
                immersion_varnishers, int(ceil(0.3 * len(immersion_varnishers)))
            )
            machines += random.sample(glazers, int(ceil(0.3 * len(glazers))))

            for m in machines:
                m.colours.add(colour)

        for type in ["spray-varnisher", "immersion-varnisher", "glazer"]:
            for m in self.machines.get(type, []):
                if not m.colours:
                    m.colours.add(random.choice(list(self.colours)))

    def _determine_necessary_colours(self):
        used_colours = dict()
        used_colours["varnished"] = set()
        used_colours["glazed"] = set()
        used_colours["unspecified"] = set()

        for part in self.parts:
            if "colour" in part.goalselection:
                colour = part.goalprops["colour"]

                if "treatment" in part.goalselection:
                    used_colours[part.goalprops["treatment"]].add(colour)
                else:
                    used_colours["unspecified"].add(colour)

        return used_colours

    def dump(self, out=None):
        self._dump_header(out)
        self._dump_objects("  ", out)
        self._dump_init("  ", out)
        self._dump_goal("  ", out)
        self._dump_metric("  ", out)

        print(")", file=out)

    def _dump_header(self, out=None):
        print(
            "; woodworking task with %s parts and %d%% wood"
            % (len(self.parts), self.wood_factor * 100),
            file=out,
        )
        print("; Machines:", file=out)

        for type, machines in self.machines.items():
            print(";   %d %s" % (len(machines), type), file=out)

        print("", file=out)
        print("(define (problem wood-prob)", file=out)
        print("  (:domain woodworking)", file=out)

    def _dump_objects(self, indent="", out=None):
        print(indent + "(:objects", file=out)

        for type, machines in self.machines.items():
            print(
                "%s  %s - %s" % (indent, " ".join([m.name for m in machines]), type),
                file=out,
            )

        print("%s  %s - acolour" % (indent, " ".join(self.colours)), file=out)
        print("%s  %s - awood" % (indent, " ".join(self.woods)), file=out)
        print(
            "%s  %s - part" % (indent, " ".join([p.name for p in self.parts])), file=out
        )
        print(
            "%s  %s - board" % (indent, " ".join([b.name for b in self.boards])),
            file=out,
        )

        boardsizes = [
            "%s%d" % (_boardsize_prefix, size)
            for size in range(self.max_board_size + 1)
        ]

        print("%s  %s - aboardsize" % (indent, " ".join(boardsizes)), file=out)
        print(indent + ")", file=out)

    def _dump_init(self, indent="", out=None):
        print(indent + "(:init", file=out)
        print(indent + "  (grind-treatment-change varnished colourfragments)", file=out)
        print(indent + "  (grind-treatment-change glazed untreated)", file=out)
        print(indent + "  (grind-treatment-change untreated untreated)", file=out)
        print(indent + "  (grind-treatment-change colourfragments untreated)", file=out)
        print(indent + "  (is-smooth smooth)", file=out)
        print(indent + "  (is-smooth verysmooth)", file=out)
        print(indent + "  (= (total-cost) 0)", file=out)

        for size in range(self.max_board_size):
            print(
                "%s  (boardsize-successor %s%d %s%d)"
                % (indent, _boardsize_prefix, size, _boardsize_prefix, size + 1),
                file=out,
            )

        for type in self.machines:
            for m in self.machines[type]:
                m.dump_init(indent + "  ", out)

        for part in self.parts:
            part.dump_init(indent + "  ", out)

        for board in self.boards:
            board.dump_init(indent + "  ", out)

        print(indent + ")", file=out)

    def _dump_goal(self, indent="", out=None):
        print(indent + "(:goal", file=out)
        print(indent + "  (and", file=out)

        for part in self.parts:
            part.dump_goal(indent + "  ", out)

        print(indent + "  )", file=out)
        print(indent + ")", file=out)

    def _dump_metric(self, indent="", out=None):
        print(indent + "(:metric minimize (total-cost))", file=out)


if __name__ == "__main__":
    if len(sys.argv) not in [4, 5]:
        print(
            "Usage: python create_woodworking_instance.py "
            "wood_factor size num_machines [random_seed]"
        )
        sys.exit(1)

    wood_factor = float(sys.argv[1])
    size = int(sys.argv[2])
    num_machines = int(sys.argv[3])

    if len(sys.argv) == 5:
        seed = int(sys.argv[4])
    else:
        seed = None

    random.seed(seed)

    instance = Task(size, num_machines, wood_factor)
    instance.dump(out=sys.stdout)
