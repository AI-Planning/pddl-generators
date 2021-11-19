all: 3ops 4ops matching-untyped matching-typed

bwstates:
	$(MAKE) -C bwstates.1

3ops: bwstates
	$(MAKE) -C 3ops/2pddl

4ops: bwstates
	$(MAKE) -C 4ops/2pddl

matching-untyped: bwstates
	$(MAKE) -C matching-untyped

matching-typed: bwstates
	$(MAKE) -C matching-typed

clean:
	$(MAKE) -C bwstates.1 clean
	$(MAKE) -C 3ops clean
	$(MAKE) -C 4ops clean
	$(MAKE) -C matching-untyped clean
	$(MAKE) -C matching-typed clean
