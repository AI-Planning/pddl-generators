NAME=n-puzzle-generator

$(NAME): $(NAME).cc
	$(CXX) $(NAME).cc -o $(NAME)

test: $(NAME)
	./$(NAME) -n 2 -s 1 > $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f $(NAME)
