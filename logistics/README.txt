usage:

OPTIONS   DESCRIPTIONS

-a <num>    number of airplanes
-c <num>    number of cities (minimal 1)
-s <num>    city size(minimal 1)
-p <num>    number of packages (minimal 1)
-t <num>    number of trucks (optional, default and minimal: same as number of cities; there will be at least one truck per city)
-r <num>    random seed (minimal 1, optional)


IPC instances:

IPC00: from 4 to 15 packages, smaller instances have 2 cities, larger ones have 5 cities.
There are instances with 1 or 2 airplanes.

IPC98: cities are scaled linealy, with one more city per instance. There is an outlier
with 47 cities, all others have around 30 something. There are instances with up to 15
airplanes. Packages scale up to 57.  Sometimes there is more than one truck per city.



