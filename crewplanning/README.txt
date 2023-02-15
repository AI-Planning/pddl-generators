This generator has not been used for IPC 2011.
All the problems in this domain came from IPC 2008.

IPC 2011 Organizers





pddl-generators note:

Crewplanning domain is a numeric / temporal domain from ipc-2008 .
This generator requires an ocaml and ocamlbuild, but
the source code seems to be missing Utils module, therefore we cannot build it at the moment.

The only part of Utils module being used is two functions:

* Utils.pf, which I assume is similar to printf
* Utils.str, which I assume is similar to sprintf

Therefore, it is relatively easy to rewrite it with Printf.fprintf and Printf.sprintf
in the modern ocaml standard modules.

The original generator produces several files from the same command line argument.
This is rewritten so that each run produces a single pddl file.

Install requirements: `sudo apt install ocaml ocamlbuild`
