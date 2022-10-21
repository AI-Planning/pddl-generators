* Origin: Drew McDermott. Used in the AIPS-1998 competition.
* Adaptions: None.
* Description: Typed ADL domain using complex pre- and
  effect-conditions, as well as conditional effects. Assemble one goal
  item by repeatedly putting together several items that are part of the
  same meta-item. The part-of relation defines a tree, where the root is
  the goal item and the leafs are the base items that do not need to be
  assembled. Any item can need a resource (like an ofen) during the
  process of its being assembled. It may be that a ``transient'' part
  has to be assembled somewhere and removed afterwards, that parts of
  the same meta-item need to be assembled in a specified order (obeying
  assemble order constraints) and that some other part needs to be
  assembled before the transient part is removed again (obeying remove
  order constraints).
* Parameters:
  * -d depth of part-of tree
  * -m maximal number of sons to any node in the tree
  * -h probability that a node has any sons
  * -n number of distinct resources
  * -r probability that a non-base node requires a resource
  * -t probability that an item is transient part for any higher-in-tree item
  * -a probability that a pair of items has an assemble order constraint
  * -o probability that an item has a remove ordering constraint with a transient part
  * -s random seed
* Generation: Create a tree of depth -d. Nodes have sons with
  probability -h. If they do have sons, then a random number between 1
  and -m, biased to be lower the deeper the node is in the tree. With
  probability -r, the node needs a random resource. An item is assigned
  a transient part relation to any item in a higher tree level with
  probability -t, parts or transient parts of the same item have an
  assemble ordering constraint with probability -a, and for a transient
  part relation (A, B) the parts of B are given a remove ordering
  constraint to A with probability -o. Cycles in the assemble and remove
  order constraints are avoided by arbitrarily ordering the respective
  items, and allowing constraints only between pairs A and B with A
