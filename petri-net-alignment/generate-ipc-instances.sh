#! /bin/bash

# Generate 20 tasks like the IPC 2018 tasks: the same random run of the net d64
# with 0, 10, ..., 190 noise activities.

set -euo pipefail

cd "$(dirname "$0")"
SEED=${1:-2018}
for i in $(seq 1 20); do
    p=$(printf "p%02d" $i)
    ./generate.py nets/d64.pnml --seed $SEED --noise $(( (i - 1) * 10 )) --domain domain-$p.pddl --problem $p.pddl
done
