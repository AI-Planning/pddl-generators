TYPED=sokoban-generator-typed
UNTYPED=sokoban-generator-untyped

all: $(TYPED) $(UNTYPED)

$(TYPED): $(TYPED).cpp
	$(CXX) $(TYPED).cpp -o $(TYPED)

$(UNTYPED): $(UNTYPED).cpp
	$(CXX) $(UNTYPED).cpp -o $(UNTYPED)

test: $(TYPED)
	./$(TYPED) -n 5 -b 1 -w 0 -s 1 > $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f $(TYPED) $(UNTYPED)
