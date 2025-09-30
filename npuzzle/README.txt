N-Puzzle Domain:
----------------

This is the classic NxN sliding puzzle domain. It has received a large
amount of attention from search researchers and in particularly has
been studied in the context of macro learning.

usage:

OPTIONS      DESCRIPTIONS

-n <num>     rows / columns (minimal 1)
-s <integer> random seed
-u           use to generate unsolvable instances

options must be included in this order!!

example usage:
./n-puzzle-generator -n 2 -s 0
