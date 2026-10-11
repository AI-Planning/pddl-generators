#!/usr/bin/env python3

"""Generate a task for aligning an event trace with a Petri net.

The trace is a random run of the Petri net. Optionally, we add noise to it by
inserting activities from other random runs and by swapping adjacent events.

The encoding follows the Java tool that generated the IPC 2018 tasks (see
README): there is one action for each "move in the model", "move in the log"
and "synchronous move", so the domain file depends on the trace.
"""

import argparse
import random
import xml.etree.ElementTree as ET
from collections import defaultdict


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("net", help="Petri net in PNML format (see nets/)")
    parser.add_argument("--seed", type=int, default=0, help="random seed (default: %(default)s)")
    parser.add_argument(
        "--noise",
        type=int,
        default=0,
        help="number of activities from other runs to insert into the trace (default: %(default)s)",
    )
    parser.add_argument(
        "--swap",
        type=int,
        default=30,
        help="swap each pair of adjacent events with probability SWAP/101 (default: %(default)s)",
    )
    parser.add_argument("--domain", required=True, help="output domain file")
    parser.add_argument("--problem", required=True, help="output problem file")
    return parser.parse_args()


def format_name(name):
    """Remove or replace characters that are not allowed in PDDL names."""
    for char in " /()<>.":
        name = name.replace(char, "")
    for char in ",+-":
        name = name.replace(char, "_")
    return name.lower()


class Transition:
    def __init__(self, name, invisible):
        self.name = name
        self.invisible = invisible
        self.inputs = []
        self.outputs = []


class PetriNet:
    def __init__(self, filename):
        root = ET.parse(filename).getroot()

        def elements(tag):
            return [e for e in root.iter() if e.tag.split("}")[-1] == tag]

        def label(element):
            text = element.find("name/text")
            return text.text if text is not None and text.text else ""

        place_names = {p.get("id"): "t" + format_name(label(p) or p.get("id")) for p in elements("place")}
        self.places = list(place_names.values())
        transitions = {}
        num_invisible = 0
        for t in elements("transition"):
            name = label(t).strip().strip('"')
            invisible = not name
            if invisible:
                name = f"generatedinv{num_invisible}"
                num_invisible += 1
            transitions[t.get("id")] = Transition(format_name(name), invisible)
        self.transitions = list(transitions.values())
        names = [t.name for t in self.transitions]
        if len(set(names)) != len(names):
            raise ValueError("transition labels must be unique")

        has_input = set()
        has_output = set()
        for arc in elements("arc"):
            source, target = arc.get("source"), arc.get("target")
            if source in transitions:
                transitions[source].outputs.append(place_names[target])
                has_input.add(place_names[target])
            else:
                transitions[target].inputs.append(place_names[source])
                has_output.add(place_names[source])
        self.initial_marking = [p for p in self.places if p not in has_input]
        self.final_marking = [p for p in self.places if p not in has_output]

    def random_run(self, rng):
        """Fire random enabled transitions until no transition is enabled and
        return the names of the visible transitions that fired."""
        marking = set(self.initial_marking)
        trace = []
        while True:
            enabled = [t for t in self.transitions if marking.issuperset(t.inputs)]
            if not enabled:
                return trace
            transition = rng.choice(enabled)
            marking.difference_update(transition.inputs)
            marking.update(transition.outputs)
            if not transition.invisible:
                trace.append(transition.name)


def add_noise(trace, noise, net, rng):
    """Insert noise activities from other random runs at random positions."""
    activities = []
    while len(activities) < noise:
        activities += net.random_run(rng)
    activities = rng.sample(activities, noise)
    new_trace = []
    i = j = 0
    while i < len(trace) and j < len(activities):
        if rng.choice([True, False]):
            new_trace.append(trace[i])
            i += 1
        else:
            new_trace.append(activities[j])
            j += 1
    return new_trace + trace[i:] + activities[j:]


def swap_noise(trace, noise, rng):
    for i in range(len(trace) - 1):
        if rng.randint(0, 100) < noise:
            trace[i], trace[i + 1] = trace[i + 1], trace[i]
    return trace


def action(name, preconditions, effects):
    def conjunction(atoms):
        if len(atoms) == 1:
            return f"    {atoms[0]}\n"
        return "    (and\n" + "".join(f"        {atom}\n" for atom in atoms) + "    )\n"

    return (
        f"(:action {name}\n"
        "    :parameters ()\n"
        "    :precondition\n" + conjunction(preconditions) + "    :effect\n" + conjunction(effects) + ")\n\n"
    )


def write_domain(net, trace, filename):
    events = [f"ev{i}" for i in range(1, len(trace) + 1)] + ["evend"]
    parts = [
        "(define (domain petri-net-alignment)\n",
        "(:requirements :action-costs :typing :equality)\n",
        "(:types\n    place event\n)\n",
        "(:constants\n",
        "".join(f"    {p} - place\n" for p in net.places),
        "".join(f"    {e} - event\n" for e in events),
        ")\n",
        "(:predicates\n    (token ?p - place)\n    (tracepointer ?e - event)\n    (allowed)\n)\n",
        "(:functions\n    (total-cost) - number\n)\n",
    ]
    positions = defaultdict(list)
    for i, activity in enumerate(trace):
        positions[activity].append(i)
    for t in net.transitions:
        consume = [f"(not (token {p}))" for p in t.inputs]
        produce = [f"(token {p})" for p in t.outputs]
        for i in positions[t.name]:
            event, next_event = events[i], events[i + 1]
            parts.append(
                action(
                    f"movesync-{t.name}-{event}",
                    [f"(token {p})" for p in t.inputs] + [f"(tracepointer {event})"],
                    ["(allowed)"] + consume + produce + [f"(not (tracepointer {event}))", f"(tracepointer {next_event})"],
                )
            )
        cost = 0 if t.invisible else 1
        parts.append(
            action(
                f"moveinthemodel-{t.name}",
                [f"(token {p})" for p in t.inputs],
                ["(not (allowed))"] + consume + produce + [f"(increase (total-cost) {cost})"],
            )
        )
    for i, activity in enumerate(trace):
        event, next_event = events[i], events[i + 1]
        parts.append(
            action(
                f"moveinthelog-{activity}-{event}-{next_event}",
                [f"(tracepointer {event})", "(allowed)"],
                [f"(not (tracepointer {event}))", f"(tracepointer {next_event})", "(increase (total-cost) 1)"],
            )
        )
    parts.append(")\n")
    with open(filename, "w") as f:
        f.write("".join(parts))


def write_problem(net, filename):
    init = ["(tracepointer ev1)", "(allowed)"]
    init += [f"(token {p})" for p in net.initial_marking]
    init += ["(= (total-cost) 0)"]
    goal = [f"(token {p})" if p in net.final_marking else f"(not (token {p}))" for p in net.places]
    goal += ["(tracepointer evend)"]
    with open(filename, "w") as f:
        f.write("(define (problem align)\n(:domain petri-net-alignment)\n")
        f.write("(:init\n" + "".join(f"    {atom}\n" for atom in init) + ")\n")
        f.write("(:goal\n(and\n" + "".join(f"    {atom}\n" for atom in goal) + ")\n)\n")
        f.write("(:metric minimize (total-cost))\n)\n")


def main():
    args = parse_args()
    rng = random.Random(args.seed)
    net = PetriNet(args.net)
    trace = net.random_run(rng)
    if args.noise:
        trace = add_noise(trace, args.noise, net, rng)
    trace = swap_noise(trace, args.swap, rng)
    write_domain(net, trace, args.domain)
    write_problem(net, args.problem)


if __name__ == "__main__":
    main()
