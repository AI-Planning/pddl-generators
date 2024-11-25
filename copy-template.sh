#!/bin/bash

domain=$1

cp pddl_generators/__template/* -t pddl_generators/$domain/
echo pddl_generators/$domain/__init__.py
