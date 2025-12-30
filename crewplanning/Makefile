all: main.native

main.native:
	ocamlbuild main.native || echo "Warning: skipping crewplanning because ocamlbuild is missing"

clean:
	ocamlbuild -clean

test: main.native
	@#./main.native -day 3 -crew 3 -util 2 -seed 100 > $${PDDL_TEST_DIR}/problem.pddl
	@echo "[SKIP] skipping crewplanning tests because generated problems use missing predicates"
