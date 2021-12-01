## Fridge

*   Origin: Tony Barrett. Taken from the [IPP][6] domain collection.
*   Adaptions: Several adaptions made to allow for a flexible number of
    screws on each backplane, and to force a fridge being turned off when
    the screws of the backplane are removed.
*   Description: Typed ADL domain using comlex ADL preconditions (that
    simplify to STRIPS constructs after instantiation). Original was
    STRIPS domain, adaption uses quantification over all screws in
    precondititions, to allow for a flexible number of those. For a number
    of fridges, unfasten the screws holding the backplane, then remove the
    backplanes and exchange the broken compressor with a new one, then
    re-assemble the fridge and turn it on.
*   Parameters:
    *   -f number of fridges
    *   -s number of screws per backplane
    *   -r random seed
*   Generation: No randomization. Simply specify all static relations as
    well as the initial and goal situations.
