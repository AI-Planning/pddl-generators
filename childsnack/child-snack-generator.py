#!/usr/bin/env python
# Generator for the child-snack domain
#
# Authors: Raquel Fuentetaja and Tomas de la Rosa
#
# See domain file for description.
# The ratio parameter is the proportion of needed symbols
# that are declared in advance in the problem file. The min ratio
# should be 1.0 to guarantee solvability
#
# The generator is also able to generate problems in alternative
# representations:
#   (1) Pool: The standard pizza domain
#   (2) Control: A modified version with precondition control for
#   creating one slice at a time.
#   (3) Numeric: An equivalent version in numeric PDDL, where
#   no slice symbols have to be declared in advance.
#

from __future__ import print_function

from math import ceil
import random
import sys


class Task(object):
    def __init__(
        self,
        seed,
        n_children,
        n_trays,
        gluten_factor,
        domain_type="pool",
        const_ratio=1.2,
    ):
        self.type = domain_type
        self.seed = seed
        self.n_children = n_children
        self.n_trays = n_trays
        self.gluten_factor = gluten_factor
        self.const_ratio = const_ratio

        self.n_sandwiches = int(ceil(self.n_children * self.const_ratio))

        self.tables = enum_objects("table", 3)

        self.trays = enum_objects("tray", self.n_trays)
        self.children = enum_objects("child", self.n_children)
        self.sandwiches = enum_objects("sandw", self.n_sandwiches)
        self.breads = enum_objects("bread", self.n_children)
        self.contents = enum_objects("content", self.n_children)

        self.n_allergie = int(self.n_children * self.gluten_factor)

    def dump(self, out=None):
        self._dump_header(out)
        self._dump_objects("  ", out)
        self._dump_init("  ", out)
        self._dump_goal("  ", out)
        # self._dump_metric("  ", out)
        print(")", file=out)

    def _dump_header(self, out=None):
        print(
            "; child-snack task with %s children and %s gluten factor "
            % (self.n_children, self.gluten_factor),
            file=out,
        )
        print("; constant factor of %s" % self.const_ratio, file=out)
        print("; random seed: %d" % self.seed, file=out)
        print(file=out)
        print("(define (problem prob-snack)", file=out)
        print("  (:domain child-snack)", file=out)

    def _dump_objects(self, indent="", out=None):
        print(indent + "(:objects", file=out)
        print("%s  %s - child" % (indent, " ".join(self.children)), file=out)
        print("%s  %s - bread-portion" % (indent, " ".join(self.breads)), file=out)
        print("%s  %s - content-portion" % (indent, " ".join(self.contents)), file=out)
        print("%s  %s - tray" % (indent, " ".join(self.trays)), file=out)
        print("%s  %s - place" % (indent, " ".join(self.tables)), file=out)

        if self.type != "numeric":
            print("%s  %s - sandwich" % (indent, " ".join(self.sandwiches)), file=out)

        print(indent + ")", file=out)

    def _dump_init(self, indent="", out=None):
        print(indent + "(:init", file=out)

        for t in self.trays:
            print(indent + "%s (at %s kitchen)" % (indent, t), file=out)
        for b in self.breads:
            print(indent + "%s (at_kitchen_bread %s)" % (indent, b), file=out)
        for c in self.contents:
            print(indent + "%s (at_kitchen_content %s)" % (indent, c), file=out)

        for b in random.sample(self.breads, self.n_allergie):
            print(indent + "%s (no_gluten_bread %s)" % (indent, b), file=out)

        for c in random.sample(self.contents, self.n_allergie):
            print(indent + "%s (no_gluten_content %s)" % (indent, c), file=out)

        allergie_children = set(random.sample(self.children, self.n_allergie))
        no_allergie_children = set(self.children).difference(allergie_children)

        for c in list(allergie_children):
            print(indent + "%s (allergic_gluten %s)" % (indent, c), file=out)
        for c in list(no_allergie_children):
            print(indent + "%s (not_allergic_gluten %s)" % (indent, c), file=out)

        for c in self.children:
            print(
                indent + "%s (waiting %s %s)" % (indent, c, random.choice(self.tables)),
                file=out,
            )

        if self.type == "pool":
            for s in self.sandwiches:
                print(indent + "%s (notexist %s)" % (indent, s), file=out)

        elif self.type == "control":
            print("%s   (current %s)" % (indent, self.sandwiches[0]), file=out)
            for i in range(1, len(self.sandwiches)):
                print("%s   (next sandw%s sandw%s)" % (indent, i, i + 1), file=out)
        elif self.type == "numeric":
            print("%s   (= (nsandwiches-at_kitchen-no_gluten)  0)" % indent, file=out)
            print("%s   (= (nsandwiches-at_kitchen)  0)" % indent, file=out)
            for t in self.trays:
                print(
                    "%s   (= (nsandwiches-ontray-no_gluten %s)  0)" % (indent, t),
                    file=out,
                )
                print("%s   (= (nsandwiches-ontray %s)  0)" % (indent, t), file=out)

        print(indent + ")", file=out)

    def _dump_goal(self, indent="", out=None):
        print(indent + "(:goal", file=out)
        print(indent + "  (and", file=out)
        for g in self.children:
            print(indent + "%s (served %s)" % (indent, g), file=out)
        print(indent + "  )", file=out)
        print(indent + ")", file=out)

    def _dump_metric(self, indent="", out=None):
        print(indent + "(:metric minimize (total-cost))", file=out)


def enum_objects(pddltype, n):
    return ["%s%s" % (pddltype, i) for i in range(1, n + 1)]


def usage():
    print("\n Incorrect input arguments ")
    print(
        "\n Usage: "
        + sys.argv[0]
        + " [pool | control | numeric]"
        + " <seed> <num_children> <num_trays> <gluten_factor> <const_ratio>"
    )
    print("\n    <const_ratio>: min 1.0")
    sys.exit(1)


if __name__ == "__main__":

    if len(sys.argv) < 6:
        usage()

    try:
        domain_type = sys.argv[1]
        seed = int(sys.argv[2])
        n_children = int(sys.argv[3])
        n_trays = int(sys.argv[4])
        gluten_factor = float(sys.argv[5])
        const_ratio = float(sys.argv[6])

        random.seed(seed)
        pddltask = Task(
            seed, n_children, n_trays, gluten_factor, domain_type, const_ratio
        )
        pddltask.dump()

    except BaseException:
        usage()
