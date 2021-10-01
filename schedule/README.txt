## Schedule

*   Origin: One variation appears in the Prodigy collection by Manuela
    Veloso. Prepared for the AIPS-2000 competition by Fahiem Bacchus.
*   Adaptions: None.
*   Description: Typed ADL domain using conditional effects. Encodes a
    simple Scheduling kind of problem where a number of objects need to be
    processed using a collection of machines. Possible actions are
    polishing, punching holes, painting etc. All actions need uniform
    time, which is modelled by a do-time-step operator. If that operator
    is applied, then all busy machines are no longer busy, and all
    scheduled objects are no longer scheduled - this is also an example of
    the kind of conditional effects that are used in the representation.
*   Parameters: (without significant changes to the domain, parameters -s
    to -o can not be arbitrarily increased)
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
*   Generation: All parts are given a random initial shape and surface
    condition. With the respective probability, they are given random
    colours or holes. In the goal state, they need, with the respective
    probabilities, to be cylindrical (which is the only shape that can be
    produced by the machines), to be randomly coloured, to have a random
    hole, and to have a random goal surface condition.
