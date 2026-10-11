# universal-pddl-parser
An algorithm for parsing any planning problem in PDDL format.

Currently supports STRIPS planning, temporal planning, multiagent planning.

Compiling with "scons" creates an executable "Domain".

Compiling with "scons tests" runs a series of automated tests on actual planning domains.

The "Domain" and "Instance" classes provide limited support for creating PDDL domains:

- creating types
- creating constants
- adding predicates
- adding functions
- adding actions
- adding preconditions and effects of actions
- adding objects
- adding initial and goal states

However, more complicated constructs (such as forall and when) currently have to be implemented manually. These classes include a method "PDDLPrint" for printing the resulting domains in PDDL format.

The file "examples/compileSHE.cpp" provides an example of how to create a PDDL domain from an existing temporal domain. This is the compilation used in the following paper:

 Sergio Jimenez, Anders Jonsson, Hector Palacios (2015)
 Temporal Planning With Required Concurrency Using Classical Planning
 International Conference on Automated Planning and Scheduling (ICAPS)

Compiling with "scons TPSHE" creates an executable "examples/compileSHE".

The file "examples/serialize.cpp" provides an example of how to create a PDDL domain from an existing multiagent domain. This is the compilation used in the following paper:

 Matthew Crosby, Anders Jonsson and Michael Rovatsos (2014).
 A Single-Agent Approach to Multiagent Planning.
 European Conference on Artificial Intelligence (ECAI)

Compiling with "scons serial" creates an executable "examples/serialize".
