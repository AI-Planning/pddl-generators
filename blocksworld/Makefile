all: 3ops 4ops

bwstates:
	$(MAKE) -C bwstates.1

3ops: bwstates
	$(MAKE) -C 3ops/2pddl

4ops: bwstates
	$(MAKE) -C 4ops/2pddl

clean:
	cd bwstates.1 && make clean
	cd 3ops/2pddl && make clean
	cd 4ops/2pddl && make clean
