NAME=gold-miner-generator

$(NAME): $(NAME).cpp
	g++ $(NAME).cpp -o $(NAME)

test: $(NAME)
	./$(NAME) -r 3 -c 3 -s 1 > $${PDDL_TEST_DIR}/problem.pddl

clean:
	rm -f $(NAME)
