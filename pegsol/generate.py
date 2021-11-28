#!/bin/env python3

"""
A randomized Pegsol generator for sequential STRIPS with negative preconditions.
"""


import argparse
import random
import numpy as np
import numpy.random as nr
import functools
import textwrap

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--rows", type=int, default=7, help="Number of rows, must be odd.")
parser.add_argument("--cols", type=int, default=7, help="Number of columns, must be odd.")
parser.add_argument("--rowcuts", type=int, default=2, help="Number of rows to cut from each corner. Should be less than floor(rows/2)")
parser.add_argument("--colcuts", type=int, default=2, help="Number of cols to cut from each corner. Should be less than floor(cols/2)")
parser.add_argument("occupancy", type=float, help="Probability that the pegs are initially occupied. (0, 1]")
# parser.add_argument("--european", action="store_true", help="If present, corners are cut in a triangular form (a European pegsol board)")
parser.add_argument("--seed", type=int, default=1)
args = parser.parse_args()


random.seed(args.seed)

# for development
# rows = 7
# cols = 7
# rowcuts = 2
# colcuts = 2
# occupancy = 0.8


rows = args.rows
cols = args.cols
rowcuts = args.rowcuts
colcuts = args.colcuts
occupancy = args.occupancy

assert rows % 2 == 1, f"rows={rows} must be odd"
assert cols % 2 == 1, f"cols={cols} must be odd"
assert rowcuts < rows // 2, f"rowcuts={rowcuts} must be less than floor(rows={rows}/2)"
assert rowcuts < rows // 2, f"rowcuts={rowcuts} must be less than floor(rows={rows}/2)"
assert colcuts < cols // 2, f"colcuts={colcuts} must be less than floor(cols={cols}/2)"
assert occupancy <= 1.0, f"occupancy={occupancy} must be in 0 < occupancy <= 1"
assert 0.0 < occupancy, f"occupancy={occupancy} must be in 0 < occupancy <= 1"

total = rows * cols - 4 * rowcuts * colcuts

################################################################
# board generation

INVALID = -1
FREE = 0
PEG = 1

# initialize free:0 or peg:1
board = (nr.random((rows, cols)) < occupancy).astype(int)

# cut the corners
board[:rowcuts,:colcuts] = -1
board[-rowcuts:,:colcuts] = -1
board[:rowcuts,-colcuts:] = -1
board[-rowcuts:,-colcuts:] = -1


################################################################
# pddl generation

def pos(i,j):
    return f"pos-{i}-{j}"


def objects():
    return [ pos(i,j) for i,j in zip(*np.where(board != INVALID)) ] + ["-", "location"]


def init():
    return [
        ["=", ["total-cost"], "0"],
        ["move-ended"],
        *list(in_line()),
        *[ ["free", pos(i,j)] for i,j in zip(*np.where(board == FREE)) ],
        *[ ["occupied", pos(i,j)] for i,j in zip(*np.where(board == PEG)) ],
    ]


def in_line():
    for i,j in zip(*np.where(board != INVALID)):
        # horizontal move
        if j+2 < cols and board[i, j] != INVALID and board[i, j+1] != INVALID and board[i, j+2] != INVALID:
            yield ["IN-LINE", pos(i,j), pos(i,j+1), pos(i,j+2), ]
            yield ["IN-LINE", pos(i,j+2), pos(i,j+1), pos(i,j), ]
        # vertical move
        if i+2 < rows and board[i, j] != INVALID and board[i+1, j] != INVALID and board[i+2, j] != INVALID:
            yield ["IN-LINE", pos(i,j), pos(i+1,j), pos(i+2,j), ]
            yield ["IN-LINE", pos(i+2,j), pos(i+1,j), pos(i,j), ]


def goal():
    return [
        "and",
        *[ ["free", pos(i,j)] for i,j in zip(*np.where(board != INVALID)) if i != (rows//2) and j != (cols//2) ],
        ["occupied", pos((rows//2), (cols//2))]
    ]


def problem():
    return [
        "define", ["problem", "pegsolprob"],
        [":domain", "pegsolitaire-sequential"],
        [":objects", *objects()],
        [":init", *init()],
        [":goal", goal()],
        [":metric", "minimize", ["total-cost"]],
    ]


def print_sexp(obj,indent=0):
    if isinstance(obj, str):
        return obj
    assert isinstance(obj, list), f"{obj} is not str or list"
    if len(obj) > 4:
        return textwrap.indent(
            "(" + "\n".join(
                map(lambda x: print_sexp(x,indent+1),
                    obj)) + ")",
            " "*indent)
    else:
        return textwrap.indent(
            "(" + " ".join(
                map(lambda x: print_sexp(x,indent+1),
                    obj)) + ")",
            " "*indent)

print_sexp(problem())




