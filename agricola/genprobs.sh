#!/usr/bin/env bash

seed=1
for stages in {3..12}; do
    for workers in {5..10}; do
	for i in {1..2}; do
	    ./GenAgricola.py $stages $seed --num_workers $workers > prob$stages-$workers-$seed.pddl
	    seed=$((seed+1))
	done
    done
done

for stages in {6..12}; do
    for workers in {5..6}; do
	for i in {1..2}; do
	    ./GenAgricola.py $stages $seed --num_workers $workers --must_create_workers > prob-allworkers-$stages-$workers-$seed.pddl
	    seed=$((seed+1))
	done
    done
done
