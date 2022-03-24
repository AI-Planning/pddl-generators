** Miconic-STRIPS **

* Origin: Jana Koehler. Used in the AIPS-2000 competition.
* Adaptions: None.
* Description: Typed STRIPS domain. Like above, but with explicit
  control over the passengers that get in or out of the lift.
* Parameters:
  * -f number of floors
  * -p number of passengers
* Generation: Original generator used in the AIPS-2000 competition.
  Simply distribute origin and destination floors at random.


OPTIONS   DESCRIPTIONS

-f <num>    number of floors (minimal 2)
-p <num>    number of passengers (minimal 1)
-r <num>    random seed (optional)


IPC instances: linear scaling where passengers go from 1 to 30. Number of floors
is passengers * 2.  5 random instances
