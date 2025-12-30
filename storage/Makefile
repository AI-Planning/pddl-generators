GENERATOR=storage

all: generator

generator: main.cpp
	$(CXX) main.cpp -o $(GENERATOR)

test: generator
	./$(GENERATOR) -o 1 -c 1 -n 1 -s 1 -d 1 -e 1 $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f $(GENERATOR)
