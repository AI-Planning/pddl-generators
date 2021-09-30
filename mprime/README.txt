
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

