#!/bin/bash


pddl-generators -h | less

pddl-generators --helpall | less

pddl-generators blocksworld-3ops -- -h | less

pddl-generators --seed 42 blocksworld-4ops -- 5
pddl-generators --seed 42 blocksworld-4ops -- 5
pddl-generators --seed 44 blocksworld-4ops -- 5
pddl-generators --seed 44 blocksworld-4ops -- 5
pddl-generators --seed 42 blocksworld-4ops -- 6
pddl-generators --seed 42 blocksworld-4ops -- 6



pddl-generators visitall -- 5 5 0.5

pddl-generators --output-directory result   visitall -- 5 5 0.5
pddl-generators --output $HOME/problem.pddl visitall -- 5 5 0.5
pddl-generators --output problem.pddl       visitall -- 5 5 0.5



pddl-generators pathways -- -h
pddl-generators pathways -- 12 1 3
pddl-generators --seed 33 pathways -- 12 1 3
