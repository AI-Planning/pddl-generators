How to use the problem generator:

The problem generator creates X instances for the seq-sat and seq-opt
tracks. It allows to change the available percentages of wood and the
parts.
To change them go to the end of the file and look for the tag tasks:

tasks = [generate_instance(size, wood_factor)
         for wood_factor in [1.4, 1.2, 1.0]
         for size in range (3, 33, 3)]

The former generates 30 instances, 10 for each wood factor, starting
in 3, ending in 33 and increasing by 3 from one to each other.

By default problems are created in sub-folders named "seq-sat" and
"seq-opt" which have to exist before the script is run.

The default behavior is to generate problems with only one tool of
each type, if more tools want to be created, the _default_machines
constant has to be changed.

In its current form, the script generates the last 4 problems of IPC 2011:
p17-->p17 IPC
p18-->p19 IPC
p19-->p18 IPC
p20-->p20 IPC