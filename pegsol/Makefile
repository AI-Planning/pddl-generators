test:
	./pegsol-instances.py  # Only analyzes instances, doesn't generate them.
	echo "(define (problem pegsol-test)" > $${PDDL_TEST_DIR}/problem.pddl && \
	echo "  (:domain pegsolitaire-sequential)" >> $${PDDL_TEST_DIR}/problem.pddl && \
	echo "  (:objects pos1 pos2 - location)" >> $${PDDL_TEST_DIR}/problem.pddl && \
	echo "  (:init (occupied pos1) (free pos2))" >> $${PDDL_TEST_DIR}/problem.pddl && \
	echo "  (:goal (occupied pos2)))" >> $${PDDL_TEST_DIR}/problem.pddl
