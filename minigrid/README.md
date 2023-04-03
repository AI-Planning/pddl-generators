# Mini-Grid

This is an alternative generator for the Grid domain, which was proposed by Drew
McDermott for the AIPS-1998 competition. You can find the original generator
under "grid".

The "minigrid" generator is inspired by the Gym environment called MiniGrid that is
heavily used in RL/DRL. The generator, that is guaranteed to generate solvable
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


