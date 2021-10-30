#!/usr/bin/env python

import argparse

import itertools
import random


def objectlist(tagtype, n):
    return [tagtype + str(i + 1) for i in range(n)]


def objectSeq(tagtype, n):
    oblist = objectlist(tagtype, n)
    return " ".join(oblist)


def unarypred(symbol, arg):
    return "(" + symbol + " " + arg + ")"


def unaryNumvar(symbol, arg):
    return "(= (" + symbol + " " + arg + ") 0)"


def naryNumvar(symbol, args):
    return "(= (" + symbol + " " + " ".join(args) + ") 0)"


def narypred(symbol, args):
    return "(" + symbol + " " + " ".join(args) + ")"


def factSequence(predicate, tagtype, n, indent="\n     "):
    objlist = objectlist(tagtype, n)
    facts = [unarypred(predicate, ob) for ob in objlist]
    return indent.join(facts)


def numVarSequence(predicate, tagtype, n, indent="\n    "):
    objlist = objectlist(tagtype, n)
    facts = [unaryNumvar(predicate, ob) for ob in objlist]
    return indent.join(facts)


def numVarMatrix(predicate, objlist1, objlist2, indent="\n     "):
    numvars = [
        naryNumvar(predicate, list(obtuple))
        for obtuple in itertools.product(objlist1, objlist2)
    ]
    return indent.join(numvars)


def factChain(pred, tagtype, n, rev=False, indent="\n    ", start0=False):
    objlist = objectlist(tagtype, n)
    si = 1
    if start0:
        objlist = [tagtype + "0"] + objlist
        si = 0

    if rev:
        objlist.reverse()

    facts = list()
    for i in range(len(objlist) - 1):
        facts.append(narypred(pred, objlist[i : i + 2]))
    return indent.join(facts)


# Reading the command line arguments


def get_objects(lstage):
    strobj = "\n"
    strobj += objectSeq("num", num_ints) + " - num \n"
    strobj += objectSeq("stage", last_stage + 1) + " - stage\n"
    strobj += objectSeq("round", num_rounds) + " - round \n"
    strobj += objectSeq("worker", num_workers) + " - worker \n"
    strobj += objectSeq("room", num_workers) + " - room \n"

    return strobj


def get_init():
    indent = "\n    "
    strinit = indent + factChain("NEXT_NUM", "num", num_ints, start0=True)

    seqfacts = []
    for i in range(num_ints - 1):
        seqfacts.append(narypred("NEXT2_NUM", ["num" + str(i), "num" + str(i + 2)]))

    for i in range(1, num_ints + 1):
        for j in range(1, i + 1):
            strinit += (
                indent
                + "(NUM_SUBSTRACT num"
                + str(i)
                + " num"
                + str(j)
                + " num"
                + str(i - j)
                + ")"
            )

    strinit += indent + indent.join(seqfacts)

    strinit += indent + factChain("NEXT_STAGE", "stage", last_stage + 1)
    strinit += indent + factChain("NEXT_ROUND", "round", num_rounds)

    strinit += indent + factChain("NEXT_WORKER", "worker", num_workers, True)
    strinit += indent + "(NEXT_WORKER worker1 noworker)\n"

    for j in range(1, num_rounds + 1):
        if j in normalrounds:
            tround = " tnormal"
        else:
            tround = " tharvest"
        strinit += indent + "(category_round round" + str(j) + tround + ")"

    opencards = [
        "act_labor",
        "act_wood",
        "act_clay",
        "act_reed",
        "act_build",
        "act_plow",
        "act_grain",
        "act_stone",
    ]
    facts = [unarypred("open_action", ob) for ob in opencards]
    strinit += indent + indent.join(facts)

    roundcards = [
        "act_fences",
        "act_sheep",
        "act_sow",
        "act_family",
        "act_improve",
        "act_carrot",
        "act_boar",
        "act_cattle",
    ]
    availcards = opencards + roundcards

    s1cards = roundcards[:4]
    s2cards = roundcards[4:]

    dfacts = []
    random.shuffle(s1cards)
    strinit += indent + "(open_action " + s1cards[0] + ")"
    for k in range(min(4, num_rounds)):
        dfacts.append("(DRAWCARD_ROUND " + s1cards[k] + " round" + str(k + 1) + ")")
    strinit += indent + indent.join(dfacts)

    dfacts = []
    random.shuffle(s2cards)
    for k in range(min(4, num_rounds - 4)):
        dfacts.append("(DRAWCARD_ROUND " + s2cards[k] + " round" + str(k + 5) + ")")
    strinit += indent + indent.join(dfacts)

    for k in range(9, num_rounds + 1):
        strinit += indent + "(DRAWCARD_ROUND void round" + str(k) + ")"

    facts = [unarypred("available_action", ob) for ob in availcards]
    strinit += indent + indent.join(facts)

    for k in range(2, num_workers + 1):
        strinit += indent + "(FOOD_REQUIRED worker" + str(k) + " num" + str(2 * k) + ")"
        strinit += (
            indent + "(FOOD_REQUIRED worker" + str(k) + " num" + str((2 * k) + 1) + ")"
        )

    strinit += indent + "(current_worker  worker2)"
    strinit += indent + "(max_worker  worker2)"
    strinit += indent + "(current_round round1)"
    strinit += indent + "(current_stage stage1)"
    strinit += indent + "(harvest_phase stage1 harvest_init)"

    strinit += indent + "(num_food num" + str(random.randint(0, 3)) + ")"

    for el in ["wood", "clay", "reed", "stone"]:
        strinit += indent + "(SUPPLY_RESOURCE act_" + el + " " + el + ")"
    # for el in ['sheep','boar','cattle']:
    #     strinit += indent + "(SUPPLY_ANIMAL act_"+el+" "+el+")"

    strinit += indent + "(built_rooms room1 worker1)"
    strinit += indent + "(built_rooms room2 worker2)"
    for k in range(3, num_workers + 1):
        strinit += indent + "(space_rooms room" + str(k) + ")"

    cost_list = [4, 6, 15, 30, 60]
    while len(cost_list) < num_workers - 1:
        cost_list.append(cost_list[-1] + 30)

    [250, 210, 180, 150, 120, 90, 60, 30, 15, 6, 4]
    for w, c in zip(range(2, num_workers + 1), cost_list[::-1]):
        strinit += (
            indent + "(= (group_worker_cost worker" + str(w) + ") " + str(c) + ")"
        )

    return strinit


def get_goals(last_stage, must_create_workers):
    strgoals = ""
    indent = "\n    "
    strgoals += indent + "(harvest_phase stage" + str(last_stage) + " harvest_end)"

    if must_create_workers:
        strgoals += indent + "(max_worker worker%d)" % num_workers

    return strgoals


name = "prob_agricola"

normalrounds = [1, 2, 3, 5, 6, 8, 10, 12]
parser = argparse.ArgumentParser()

parser.add_argument("last_stage", type=int)
parser.add_argument("seed", type=int)
parser.add_argument("--num_workers", type=int, default=5)
parser.add_argument("--num_ints", type=int, default=16)
parser.add_argument("--num_rounds", type=int, default=20)
parser.add_argument("--must_create_workers", action="store_true")

args = parser.parse_args()


num_rounds = args.num_rounds
num_workers = args.num_workers
num_ints = max(args.num_ints, 2 + args.num_workers * 2)
last_stage = args.last_stage
random.seed(args.seed)

hrounds = [r for r in range(1, 21) if r not in normalrounds]
num_rounds = hrounds[last_stage - 1] + 1

print("(define (problem " + name + ")")
print(" (:domain agricola)")
print(" (:objects " + get_objects(last_stage) + ")")
print(" (:init " + get_init() + ")")
print(" (:goal (and " + get_goals(last_stage, args.must_create_workers) + "))")
print(" (:metric minimize (total-cost))")
print(")")

# except:
#   print "Usage: " +sys.argv[0] + " <name> "+sys.argv[1]+" <last_stage>"
