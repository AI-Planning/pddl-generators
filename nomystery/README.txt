Nomystery is a transportation domain designed to study resource constrained planning
[1,2]. In this domain, a truck moves in a weighted graph; a set of packages must be
transported between nodes; actions move along edges, and load/unload packages; each move
consumes the edge weight in fuel. In brief, Nomystery is a straightforward problem similar
to the ones contained in many IPC benchmarks. Its key feature is that it comes with a
domain-specific optimal solver allowing to control the constrainedness of the
resources. The generator first creates a random connected undirected graph with n nodes,
and it adds k packages with random origins and destinations. The edge weights are
uniformly drawn between 1 and an integer W. The optimal solver computes the minimum
required amount of fuel M, and the initial fuel supply is set to [C×M], where C >= 1 is a
(float) input parameter of the generator. The parameter C denotes the ratio between the
available fuel vs. the minimum amount required. The problem becomes more constrained when
C approaches 1.

A ubiquitous feature of planning problems is the need to economize limited resources such
as fuel or money. While heuristic search, mostly based on relaxation heuristics, is
currently the superior method for most varieties of planning, its ability to solve
critically resource-constrained problems is limited: relaxation heuristics basically
ignore the resource consumption by actions. Nomystery generator is a nice test domain to
study the behavior of planning algorithms in planning problems with resources.

There are two ways to scale problem difficulty in Nomystery: increasing the number of
packages and locations, and decreasing C. The generator is very fast up to 18 locations
and 18 packages. Two types of encoding can be used for the problems: Hard and
Hard-cost. The only difference is in the action costs. While all actions have a unit cost
in Hard encoding, in Hard-cost version action costs are equal to the amount of fuel
consumed by the action. Therefore, the total cost of a plan for Hard-cost encoding equals
the amount of fuel consumed in the plan. In Hard encoding, problems with 12 locations and
12 packages become quite challenging for state of the art planners when C is close 1
[1]. Hard-cost encoding makes the problems easier for the current planners. The reason is
that the heuristic functions that consider costs are not any more completely ignorant to
the resource consumption of actions. However, this type of encoding is not always feasible
for resource planning; there might be several resources and the cost of the plan might be
different from the amount of the resource consumption. However, our initial experiments
show problems in this encoding with 12 locations and 15 packages become challenging for
current planners when C is close to 1.

From the two versions of the domain we have chosen the non-cost one (cost of the plan and
resource consumption are not related). As the domain creator says, this is the most
difficult one of the two formulations. In addition, it is the less similar to the
Transport domain. According to the domain creator planners find it difficult to solve
problems with more than 12 locations and 12 packages when the constraint on the resource
tends to 1. For the satisficing track, problems 1-10 start with 6 locations and 6 packages
with c=1.5 and each problem increases 1 package and 1 location to the previous one (so
problem 10 has 15 packages and 15 locations). Problems 11-20 also start in 6-6 and
increase parameters by 1, but in this case c=1.1.

For the optimal track, we developed a first version where problems 1-10 start with 2
locations and 1 package with c=1.5 and each problem increases 1 location and 1 package to
the previous one (so problem 10 has 10 packages and 11 locations). Problems 11-20 also
start in 2-1 and increase parameters by 1, but in this case c=1.1. This version resulted
to be quite simple, so we generated a tougher one. We started with 4 locations and 3
packages (last problem had 13 locations and 12 packages) with c=1.5 and c=1.1. That
version was also quite simple, so a third one has been generated. From experiments it
seems the lower the constraint the easier the problems are for the optimal planners: the
search space is pruned quickly as the cost limits the number of applicable actions. Taking
that into account we have generated problems with c=1.1, 1.5 and 2.0. Problems 1-6 have
c=1.1 and start in 9 locations and 8 packages till 14-13. From 7-13 c=1.5, starting in 8
locations and 7 packages. Problems 14-20 are like 7-13 but with c=2.0.

