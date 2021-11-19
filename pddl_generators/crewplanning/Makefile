
all: main.native

main.native:
	ocamlbuild main.native

clean:
	ocamlbuild -clean

test: main.native
	./main.native -day 3 -crew 3 -util 2 -seed 100
