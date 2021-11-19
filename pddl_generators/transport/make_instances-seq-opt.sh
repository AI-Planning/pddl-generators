#! /bin/bash

set -euo pipefail

# <nodes> <size^(1/2)> <degree> <mindistance> <nr-trucks> <nr-packages> <seed>")
SEED=2012

cd "$(dirname "$0")"

./city-generator.py  13 1000  2 100  2  6 $SEED > p07.pddl
./city-generator.py  14 1000  2 100  2  7 $SEED > p09.pddl
./city-generator.py  15 1000  2 100  2  8 $SEED > p13.pddl
./city-generator.py  13 1000  3 100  3  6 $SEED > p08.pddl
./city-generator.py  14 1000  3 100  3  7 $SEED > p10.pddl
./city-generator.py  15 1000  3 100  3  8 $SEED > p14.pddl

./two-cities-generator.py  7 1000  4 100  2  8 $SEED > p11.pddl
./two-cities-generator.py  7 1000  4 100  3  8 $SEED > p12.pddl

./three-cities-generator.py   5 1000 4 100  2  9 $SEED > p15.pddl
./three-cities-generator.py   5 1000 4 100  3  9 $SEED > p16.pddl
