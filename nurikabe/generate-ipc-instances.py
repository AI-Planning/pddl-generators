#!/usr/bin/env python

import os, errno, subprocess, sys


def mkdirs(name):
    try:
        os.makedirs(name)
    except OSError as e:
        if e.errno != errno.EEXIST:
            pass


AGL_SET = [
    (6, 35),
    (6, 64),
    (7, 76),
    (7, 88),
    (8, 93),
    (8, 99),
    (9, 106),
    (9, 108),
    (10, 255),
    (10, 260),
    (11, 420),
    (11, 762),
    (12, 875),
    (12, 1062),
    (13, 6230),
    (13, 6468),
    (14, 21097),
    (14, 24221),
    (15, 28511),
    (15, 38765),
]

OPT_SET = [
    (3, 2),
    (3, 10),
    (4, 19),
    (4, 28),
    (5, 56),
    (5, 61),
    (6, 66),
    (6, 72),
    (7, 77),
    (7, 81),
    (8, 83),
    (8, 87),
    (9, 101),
    (9, 107),
    (10, 252),
    (10, 266),
    (11, 345),
    (11, 499),
    (12, 820),
    (12, 1244),
]


def generate(dirname, seeds):
    mkdirs(dirname)
    for size, seed in seeds:
        base_cmd = [
            sys.executable,
            "generate.py",
            "random",
            "-1",
            str(size),
            str(size),
            str(seed),
        ]
        for ext in ["raw", "pddl", "solution"]:
            output = subprocess.check_output(base_cmd + [ext]).decode()
            with open(
                os.path.join(dirname, "random-%dx%d-%d.%s" % (size, size, seed, ext)),
                "w",
            ) as f:
                f.write(output)


generate("agl-instances", AGL_SET)
generate("opt-instances", OPT_SET)
