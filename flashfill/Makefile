CXXFLAGS = -O2 -std=c++11 -Isrc -Iuniversal-pddl-parser
OBJECTS = src/compile.o $(patsubst %.cpp,%.o,$(wildcard universal-pddl-parser/*.cpp))
HEADERS = $(wildcard src/*.h universal-pddl-parser/*.h)

compile: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test: compile
	./generate-example.py add-paren 3 --seed 1 > $${PDDL_TEST_DIR}/example1.pddl
	./generate-example.py add-paren 4 --seed 2 > $${PDDL_TEST_DIR}/example2.pddl
	./generate.py 4 $${PDDL_TEST_DIR}/example1.pddl $${PDDL_TEST_DIR}/example2.pddl --domain $${PDDL_TEST_DIR}/domain.pddl --problem $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f compile $(OBJECTS)
