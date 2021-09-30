#!/bin/bash

snake_size=1
initial_apples=5

for percentage in {40,55,70,85,100}; do
    ./generate.py empty-6x5 $snake_size $initial_apples $percentage% 112$percentage pddl --output instances-opt
    ./generate.py empty-6x6 $snake_size $initial_apples $percentage% 113$percentage pddl --output instances-opt
    ./generate.py empty-6x7 $snake_size $initial_apples $percentage% 114$percentage pddl --output instances-opt
done

for percentage in {40,55,70,85,99}; do
    ./generate.py empty-5x5 $snake_size $initial_apples $percentage% 111$percentage pddl --output instances-opt
done

for percentage in {40,55,70,85,100}; do
    ./generate.py empty-6x6 $snake_size $initial_apples $percentage% 221$percentage pddl --output instances-sat
    ./generate.py empty-8x8 $snake_size $initial_apples $percentage% 223$percentage pddl --output instances-sat
done

for percentage in {40,55,70,85,99}; do
    ./generate.py empty-7x7 $snake_size $initial_apples $percentage% 222$percentage pddl --output instances-sat
    ./generate.py empty-9x9 $snake_size $initial_apples $percentage% 224$percentage pddl --output instances-sat
done
