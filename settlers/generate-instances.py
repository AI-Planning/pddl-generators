#!/usr/bin/env python

from generator import *

class Struct:
    def __init__(self, **entries):
        self.__dict__.update(entries)
    def __repr__(self):
        return str(self.__dict__)


def generate(seed, locations, edges, seas, goals, constrainedness, constraint_increment, vehicles, vehicle_increment):
    problem =  "p-{locations}-{seas}-{goals}-{constrainedness}-{vehicles}-{seed}.pddl".format(**locals())
    domain = "domain.pddl".format(**locals())

    copy_locals = dict(**locals())


    params = {"no_cost" : False,
              "coef_labour" : 5,
              "coef_pollution" : 10,
              "name" : "settlers",
              "prob_coast" : 2,
              "prob_goal_house": 3,
              "prob_goal_building": 3,
              "prob_goal_raillink": 4,
              "domain_source" :  "domain_independent_conditional_effects_independent_levels.pddl"
    }

    params.update(**copy_locals)
    params  = Struct(**params)

    return SettlersInstance(params)


def write_instance(folder, inst, map_name, const, vconst, name):
    problem =  "instances-{}/p{}-{}-{:02d}-{}.pddl".format(folder, folder, map_name, len(inst.goals), name)

    with open("instances-{}/domain.pddl".format(folder), "w") as f:
        f.write(inst.get_domain())
    with open(problem, "w") as f:
        f.write(inst.get_problem(True, True))




NUM_INSTANCES_GEN = 10000
NUM_INSTANCES_SEL = 1000

maps = {"tiny" : (3, 2, 0),
        "small" : (5, 5, 1),
        "large" : (8, 9, 1),
        "huge" : (16, 18, 2)
}

hardness_instance = {
    "veryeasy" : int(5*NUM_INSTANCES_SEL/100),
    "easy" : int(25*NUM_INSTANCES_SEL/100),
    "medium" : int(50*NUM_INSTANCES_SEL/100),
    "hard" : int(75*NUM_INSTANCES_SEL/100),
    "veryhard" : int(75*NUM_INSTANCES_SEL/100)
}

resource_constrainedness  = {"sat" : (1.5, 5, 1.5, 3),
                             "opt" : (1.2, 4, 1.2, 2)
}


seed = 0
for (folder, map_name, selection, goal_range) in [
        ("opt", "tiny", ["easy"], [1]),
        ("opt", "tiny", ["veryeasy"], [6]),
        ("opt", "tiny", ["veryeasy", "easy"], range(2, 6)),
        ("opt", "small", ["easy"], [1]),
        ("opt", "small", ["veryeasy"], [6]),
        ("opt", "small", ["veryeasy", "easy"], range(2, 6)),
        ("sat", "small", ["medium"], range(4, 11)),
        ("sat", "large", ["medium"], range(4, 11)),
        ("sat", "huge", ["medium"], range(3, 9))]:

    const, inc_res, vconst, inc_vehicles = resource_constrainedness[folder]
    for goals in goal_range:
        loc, edges, seas = maps[map_name]
        instances = []
        while len(instances) < 100:
            instances = [generate(seed + i, loc, edges, seas, goals, const, inc_res, vconst, inc_vehicles) for i in range(NUM_INSTANCES_GEN)]
            seed += NUM_INSTANCES_GEN
            instances = filter (lambda x : len(x.goals) == goals, instances)

        print(folder, map_name, goals, const, len(instances))
        instances = instances[:NUM_INSTANCES_SEL]
        instances = sorted(instances, key=lambda x : (x.get_resources(), x.get_vehicles()))

        assert(len(instances) == NUM_INSTANCES_SEL)
        for hardness in selection:
            write_instance (folder, instances[hardness_instance[hardness]], map_name, const, vconst, hardness)

