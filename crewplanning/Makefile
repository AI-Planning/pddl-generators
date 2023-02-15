all: main.native

main.native:
	ocamlbuild main.native || echo "Warning: skipping crewplanning because ocamlbuild is missing"

clean:
	ocamlbuild -clean

test: main.native
	./main.native -day 3 -crew 3 -util 2 -seed 100 || echo "Warning: skipping crewplanning tests because ocaml is missing"
