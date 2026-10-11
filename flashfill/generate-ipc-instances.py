#!/usr/bin/env python3

"""Generate the 36 candidate tasks for IPC 2018 in the directory ipc-instances."""

import os
import subprocess

DIR = os.path.dirname(os.path.abspath(__file__))

# Task name: (numbers of program lines, examples)
TASKS = {
    "add-paren-2a": ((4, 5, 6), [f"add-paren-{x}-{y}" for x, y in [(3, 2), (3, 3)]]),
    "add-paren-2b": ((4, 5, 6), [f"add-paren-{x}-{y}" for x, y in [(4, 1), (5, 1)]]),
    "add-paren-3b": ((4, 5, 6), [f"add-paren-{x}-{y}" for x, y in [(4, 2), (5, 3), (5, 4)]]),
    "extract-minutes-2": ((4, 5, 6), [f"extract-minutes-{x}" for x in [1, 2]]),
    "extract-minutes-3": ((4, 5, 6), [f"extract-minutes-{x}" for x in [3, 4, 5]]),
    "extract-minutes-4": ((4, 5, 6), [f"extract-minutes-{x}" for x in [6, 7, 8, 9]]),
    "extract-minutes-5": ((4, 5, 6), [f"extract-minutes-{x}" for x in [1, 3, 5, 7, 9]]),
    "first-last-initial-2a": ((6, 7, 8), [f"first-last-initial-{x}-{y}" for x, y in [(3, 3), (3, 4)]]),
    "first-last-initial-2b": ((6, 7, 8), [f"first-last-initial-{x}-{y}" for x, y in [(3, 5), (3, 6)]]),
    "initials-2a": ((3, 4, 5), [f"initials-{x}-{y}" for x, y in [(3, 3), (3, 4)]]),
    "initials-2b": ((3, 4, 5), [f"initials-{x}-{y}" for x, y in [(4, 5), (6, 5)]]),
    "initials-3": ((3, 4, 5), [f"initials-{x}-{y}" for x, y in [(3, 4), (3, 5), (4, 4)]]),
}

os.makedirs("ipc-instances", exist_ok=True)
for name, (lines, examples) in TASKS.items():
    for num_lines in lines:
        problem = f"ipc-instances/{name}-{num_lines}.pddl"
        domain = f"ipc-instances/domain-{name}-{num_lines}.pddl"
        examples_paths = [os.path.join(DIR, "examples", f"{example}.pddl") for example in examples]
        cmd = [os.path.join(DIR, "generate.py"), str(num_lines)] + examples_paths + ["--domain", domain, "--problem", problem]
        subprocess.run(cmd, check=True)
