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


OPTIONS   DESCRIPTIONS

-f <num>    number of floors (minimal 2)
-p <num>    number of passengers (minimal 1)



IPC instances: linear scaling where passengers go from 1 to 30. Number of floors is
passengers x 2.  5 random instances


We can follow a similar schema, where we do a linear scaling of the number of passengers
and keep floors at passengers x 2. We can also allow to add some parameter c that controls
the ratio between passengers and floors.

I suggest to use:
   -f ax+b
   -p c(ax+b)

where 1 <= B <= 100 (up to 100 passengers)
1 <= A <= 10 (up to +10 passengers per instance).
0.1 <= C <= 10 (or perhaps this can be set manually to 1.5 or 2)

