GENERATOR=nomystery
BUILD_DIR=build

.PHONY: all build test clean distclean

all: build

build:
	@cmake -S . -B $(BUILD_DIR)
	@cmake --build $(BUILD_DIR)
	@ln -sf $(BUILD_DIR)/$(GENERATOR) $(GENERATOR)

test: build
	./$(GENERATOR) -l 3 -p 1 -s 1 > $${PDDL_TEST_DIR}/p01.pddl

clean:
	rm -rf $(BUILD_DIR)

distclean: clean
	rm -f $(GENERATOR)
