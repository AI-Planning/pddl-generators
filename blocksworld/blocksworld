#! /bin/bash

set -e
set -u

# Either 3 or 4.
OPS=$1
BLOCKS=$2

BS=./bwstates.1/bwstates
TR=./${OPS}ops/2pddl/2pddl


$BS -s 2 -n ${BLOCKS} > STATES
$TR -d STATES -n ${BLOCKS}

rm STATES
