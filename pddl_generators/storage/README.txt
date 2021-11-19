Problem generator for Storage-Propositional
Authors: Alfonso Gerevini and Alessandro Saetti
===============================================

storage [options] file

Command line options:
 -h      Display this help
 -p      Set the number of problem (header of the problem file)
 -n      Set the number of hoists (default 3)
 -d      Set the number of depots (default 1)
 -o      Set the number of containers (default 1)
 -s      Set the number of store-areas (default 9)
 -c      Set the number of crates (default 5)
 -e      Set seed for random number generator (must be different from 0)


Constraints: 
   Number of number of crates <= store-areas
   Number of number of depots <= store-areas
   Number of number of hoists <= store-areas
   
   3 < Number of crates / Number of containers <= 4


The command lines used to generate the IPC5 problems of Storage-Propositional are:

storage -p 01 -o 1 -e 2005 -c 1 -n 1 -s 1 -d 1 pfile01
storage -p 02 -o 1 -e 2005 -c 1 -n 2 -s 2 -d 1 pfile02
storage -p 03 -o 1 -e 2005 -c 1 -n 3 -s 3 -d 1 pfile03
storage -p 04 -o 1 -e 2005 -c 2 -n 1 -s 4 -d 1 pfile04
storage -p 05 -o 1 -e 2005 -c 2 -n 2 -s 4 -d 1 pfile05
storage -p 06 -o 1 -e 2005 -c 2 -n 3 -s 4 -d 1 pfile06
storage -p 07 -o 1 -e 2005 -c 3 -n 1 -s 6 -d 1 pfile07
storage -p 08 -o 1 -e 2005 -c 3 -n 2 -s 6 -d 1 pfile08
storage -p 09 -o 1 -e 2005 -c 3 -n 3 -s 6 -d 1 pfile09
storage -p 10 -o 1 -e 2005 -c 4 -n 1 -s 8 -d 1 pfile10
storage -p 11 -o 1 -e 2005 -c 4 -n 2 -s 8 -d 1 pfile11
storage -p 12 -o 1 -e 2005 -c 4 -n 3 -s 8 -d 1 pfile12
storage -p 13 -o 2 -e 2005 -c 5 -n 1 -s 10 -d 2 pfile13
storage -p 14 -o 2 -e 2005 -c 5 -n 2 -s 10 -d 2 pfile14
storage -p 15 -o 2 -e 2005 -c 5 -n 3 -s 10 -d 2 pfile15
storage -p 16 -o 2 -e 2005 -c 06 -n 3 -s 12 -d 2 pfile16
storage -p 17 -o 2 -e 2005 -c 07 -n 3 -s 14 -d 2 pfile17
storage -p 18 -o 2 -e 2005 -c 08 -n 3 -s 16 -d 2 pfile18
storage -p 19 -o 3 -e 2005 -c 09 -n 3 -s 18 -d 



containers 1 crates 1 hoists 1 store_areas 1 depots 1
containers 1 crates 1 hoists 2 store_areas 2 depots 1
containers 1 crates 1 hoists 3 store_areas 3 depots 1
containers 1 crates 2 hoists 1 store_areas 4 depots 1
containers 1 crates 2 hoists 2 store_areas 4 depots 1
containers 1 crates 2 hoists 3 store_areas 4 depots 1
containers 1 crates 3 hoists 1 store_areas 6 depots 1
containers 1 crates 3 hoists 2 store_areas 6 depots 1
containers 1 crates 3 hoists 3 store_areas 6 depots 1
containers 1 crates 4 hoists 1 store_areas 8 depots 1
containers 1 crates 4 hoists 2 store_areas 8 depots 1
containers 1 crates 4 hoists 3 store_areas 8 depots 1
containers 2 crates 5 hoists 1 store_areas 10 depots 2
containers 2 crates 5 hoists 2 store_areas 10 depots 2
containers 2 crates 5 hoists 3 store_areas 10 depots 2
containers 2 crates 6 hoists 3 store_areas 12 depots 2
containers 2 crates 7 hoists 3 store_areas 14 depots 2
containers 2 crates 8 hoists 3 store_areas 16 depots 2
containers 3 crates 9 hoists 3 store_areas 18 depots 
