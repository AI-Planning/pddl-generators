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
*   Generation: Create a simple map of -l locations such that location i is linked to location i%2B1, and location -l is linked to location 1. Randomly assign transportation capacity between 1 and -s to all vehicles, and fuel between 0 and -f to all locations. Distribute cargo origin and destination locations randomly over all locations, likewise for the starting locations of the vehicles.



IPC instances