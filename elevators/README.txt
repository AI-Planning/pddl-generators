There was not Readme.txt in the generator version I have, so this is
the way I am using it. I cannot credit the author as I don't know who
was she/he.

- Change the following variables at generate_data.c
  	- FLOORS: number of floors
	- AREA_SIZE: a divisor of FLOORS (a slow elevator covers this
  	  + 1 floors) (a fast elevator skips this/2-1 floors)
	- FAST_ELEVATORS: number of fast elevators
	- SLOW_ELEVATORS: number of slow elevators covering the same area
	- FAST_CAPACITY: persons a fast elevators can hold
	- SLOW_CAPACITY: persons a slow elevators can hold

- Use generate_data.c to generate a first txt version of the
  problems. Parameters are:
  	    -  MinPassengers : the number of passengers of the first problem
	    -  MaxPassengers : the number of passengers of the last problem
	    -  Step : the step to go from MinPassengers to MaxPassengers
	    -  MinID : the start number of number problems of the same size
	    -  MaxID : the end number of number problems of the same size
 
- If you want to change the floors, area size, etc. do again the two
  previous steps.

- Decide which version you want to create by putting to 1 the value
  of the following variables of file generate_pddl.c:
     - int temporal=0;
     - int numeric=0;
     - int net_benefit=0;
  If all are 0, the STRIPS with action-costs version is generated.

- Use generate_pddl.c to create the pddl problems. Parameters are:
  - Min number of floors of the problems (first number of problem name)
  - Max number of floors of the problems (first number of problem name)
  - Floor step: difference of floors from one problem to the next one
  - MinPassengers : the number of passengers of the first problem
   (second number of problem name)
  - MaxPassengers : the number of passengers of the last problem
   (second number of problem name) 
  - Step : the step to go from MinPassengers to MaxPassengers
  - MinID : the start number of the first problem of the same size
   (third number of the problem name)
  - MaxID : the end number of the last problem of the same size
   (third number of the problem name) 

EXAMPLE

If in generate_data.c we have: 
#define FLOORS 25
#define AREA_SIZE 6
#define FAST_ELEVATORS 3
#define SLOW_ELEVATORS 2
#define FAST_CAPACITY 6
#define SLOW_CAPACITY 4

and we execute: 

generate_data 40 50 3 1 1

we will obtain the following files:
p25_40_1.txt
p25_43_1.txt
p25_46_1.txt
p25_49_1.txt


Then, we can call

generate_pddl 25 25 1 40 49 3 1 1

to obtain files

p25_40_1.pddl  p25_43_1.pddl  p25_46_1.pddl  p25_49_1.pddl



