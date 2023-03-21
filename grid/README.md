# Grid

Two generators for grid domain which was proposed by Drew McDermott for the AIPS-1998 competition,
the first competition. The first generator is from Drew McDermott.

* Origin: Drew McDermott. Used in the AIPS-1998 competition.
* Adaptions: None.
* Description: Untyped STRIPS domain. A robot moves along a rectangular grid
  where positions can be locked. Locks can be opened with matching keys, and
  the goal is to have some of these keys at specified locations.
* Parameters:
  * -x horizontal extension of grid
  * -y vertical extension of grid
  * -t number of different key and lock types
  * -p probability, for any key, to be mentioned in the goal
  * -k number of keys vector (one 0 .. 9 entry for each type)
  * -l number of locks vector (one 0 .. 9 entry for each type)
  * -r random seed (optional)
* Generation: Randomly distribute the robot, lock and key positions over the
  grid. No two locks can be at the same location, and the robot must not start
  on a locked position. If a key is required to be mentioned in the goal, then
  generate a random goal location for it.

One current issue is that locked spots are distributed entirely randomly. It
would be more interesting if we have some kind of "rooms" scenario where is more
likely that one must traverse via some locked places.

IPC instances:

-x 5 -y 5 -t 4 -k 8 -l 8 -p ? -s 0


## mini-grid

The second generator is inspired by the gym environment called MiniGrid that is
heavily used in RL/DRL.  The generator, that is guaranteed to generate solvable
and interesting instances, is from Blai Bonet (https://github.com/bonetblai/mini-grid).

Random instances are generated used given floorplans which already contain "walls"
that create rooms, and locked cells that serve as doors between rooms. On such
floorplans, shapes and keys are randomly assigned and placed around, together
with initial and goal position for the robot. The goal is always for the robot
to reach a designated cell.

Different floorplans are provided but new floorplans can be used in creative ways,
not necessarily rectangular shapes. The original repository contains example
problems generated from the given floorplans together with solutions computed
with pyperplan.

The options for the generator are:

```
usage: mini_grid.py [-h] [--seed SEED] [--debug_level DEBUG_LEVEL] [--num_instances NUM_INSTANCES] [--results RESULTS] [--floorplans_path FLOORPLANS_PATH] floorplan nshapes

positional arguments:
  floorplan             Filename for floorplan
  nshapes               Number of shapes for locks (0 means choose it randomly)

options:
  -h, --help            show this help message and exit
  --seed SEED           Seed for random generator (default=0)
  --debug_level DEBUG_LEVEL
                        Set debug level (default=0)
  --num_instances NUM_INSTANCES
                        Number of instances to generate (default=1)

paths:
  --results RESULTS     Path to results folders (default='')
  --floorplans_path FLOORPLANS_PATH
                        Path to floorplans (default='')
```


