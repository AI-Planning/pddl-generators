Usage: dlog [-u|-s|-t|-n|-h] <seed> <#road junctions> <#drivers> <#packages> <#trucks> [distance]
	If distance is a positive value then the distances between
	locations is set randomly using this value as the bound.

	-n: Numeric
	-u: Untyped
	-t: Timed
	-s: Simple time
	-h: Hard numeric


 IPC instances

DLOG-2-2-2  to DLOG-8-6-25 (drivers-trucks-packages)
DLOG-2-2-2
DLOG-2-2-3
DLOG-2-2-4
DLOG-3-2-4
DLOG-3-2-5
DLOG-3-3-5
DLOG-3-3-6
DLOG-3-3-7
DLOG-2-3-6
DLOG-2-3-6
DLOG-2-3-6
DLOG-2-3-6
DLOG-2-3-6
DLOG-3-3-6
DLOG-4-4-8
DLOG-5-5-10
DLOG-5-5-15
DLOG-5-5-20
DLOG-5-5-25
DLOG-8-6-25

first instance has 5 locations 
last instance has 59 locations


Some rules that we could enforce are:

drivers, trucks <= packages, locations


drivers = trucks +- 2

So, I suggest to have three linear scalings: one for trucks/drivers, one for packages, and
a final one for locations.





