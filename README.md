This repository is a collection of PDDL generators, some of which have been used
to generate benchmarks for the International Planning Competition (IPC).

# Instructions
* Install dependencies: ``sudo apt install g++ make cmake python3``
* Build all generators: ``./build_all``
* Build in parallel: ``./build_all -j4``
* Delete (some) intermediate files: ``./build_all clean``
* Build single generator: ``cd assembly; make``
* Test generators: ``sudo apt install tox && tox``

# Feedback
* Bug reports and pull requests are appreciated.

# Citation

If you use the generators for a publication, please use the following citation:

    @Misc{seipp-et-al-zenodo2022,
      author =       "Jendrik Seipp and {\'A}lvaro Torralba and J{\"o}rg Hoffmann",
      title =        "{PDDL} Generators",
      publisher =    "Zenodo",
      year =         "2022",
      howpublished = "\url{https://doi.org/10.5281/zenodo.6382173}"
    }


# Acknowledgments

The basis for this generator collection is the [FF domain
collection](http://fai.cs.uni-saarland.de/hoffmann/ff-domains.html) by
Jörg Hoffmann. The following notes from that page were written with the
original set of generators in mind, but still apply to the current
collection:

> The FF domain collection provides (randomized, where possible) generators
> for 20 STRIPS and ADL planning benchmark domains, including the examples
> used in both competitions. Below, we give, for each domain, information
> about the origin, made adaptions (if any), the parameters of the
> generator, and the randomization strategy. [...] The generators should be
> self-explanatory in terms of how to build them (makefiles are included),
> and how to run them. In the less obvious cases, we have also included a
> README file.
>
> **Note:** We do not make any claims whatsoever about the validity of the
> problem ranges generated, nor about the adequacy of the kind of problems
> that are generated within a domain. We have made an effort to imitate
> closely the examples known from published problem suites, and we have
> generally chosen the most obvious first-guess randomization strategy. Some
> of the generators, like the one for Assembly, are quite a hack.
> Nevertheless, we believe that the generators form an invaluable tool for
> experimentation --- they have definitely done so in our own experiments.
> Please contact us with any comments or suggestions.
>
> We thank all persons involved in the development of the domains [in this
> repository]. We try to mention the persons involved in creating the
> generators, but some authors are missing. Please contact us with any
> additional information on this.

## Overview of mystery/mprime/nomystery domain variants

### Repo: AI-Planning/pddl-generators (pddl-generators)

* pddl-generators/mprime: typed, speaking names, pass fuel between locations, no fuel increment
* pddl-generators/mystery: typed, speaking names, like pddl-generators/mprime but no fuel passing
* pddl-generators/nomystery: typed, speaking names, fuel associated with trucks, very different from aibasel/no-mystery

### Repo: aibasel/downward-benchmarks (aibasel)

* aibasel/mprime: untyped, obfuscated names, pass fuel between locations, forbid increasing fuel
* aibasel/mystery: untyped, obfuscated names, no fuel passing
* aibasel/unofficial/no-mprime: untyped, speaking names, copy of aibasel/mprime but with speaking predicate names, fuel increasing allowed
* aibasel/unoffical/no-mystery: untyped, speaking names, copy of aibasel/mystery but with speaking predicate names, no fuel passing
* aibasel/nomystery-{opt,sat}11-strips: same as pddl-generators/nomystery

* aibasel/mprime, aibasel/mystery, and aibasel/nomystery-{opt,sat}11-strips are the original IPC domains, with the fix applied to mprime, whereas the versions in pddl-generators are adaptations which behave the same.
