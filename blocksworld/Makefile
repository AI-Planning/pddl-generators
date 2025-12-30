all: 3ops 4ops matching

bwstates:
	$(MAKE) -C bwstates.1

3ops: bwstates
	$(MAKE) -C 3ops/2pddl

4ops: bwstates
	$(MAKE) -C 4ops/2pddl

matching: bwstates
	$(MAKE) -C matching

test: 3ops
	cp 3ops/domain.pddl $${PDDL_TEST_DIR}/domain.pddl
	./blocksworld 3 5 1 > $${PDDL_TEST_DIR}/p01.pddl

clean:
	cd bwstates.1 && make clean
	cd 3ops/2pddl && make clean
	cd 4ops/2pddl && make clean
	cd matching && make clean
