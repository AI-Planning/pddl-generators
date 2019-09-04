This repository is an inofficial collection of PDDL generators, some of
which have been used to generate benchmarks for the International
Planning Competitions (IPC).

Instructions
------------
* Build all generators: ``./build_all``
* Build in parallel: ``./build_all -j4``
* Delete intermediate files: ``./build_all clean``
* Build single generator: ``cd assembly; make``
* Test generators: ``sudo apt install python-tox && tox``

Feedback
--------
* Bug reports and pull requests are appreciated

Generator descriptions
----------------------
The following notes were taken from the webpage
http://fai.cs.uni-saarland.de/hoffmann/ff-domains.html, where also most
of the generators are from:


This page was created by Joerg Hoffmann, to form a starting point for people who want to perform large-scale empirical studies in planning. The FF domain collection provides (randomized, where possible) generators for 20 STRIPS and ADL planning benchmark domains, including the examples used in both competitions. Below, we give, for each domain, information about the origin, made adaptions (if any), the parameters of the generator, and the randomization strategy. [...] The generators should be self-explanatory in terms of how to build them (makefiles are included), and how to run them. In the less obvious cases, we have also included a README file.
** Note: ** We do not make any claims whatsoever about the validity of the problem ranges generated, nor about the adequacy of the kind of problems that are generated within a domain. We have made an effort to imitate closely the examples known from published problem suites, and we have generally chosen the most obvious first-guess randomization strategy. Some of the generators, like the one for Assembly, are quite a hack. Nevertheless, we believe that the generators form an invaluable tool for experimentation - they have definetely done so in our own experiments. Please contact us with any comments or suggestions.

** Acknowledgments: ** We thank all persons involved in the development of any of the domains below. We have mentioned all names of persons we knew were involved, and apologize if someone was left out. Please contact us with any additional information on this. Thanks also go to Malte Helmert for discussions on the validity of problem ranges, and to Ulrich Scholz for pointing out bugs.


** Assembly **

*   Origin: Drew McDermott. Used in the AIPS-1998 competition.
*   Adaptions: None.
*   Description: Typed ADL domain using complex pre- and effect-conditions, as well as conditional effects. Assemble one goal item by repeatedly putting together several items that are part of the same meta-item. The part-of relation defines a tree, where the root is the goal item and the leafs are the base items that do not need to be assembled. Any item can need a resource (like an ofen) during the process of its being assembled. It may be that a ``transient'' part has to be assembled somewhere and removed afterwards, that parts of the same meta-item need to be assembled in a specified order (obeying assemble order constraints) and that some other part needs to be assembled before the transient part is removed again (obeying remove order constraints).
*   Parameters:
    *   -d depth of part-of tree
    *   -m maximal number of sons to any node in the tree
    *   -h probability that a node has any sons
    *   -n number of distinct resources
    *   -r probability that a non-base node requires a resource
    *   -t probability that an item is transient part for any higher-in-tree item
    *   -a probability that a pair of items has an assemble order constraint
    *   -o probability that an item has a remove ordering constraint with a transient part
*   Generation: Create a tree of depth -d. Nodes have sons with probability -h. If they do have sons, then a random number between 1 and -m, biased to be lower the deeper the node is in the tree. With probability -r, the node needs a random resource. An item is assigned a transient part relation to any item in a higher tree level with probability -t, parts or transient parts of the same item have an assemble ordering constraint with probability -a, and for a transient part relation (A, B) the parts of B are given a remove ordering constraint to A with probability -o. Cycles in the assemble and remove order constraints are avoided by arbitrarily ordering the respective items, and allowing constraints only between pairs A and B with A


** Blocksworld-3ops **

*   Origin: Goes back to the annals of AI, seems to be first mentioned in Terry Winograd's PhD thesis 1972. In a 1974 planning paper by Gerald Sussman. Taken from the [IPP][6] domain collection.
*   Adaptions: None.
*   Description: Classical untyped STRIPS domain, where stackable blocks need to be re-assembled on a table with unlimited space. Representation uses 3 operators, moving a block from the table to another block, a block from another block to the table, or a block from a block to another block. Semantically, the representation does not use a robot arm, in difference to the 4 operator representation below. The initial state specifies a complete state, the goal state specifies only the on relations required between any two blocks.
*   Parameters: Number of blocks.
*   Generation: Uses random blocksworld state generator provided by John Slaney and Sylvie Thiebaux. Simply translates two such states into PDDL, and prints them out as the initial state and the goal state, where the latter state has all facts removed except the on relations between blocks.


** Blocksworld-4ops **

*   Origin: See above. Used in the AIPS-2000 competition. Taken from the [IPP][6] domain collection.
*   Adaptions: None.
*   Description: Like above, but uses a robot arm that can be used for stacking a block onto a block, unstacking a block from a block, putting down a block, or picking up a block.
*   Parameters: Number of blocks.
*   Generation: Like above.


** Briefcaseworld **

*   Origin: First mentioned by Edwin Pednault. Taken from the [IPP][6] domain collection.
*   Adaptions: None.
*   Description: Typed classical ADL domain, using conditional effects. Transport a number of objects from their start- to their goal-locations, using a briefcase. Each location is accessible from each other location, objects can be put into the briefcase or taken out of the briefcase. When a move is made between locations, then all objects inside the briefcase are also moved, which is encoded by a conditional effect.
*   Parameters: -o number of objects
*   Generation: randomly distribute the start locations of all objects and the briefcase over -o %2B 1 locations. Do the same for the goal locations.


** Ferry **

*   Origin: ?. Taken from the [IPP][6] domain collection.
*   Adaptions: Sail operator modified so that moves can only happen between different locations.
*   Description: Untyped STRIPS domain. Transport a number of cars from their start- to their goal-locations, using a ferry. Each location is accessible from each other location, cars can be debarked or boarded, the ferry can always carry only one car at a time.
*   Parameters:
    *   -l number of locations
    *   -c number of cars
*   Generation: randomly distribute the start and goal locations of all cars over the locations.


** Freecell **

*   Origin: Fahiem Bacchus. Used in the AIPS-2000 competition.
*   Adaptions: Domain encoding modified such that cards, freecells, and columns all have their own natural numbers. This helps avoiding superfluous action instances.
*   Description: Typed STRIPS encoding of a card game (similar to Solitaire) that comes free with Microsoft Windows. Given a random configuration of cards across some columns, move all cards in a specified order onto some goal stacks, obeying a number of stacking rules, and using a number of freecells as a resource.
*   Parameters:
    *   -f number of freecells
    *   -c number of columns
    *   -s number of suits (maximal 4)
    *   -i number of initial stacks
    *   -0 .. -3 number of cards in each suite
*   Generation: As long as there is a card that has not yet been placed somewhere, choose one such card at random, and place it randomly on one initial stack.


** Fridge **

*   Origin: Tony Barrett. Taken from the [IPP][6] domain collection.
*   Adaptions: Several adaptions made to allow for a flexible number of screws on each backplane, and to force a fridge being turned off when the screws of the backplane are removed.
*   Description: Typed ADL domain using comlex ADL preconditions (that simplify to STRIPS constructs after instantiation). Original was STRIPS domain, adaption uses quantification over all screws in precondititions, to allow for a flexible number of those. For a number of fridges, unfasten the screws holding the backplane, then remove the backplanes and exchange the broken compressor with a new one, then re-assemble the fridge and turn it on.
*   Parameters:
    *   -f number of fridges
    *   -s number of screws per backplane
*   Generation: No randomization. Simply specify all static relations as well as the initial and goal situations.


** Grid **

*   Origin: Drew McDermott. Used in the AIPS-1998 competition.
*   Adaptions: None.
*   Description: Untyped STRIPS domain. A robot moves along a rectangular grid where positions can be locked. Locks can be opened with matching keys, and the goal is to have some of these keys at specified locations.
*   Parameters:
    *   -x horizontal extension of grid
    *   -y vertical extension of grid
    *   -t number of different key and lock types
    *   -p probability, for any key, to be mentioned in the goal
    *   -k number of keys vector (one 0 .. 9 entry for each type)
    *   -l number of locks vector (one 0 .. 9 entry for each type)
*   Generation: Randomly distribute the robot, lock and key positions over the grid. No two locks can be at the same location, and the robot must not start on a locked position. If a key is required to be mentioned in the goal, then generate a random goal location for it.


** Gripper **

*   Origin: Jana Koehler. Used in the AIPS-1998 competition.
*   Adaptions: None.
*   Description: Untyped STRIPS domain. Given a robot with two gripper hands, transport a number of balls from a room A to another room B.
*   Parameters: -n number of balls.
*   Generation: No randomization. Place all balls in room A and require them to be in B instead.


** Hanoi **

*   Origin: ?. Taken from the [IPP][6] domain collection.
*   Adaptions: None.
*   Description: Untyped STRIPS encoding of the well-known Towers of Hanoi problem.
*   Parameters: -n number of discs.
*   Generation: No randomization.


** Logistics **

*   Origin: First version by Manuela Veloso, AIPS-1998 version created by Bart Selman and Henry Kautz. Used in both the AIPS-1998 and AIPS-2000 competitions.
*   Adaptions: None.
*   Description: Classical untyped STRIPS domain. Transport packages within cities via trucks, and between cities via airplanes. Locations within a city are directly connected (trucks can move between any two such locations), and so are the cities. In each city there is exactly one truck, each city has one location that serves as an airport.
*   Parameters:
    *   -c number of cities
    *   -s size of each city, i.e. number of locations within cities
    *   -p number of packages
    *   -a number of airplanes
*   Generation: Place trucks randomly within their cities, place airplanes randomly at airports. Distribute start and goal locations of packages randomly over all locations.


** Miconic-ADL **

*   Origin: Jana Koehler. Used in the AIPS-2000 competition.
*   Adaptions: None.
*   Description: Typed ADL domain using complex preconditions and conditional effects. Transport a number of passengers with an elevator from their origin- to their destination floors. Obeye several restrictions: some passengers must be transported directly, the vips shall be served first, some must be transported non-stop, some must be attended by others, some groups of people must not meet each other, some people do not have access to certain floors. When the lift stops at some floor, all passengers waiting there get in, and all passengers wanting to go there get out, by a conditional effect.
*   Parameters:
    *   -f number of floors
    *   -p number of passengers
    *   -u percentage of passengers with direct transportation
    *   -v percentage of vips
    *   -g percentage of non-stop passengers
    *   -n percentage of passengers that must be attended
    *   -a percentage of passengers that can attend the above type
    *   -A percentage of passengers in conflict group A
    *   -B percentage of passengers in group B, which must not meet the above group
    *   -N percentage of people with no access to some floors
    *   -F percentage of of floors not to be accessed by those
*   Generation: Original generator used in the AIPS-2000 competition. Make sure that the specified percentage values are all met by randomly assigning types to the passengers (a passenger can have several types). Randomly assign origin and destination floors to all passengers, considering several heuristics to help problem becoming solvable (like not placing conflicting people at the same origin floor etc.).


** Miconic-SIMPLE **

*   Origin: Jana Koehler. Used in the AIPS-2000 competition.
*   Adaptions: None.
*   Description: Typed ADL domain using conditional effects. Like above, but without any additional constraints. The conditional effects make sure that all waiting passengers get in or out.
*   Parameters:
    *   -f number of floors
    *   -p number of passengers
*   Generation: Original generator used in the AIPS-2000 competition. Simply distribute origin and destination floors at random.


** Miconic-STRIPS **

*   Origin: Jana Koehler. Used in the AIPS-2000 competition.
*   Adaptions: None.
*   Description: Typed STRIPS domain. Like above, but with explicit control over the passengers that get in or out of the lift.
*   Parameters:
    *   -f number of floors
    *   -p number of passengers
*   Generation: Original generator used in the AIPS-2000 competition. Simply distribute origin and destination floors at random.


** Movie **

*   Origin: Corin Anderson. Used in the AIPS-1998 competition.
*   Adaptions: None.
*   Description: Untyped STRIPS domains. Buy one each out of five types of snacks, then rewind the movie and reset the counter.
*   Parameters: -n number of snacks of each type
*   Generation: No randomization.


** Mprime **

*   Origin: Drew McDermott. Used in the AIPS-1998 competition.
*   Adaptions: Translated all predicate names to get a more intuitive notation. Operator for passing on fuel from one location to another could, in the original version, be instantiated with the same location as origin and destination city, which caused the amount of fuel in that city to increase one unit. Changed that such that origin and destination cities must be different.
*   Description: Typed STRIPS domain; the name results from Mystery' (see below). Logistics variant where trucks move on a map of locations. Additionally, trucks have only limited transportation capacity, and there are constraints on the amount of fuel. Each location has initially a certain amount of fuel available. Moving a truck away from a location decreases the amount of fuel at that location by one. If a location has more than one fuel item, then it can pass a fuel item over to a different location.
*   Parameters:
    *   -l number of locations
    *   -f maximal amount of fuel at a location
    *   -s maximal amount of transportation capacity (space)
    *   -v number of trucks (vehicles)
    *   -c number of cargos
*   Generation: Create a simple map of -l locations such that location i is linked to location i%2B1, and location -l is linked to location 1. Randomly assign transportation capacity between 1 and -s to all vehicles, and fuel between 0 and -f to all locations. Distribute cargo origin and destination locations randomly over all locations, likewise for the starting locations of the vehicles.


** Mystery **

*   Origin: Drew McDermott. Used in the AIPS-1998 competition.
*   Adaptions: Translated all predicate names to get a more intuitive notation.
*   Description: Typed STRIPS domain; the name is because the original specified the semantics in a disguised manner by using unintuitive names for the predicates and constants. The domain is the same like the Mprime domain above, except that there is no way of passing on fuel between locations.
*   Parameters:
    *   -l number of locations
    *   -f maximal amount of fuel at a location
    *   -s maximal amount of transportation capacity (space)
    *   -v number of trucks (vehicles)
    *   -c number of cargos
*   Generation: Exactly like in Mprime.


** Schedule **

*   Origin: One variation appears in the Prodigy collection by Manuela Veloso. Prepared for the AIPS-2000 competition by Fahiem Bacchus.
*   Adaptions: None.
*   Description: Typed ADL domain using conditional effects. Encodes a simple Scheduling kind of problem where a number of objects need to be processed using a collection of machines. Possible actions are polishing, punching holes, painting etc. All actions need uniform time, which is modelled by a do-time-step operator. If that operator is applied, then all busy machines are no longer busy, and all scheduled objects are no longer scheduled - this is also an example of the kind of conditional effects that are used in the representation.
*   Parameters: (without significant changes to the domain, parameters -s to -o can not be arbitrarily increased)
    *   -p number of objects (parts)
    *   -s number of shapes (maximal 3: cylindrical, circular, oblong)
    *   -c number of colors (maximal 4)
    *   -w number of different widths of holes (maximal 3)
    *   -o number of orientations of holes (maximal 2)
    *   -Q probability that a part needs to be made cylindrical
    *   -W probability that a part is initially coloured
    *   -E probability that a part needs to be coloured
    *   -R probability that a part has a hole initially
    *   -T probability that a part needs to have a hole
    *   -Y probability that a part needs to have a specific surface condition
*   Generation: All parts are given a random initial shape and surface condition. With the respective probability, they are given random colours or holes. In the goal state, they need, with the respective probabilities, to be cylindrical (which is the only shape that can be produced by the machines), to be randomly coloured, to have a random hole, and to have a random goal surface condition.


** Tsp **

*   Origin: Obtained from Maria Fox and Derek Long.
*   Adaptions: None.
*   Description: Untyped STRIPS domain. Extremely simple version of TSP. The locations are connected by a complete graph, i.e. each location is accessible from each other location. The edges all have equal cost - one moving operation - and the goal is simply to have all locations visited. An optimal solution simply visits all locations once in an arbitrary ordering.
*   Parameters: -n number of locations
*   Generation: No randomization.


** Tyreworld **

*   Origin: Stuart Russel. Adaption for multiple tyres by Jana Koehler. Taken from the [IPP][6] domain collection.
*   Adaptions: None.
*   Description: Typed STRIPS domain. Replace a flat tyre with a spare one. This involves fetching the tools (wrench, jack, pump) from the boot, undoing the nuts on the flat tyre, jacking up the (appropriate) hub(s), removing the tyre, doing up the spare one, etc. Adapted for several tyres by simply increasing the number of flat tyres to be replaced.
*   Parameters: -n number of tyres.
*   Generation: No randomization.
