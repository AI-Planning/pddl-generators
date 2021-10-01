#! /bin/bash

# Usage: ./trucks-strips.sh <domain.pddl> <problem.pddl> -seed <seed> -t 1 -l <locations> -p <packages> -a <areas> -n 1

set -euo pipefail

dir=$(dirname "$0")

${dir}/trucks "${@:3}" > problem-adl.pddl
${dir}/adl2strips/ff -o ${dir}/domain-adl.pddl -f problem-adl.pddl
rm problem-adl.pddl

mv domain-strips.pddl "$1"
mv problem-strips.pddl "$2"
