#! /bin/bash

set -euo pipefail

VARIANT=$1
BLOCKS=$2
SEED=$3
OUTPUT=$4

# Get full directory name of the script (https://stackoverflow.com/a/246128).
SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

BS=${SCRIPT_DIR}/bwstates.1/bwstates
TR=${SCRIPT_DIR}/${VARIANT}/2pddl/2pddl

# note: -s 2 is correct, this is for the initial state and the goal state
$TR -d <($BS -s 2 -n ${BLOCKS} -r ${SEED}) -n ${BLOCKS} > $OUTPUT
