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
	./generate_problems.py 1 1 1 1 1 60
	rm p1_1.txt
	./$(TXT) 5 34 1 1 1 40  # Deprecated, see README.
	./$(PDDL)
	rm domain*_1.pddl p*_1.pddl p*_1.txt
