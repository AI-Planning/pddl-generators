#! /bin/bash

set -euo pipefail

# <nodes> <size^(1/2)> <degree> <mindistance> <nr-trucks> <nr-packages> <seed>")
SEED=2013

cd "$(dirname "$0")"

./city-generator.py  50 1000  4 100  4  22 $SEED > p11.pddl
./city-generator.py  53 1000  4 100  4  20 $SEED > p12.pddl
./city-generator.py  53 1000  4 100  4  22 $SEED > p13.pddl

./two-cities-generator.py  60 1000  4 100  4  22 $SEED > p14.pddl
./two-cities-generator.py  62 1000  4 100  4  20 $SEED > p15.pddl
./two-cities-generator.py  62 1000  4 100  4  22 $SEED > p16.pddl


./three-cities-generator.py   60 1000 4 100  4  22 $SEED > p17.pddl
./three-cities-generator.py   63 1000 4 100  4  20 $SEED > p18.pddl
./three-cities-generator.py   63 1000 4 100  4  22 $SEED > p19.pddl
./three-cities-generator.py   66 1000 4 100  4  22 $SEED > p20.pddl
