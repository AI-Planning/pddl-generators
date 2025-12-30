TXT=generate_problems
PDDL=generate_pddl

all: $(TXT) $(PDDL)

$(TXT): $(TXT).cpp
	$(CXX) $(TXT).cpp -o $(TXT)

$(PDDL): $(PDDL).cpp
	$(CXX) $(PDDL).cpp -o $(PDDL)

clean:
	rm -f $(TXT) $(PDDL)

test: $(TXT) $(PDDL)
	./generator.py 5 3 80
	mv domain.pddl $${PDDL_TEST_DIR}/domain.pddl
	mv problem.pddl $${PDDL_TEST_DIR}/problem.pddl
