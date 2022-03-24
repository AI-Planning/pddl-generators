## Mystery

* Origin: Drew McDermott. Used in the AIPS-1998 competition.
* Adaptions: Translated all predicate names to get a more intuitive
  notation. Also use typing. NOTE: in the aibasel benchmarks repo, the
  reformulated domain no-mystery does a similar translation of predicate names
  but doesn't use typing. This is not to be confused with the IPC'11 domain
  nomystery for which there is also a generator here.
* Description: Typed STRIPS domain; the name is because the original specified
  the semantics in a disguised manner by using unintuitive names for the
  predicates and constants. The domain is the same like the Mprime domain above,
  except that there is no way of passing on fuel between locations.
* Parameters:
  * -l number of locations
  * -f maximal amount of fuel at a location
  * -s maximal amount of transportation capacity (space)
  * -v number of trucks (vehicles)
  * -c number of cargos
* Generation: Exactly like in Mprime.
