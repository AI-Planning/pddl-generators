## Briefcaseworld

*   Origin: First mentioned by Edwin Pednault. Taken from the [IPP][6]
    domain collection.
*   Adaptions: None.
*   Description: Typed classical ADL domain, using conditional effects.
    Transport a number of objects from their start- to their
    goal-locations, using a briefcase. Each location is accessible from
    each other location, objects can be put into the briefcase or taken
    out of the briefcase. When a move is made between locations, then all
    objects inside the briefcase are also moved, which is encoded by a
    conditional effect.
*   Parameters:
    * -o number of objects
    * -s random seed
*   Generation: randomly distribute the start locations of all objects and
    the briefcase over -o %2B 1 locations. Do the same for the goal
    locations.
