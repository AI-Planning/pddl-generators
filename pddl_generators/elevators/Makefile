all: generate_pddl generate_data

generate_data: generate_data.c
	gcc generate_data.c -lm -o generate_data

generate_pddl: generate_pddl.c
	gcc generate_pddl.c -lm -o generate_pddl

test:
	./generate.py 2 3 4 5 6

clean:
	rm -f generate_data generate_pddl
