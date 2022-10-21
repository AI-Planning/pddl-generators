## Miconic-ADL

*   Origin: Jana Koehler. Used in the AIPS-2000 competition.
*   Adaptions: None.
*   Description: Typed ADL domain using complex preconditions and
    conditional effects. Transport a number of passengers with an elevator
    from their origin- to their destination floors. Obeye several
    restrictions: some passengers must be transported directly, the vips
    shall be served first, some must be transported non-stop, some must be
    attended by others, some groups of people must not meet each other,
    some people do not have access to certain floors. When the lift stops
    at some floor, all passengers waiting there get in, and all passengers
    wanting to go there get out, by a conditional effect.
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
    *   -r random seed
*   Generation: Original generator used in the AIPS-2000 competition. Make
    sure that the specified percentage values are all met by randomly
    assigning types to the passengers (a passenger can have several
    types). Randomly assign origin and destination floors to all
    passengers, considering several heuristics to help problem becoming
    solvable (like not placing conflicting people at the same origin floor
    etc.).
