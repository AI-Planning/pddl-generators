#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

import sys
import os
import argparse
import random
import re
from collections import defaultdict

try:
    import cplex
except ImportError:
    sys.stderr.write("Cplex python library was not found!\n")
    sys.exit(1)

def rnd_int(n):
    return random.randint(0, n-1)


def rnd_sample(x):
    return random.sample(list(sorted(x)), 1)[0]


def rnd_shuffle(x):
    y = list(x)
    random.shuffle(y)
    return y


class Map(object):
    def __init__(self, locations, edges, seas, prob_coast):
        if locations == 0:
            return
        assert(edges <= (locations * (locations - 1)) / 2)
        assert(seas <= locations)
        self.landmap = [set() for x in xrange(locations)]
        self.location_to_sea = [None for x in xrange(locations)]
        self.sea_to_locations = [[] for i in xrange(seas)]
        self.woodland      = 0#=rnd_int(locations)
        self.mountain      = rnd_int(locations / 2)
        self.metalliferous = rnd_int(locations / 2)
        if seas > 0 and prob_coast > 0:
            next_free_sea = 0
            # woodland not at coast simplies a bit the solver later on
            for x in xrange(1, locations):
                if next_free_sea < seas or rnd_int(prob_coast) == 0:
                    sea = rnd_int(seas) if next_free_sea >= seas else next_free_sea
                    self.location_to_sea[x] = sea
                    self.sea_to_locations[sea].append(x)
                    next_free_sea += 1
        # make map connected
        connected = list(sorted(set([0] + \
            [x  \
                for sea in [self.location_to_sea[0]] if sea != None
                for x in self.sea_to_locations[sea]
                ])))
        disconnected = rnd_shuffle([x for x in xrange(locations) if not x in connected])
        for t in disconnected:
            if t in connected:
                continue
            s = rnd_sample(connected)
            self.landmap[s].add(t)
            self.landmap[t].add(s)
            connected.append(t)
            edges -= 1
            if self.location_to_sea[t] != None:
                for t2 in self.sea_to_locations[self.location_to_sea[t]]:
                    if not t2 in connected:
                        connected.append(t2)
        del(connected)
        del(disconnected)
        # generate remaining edges, leave 2 edges open see next
        available = [x for x in xrange(locations) if len(self.landmap[x]) < locations - 1]
        while edges > 2:
            i = rnd_int(len(available))
            s = available[i]
            t = rnd_sample(set(range(locations)) - self.landmap[s] - set([s]))
            self.landmap[s].add(t)
            self.landmap[t].add(s)
            if len(self.landmap[s]) == locations - 1:
                del(available[i])
            if len(self.landmap[t]) == locations - 1:
                available.remove(t)
            edges -= 1
        # make sure all of woodland, metalliferous, and mountain are connected
        reachable = self.compute_reachability()
        connected = set(reachable[self.woodland])
        disconnected = rnd_shuffle([x for x in [self.metalliferous, self.mountain] if not x in connected])
        for t in disconnected:
            if t in connected:
                continue
            s = rnd_sample(connected)
            self.landmap[s].add(t)
            self.landmap[t].add(s)
            connected = connected | reachable[t]
            if len(self.landmap[s]) == locations - 1:
                available.remove(s)
            if len(self.landmap[t]) == locations - 1:
                available.remove(t)
            edges -= 1
        # distribute all remaining edges
        while edges > 0:
            i = rnd_int(len(available))
            s = available[i]
            t = rnd_sample(set(range(locations)) - self.landmap[s] - set([s]))
            self.landmap[s].add(t)
            self.landmap[t].add(s)
            if len(self.landmap[s]) == locations - 1:
                del(available[i])
            if len(self.landmap[t]) == locations - 1:
                available.remove(t)
            edges -= 1
        assert edges >= 0, "provided edges were not enough to make map connected"


    def compute_reachability(self):
        locations = len(self.landmap)
        map_ = [list(self.landmap[x]) for x in xrange(locations)]
        reachable = [[x == y for y in xrange(locations)] for x in xrange(locations)]
        tjd = [None] * locations
        idx = 0
        for x_ in xrange(locations):
            if tjd[x_] is not None:
                continue
            q = [[x_, 0]]
            stack = []
            while len(q) > 0:
                x = q[-1][0]
                if tjd[x] is None:
                    tjd[x] = [idx, idx, True]
                    idx += 1
                    stack.append(x)
                else:
                    y = map_[x][q[-1][1]]
                    if tjd[y][2]:
                        tjd[x][1] = min(tjd[x][1], tjd[y][0])
                    else:
                        for z in reachable[y]:
                            reachable[x][z] = True
                    q[-1][1] += 1
                while q[-1][1] < len(map_[x]):
                    y = map_[x][q[-1][1]]
                    if tjd[y] is None:
                        q.append([y, 0])
                        break
                    else:
                        if tjd[y][2]:
                            tjd[x][1] = min(tjd[x][1], tjd[y][1])
                        else:
                            for z in reachable[y]:
                                reachable[x][z] = True
                    q[-1][1] += 1
                if q[-1][1] == len(map_[x]):
                    if tjd[x][0] == tjd[x][1]:
                        r = set()
                        i = len(stack) - 1
                        while True:
                            y = stack[i]
                            tjd[y][2] = False
                            for z in xrange(locations):
                                if reachable[y][z]:
                                    r.add(z)
                            if y == x:
                                break
                            i -= 1
                        for j in xrange(i, len(stack)):
                            reachable[stack[j]] = r
                        del(stack[i:])
                    del(q[-1])
        return reachable


class Goal(object):

    def __str__(self):
        raise NotImplementedError

    def location(self):
        raise NotImplementedError

    def requires_iron(self):
        raise NotImplementedError

    def requires_coal(self):
        raise NotImplementedError

    def requires_wood(self):
        raise NotImplementedError

    def stone(self):
        raise NotImplementedError

    def timber(self):
        raise NotImplementedError

    def ore(self):
        raise NotImplementedError

    def processed_resources(self):
        raise NotImplementedError


class RailLink(Goal):

    def __init__(self, x, y):
        self.x = x
        self.y = y

    def location(self):
        return self.x

    def requires_iron(self):
        return True

    def requires_coal(self):
        return True

    def requires_wood(self):
        return False

    def stone(self):
        return 0

    def timber(self):
        return 3

    def ore(self):
        return 1

    def processed_resources(self):
        return 2

    def __str__(self):
        return "(connected-by-rail p%d p%d)" % (self.x, self.y)

class House(Goal):

    def __init__(self, x, n):
        self.x = x
        self.n = n

    def location(self):
        return self.x

    def requires_wood(self):
        return True

    def requires_iron(self):
        return False

    def requires_coal(self):
        return False

    def stone(self):
        return self.n

    def timber(self):
        return self.n

    def ore(self):
        return 0

    def processed_resources(self):
        return 2 * self.n

    def __str__(self):
        return "(housing p%d hl%d)" % (self.x, self.n)


class CoalStack(Goal):

    def __init__(self, x):
        self.x = x

    def location(self):
        return self.x

    def requires_wood(self):
        return False

    def requires_iron(self):
        return False

    def requires_coal(self):
        return False

    def stone(self):
        return 0

    def timber(self):
        return 1

    def ore(self):
        return 0

    def processed_resources(self):
        return 1

    def __str__(self):
        return "(has-coal-stack p%d)" % self.x


class SawMill(Goal):

    def __init__(self, x):
        self.x = x

    def location(self):
        return self.x

    def requires_iron(self):
        return False

    def requires_coal(self):
        return False

    def requires_wood(self):
        return False

    def stone(self):
        return 0

    def timber(self):
        return 2

    def ore(self):
        return 0

    def processed_resources(self):
        return 2

    def __str__(self):
        return "(has-sawmill p%d)" % self.x


class Ironworks(Goal):

    def __init__(self, x):
        self.x = x

    def location(self):
        return self.x

    def requires_iron(self):
        return False

    def requires_coal(self):
        return False

    def requires_wood(self):
        return True

    def stone(self):
        return 2

    def timber(self):
        return 2

    def ore(self):
        return 0

    def processed_resources(self):
        return 4

    def __str__(self):
        return "(has-ironworks p%d)" % self.x


class Docks(Goal):

    def __init__(self, x):
        self.x = x

    def location(self):
        return self.x

    def requires_iron(self):
        return False

    def requires_coal(self):
        return False

    def requires_wood(self):
        return True

    def stone(self):
        return 2

    def timber(self):
        return 2

    def ore(self):
        return 0

    def processed_resources(self):
        return 4

    def __str__(self):
        return "(has-docks p%d)" % self.x


class Wharf(Goal):

    def __init__(self, x):
        self.x = x

    def location(self):
        return self.x

    def requires_iron(self):
        return True or False

    def requires_coal(self):
        return True or False

    def requires_wood(self):
        return False or True

    def stone(self):
        return 2 + 2

    def timber(self):
        return 4 + 2

    def ore(self):
        return 2 + 0

    def processed_resources(self):
        return 4 + 4

    def __str__(self):
        return "(has-wharf p%d)" % self.x


NUM_BUILDINGS = 3#4 #5
def gen_building(x, excl_t):
    t = rnd_sample(set(range(NUM_BUILDINGS)) - excl_t)
    if t == 0:
        return t, CoalStack(x)
    elif t == 1:
        return t, SawMill(x)
    elif t == 2:
        return t, Ironworks(x)
    # elif t == 3: # currently not supported by min resource computation
    #     return t, Docks(x)
    # else: # currently not supported by min resource computation
    #     return t, Wharf(x)


class SettlersInstance(object):
    def __init__(self, params, _map = None, _goals = None):
        self.params = params
        self.comment = "Generator input: seed=%d, locations=%d, edges=%d, seas=%d, probabilities=[%d, %d, %d, %d], goals=%d, optimization=[%d, %d], C=%f, V=%f" % (params.seed, params.locations, params.edges, params.seas, params.prob_coast, params.prob_goal_house, params.prob_goal_building, params.prob_goal_raillink, params.goals, params.coef_labour, params.coef_pollution, params.constrainedness, params.vehicles)

        # set random seed
        random.seed(params.seed)

        # generate map
        self.map = Map(params.locations, params.edges, params.seas, params.prob_coast) if _map is None \
                else _map

        # generate goal
        if _goals is None:
            self.goals = []
            ph = params.prob_goal_house
            pb = params.prob_goal_building
            pr = params.prob_goal_raillink
            psum = ph + pb + pr
            locb = range(params.locations)
            buildings = defaultdict(set)
            for x in xrange(params.locations):
                if self.map.location_to_sea[x] is None:
                    buildings[x] = set([3, 4])
            locr = [x for x in xrange(params.locations) if len(self.map.landmap[x]) > 0]
            rail_links = defaultdict(set)
            houses = defaultdict(lambda: 0)
            if len(locr) == 0:
                p = int((float(pb) / psum) * pr)
                pb += p
                ph += pr - p
                pr = 0
            for ng in xrange(params.goals):
                what = random.randint(1, psum)
                if what <= ph:
                    loc = rnd_int(params.locations)
                    houses[loc] += 1
                elif what <= ph + pb:
                    i = rnd_int(len(locb))
                    x = locb[i]
                    t, building = gen_building(x, buildings[x])# rnd_sample(set(range(Building.TYPES)) - buildings[x])
                    self.goals.append(building)
                    buildings[x].add(t)
                    if len(buildings[x]) == NUM_BUILDINGS:
                        del(locb[i])
                        if len(locb) == 0:
                            p = int((float(pr) / psum) * pb)
                            pr += p
                            ph += pb - p
                            pb = 0
                else:
                    i = rnd_int(len(locr))
                    x = locr[i]
                    y = rnd_sample(self.map.landmap[x] - rail_links[x])
                    self.goals.append(RailLink(x, y))
                    rail_links[x].add(y)
                    if len(rail_links[x]) == len(self.map.landmap[x]):
                        del(locr[i])
                        if len(locr) == 0:
                            p = int((float(pb) / psum) * pr)
                            pb += p
                            ph += pr - p
                            pr = 0
            for loc in houses.keys():
                self.goals.append(House(loc, houses[loc]))
        else:
            self.goals = _goals

        # compute minimal required resource to solve the instance
        # (more precesiely, compute one entry in the ``pareto frontier'',
        # minimizing the amount of resources required in the following order
        #           stones > vehicles > timber > ore

        # compute abstraction of map such that two locations l1 and l2
        # are considered equivalent if they are reachable from each other via
        # ``land''-connections
        reachable = self.map.compute_reachability()
        tmp = defaultdict(lambda: None)
        abstract_locations = 0
        num_seas = len(self.map.sea_to_locations)
        reverse_map = []
        for x in xrange(params.locations):
            if x == min(reachable[x]):
                tmp[x] = abstract_locations
                reverse_map.append(reachable[x])
                abstract_locations += 1
        abstr_start = tmp[min([self.map.woodland, self.map.mountain, self.map.metalliferous])]
        loc_to_seas = [set() for i in xrange(abstract_locations)]
        sea_to_locs = [set() for i in xrange(len(self.map.sea_to_locations))]
        for x in tmp.keys():
            abstr_x = tmp[x]
            for y in reachable[x]:
                sea = self.map.location_to_sea[y]
                if sea is not None:
                    loc_to_seas[abstr_x].add(sea)
                    sea_to_locs[sea].add(abstr_x)

        # check how many resources are required to build only goal buildings
        # processed_resources -> amount of resources that must actually be transported
        self.num_stone = 0
        self.num_timber = 0
        self.num_ore = 0
        self.num_vehicles = 0
        processed_resources = [0 for i in xrange(abstract_locations)]
        iron_required = False
        coal_required = False
        wood_required = False
        resources_required = [False for i in xrange(len(reachable))]
        num_resources_required = [0 for i in xrange(len(reachable))]
        locs_req_res = set()
        has_wharf = defaultdict(set)
        has_coal_stack = False
        has_ironworks = False
        has_sawmill = False
        for g in self.goals:
            abstr_x = tmp[min(reachable[g.location()])]
            locs_req_res.add(abstr_x)
            resources_required[g.location()] = \
                    (g.location() not in [self.map.woodland, self.map.mountain, self.map.metalliferous]) \
                    or (g.location() != self.map.woodland and g.timber() > 0) \
                    or (g.location() != self.map.mountain and g.stone() > 0) \
                    or (g.location() != self.map.metalliferous and g.ore() > 0)
            self.num_stone += g.stone()
            self.num_timber += g.timber()
            self.num_ore += g.ore()
            num_resources_required[g.location()] += g.processed_resources()
            processed_resources[abstr_x] += g.processed_resources()
            iron_required = iron_required or g.requires_iron()
            coal_required = coal_required or g.requires_coal()
            wood_required = wood_required or g.requires_wood()
            if isinstance(g, Wharf):
                has_wharf[abstr_x].add(self.map.location_to_sea[g.location()])
            elif abstr_x == abstr_start and isinstance(g, SawMill):
                has_sawmill = True
            elif abstr_x == abstr_start and isinstance(g, CoalStack):
                has_coal_stack = True
            elif abstr_x == abstr_start and isinstance(g, Ironworks):
                has_ironworks = True

        wharf_required = \
            (len(locs_req_res) > 1 or not abstr_start in locs_req_res)

        iron_required = iron_required or wharf_required
        coal_required = coal_required or iron_required
        wood_required = wood_required or iron_required

        resources_required[self.map.metalliferous] = resources_required[self.map.metalliferous] \
                or (iron_required \
                and (self.map.metalliferous != self.map.woodland or self.map.metalliferous != self.map.mountain))
        if resources_required[self.map.metalliferous]:
            locs_req_res.add(abstr_start)
        if iron_required and not has_ironworks:
            _x = Ironworks(None)
            self.num_stone += _x.stone()
            self.num_timber += _x.timber()
            self.num_ore += _x.ore()
        if coal_required and not has_coal_stack:
            _x = CoalStack(None)
            self.num_stone += _x.stone()
            self.num_timber += _x.timber()
            self.num_ore += _x.ore()
        if wood_required and not has_sawmill:
            _x = SawMill(None)
            self.num_stone += _x.stone()
            self.num_timber += _x.timber()
            self.num_ore += _x.ore()

        abstr_start_requires_cart = wharf_required \
            or (any([resources_required[loc] for loc in reverse_map[abstr_start]]))
        if abstr_start_requires_cart:
            self.num_timber += 1
            self.num_vehicles += 1

        # print self.num_vehicles, self.num_stone, self.num_timber, self.num_ore

        # compute most resource efficient way to transport the resources
        # required for building goal buildings, to the respective locations
        if wharf_required:
            # minimal number of additional wharfs ...
            # start with 0 since goal may already specify some wharf locations
            for num_wharfs in xrange(0, len(sea_to_locs) + 1):
                best_num_timber = None
                best_num_ore = None
                best_num_vehicles = None
                # start at ''start'' island, iterate over adjacent seas
                for root_sea in loc_to_seas[abstr_start]:
                    if num_wharfs == 0 and root_sea not in has_wharf[abstr_start]:
                        continue
                    assert(0 <= root_sea and root_sea < num_seas)

                    # qelement := (next sea to consider, reachable locations)
                    q = [[0, sea_to_locs[root_sea]]]

                    # for each sea, potential positions of wharf
                    sea_is_covered = [False for i in xrange(num_seas)]
                    wharf_location_choices = [[] for i in xrange(num_seas)]
                    wharf_location_selection = [[] for i in xrange(num_seas)]
                    sea_is_covered[root_sea] = True
                    wharf_location_selection[root_sea] = [abstr_start]

                    def transitive_closure(reached, covered, selection, start_sea):
                        queue = [start_sea]
                        i = 0
                        while i < len(queue):
                            for loc in sea_to_locs[queue[i]]:
                                for sea in has_wharf[loc]:
                                    if not covered[sea]:
                                        reached.update(sea_to_locs[sea])
                                        covered[sea] = True
                                        selection = [x for x in xrange(abstract_locations) if sea in has_wharf[x]]
                                        queue.append(sea)
                            i += 1

                    # obsolete if no wharf in goal
                    transitive_closure(q[-1][1], sea_is_covered, wharf_location_selection, root_sea)

                    # outer loop => at which seas should a wharf be placed
                    # inner loop => at which locations of the respective seas
                    # should a wharf be placed
                    # TODO outer loop should be changed to minimal spanning tree
                    # computation...
                    while len(q) > 0:
                        qterm = False
                        # if all locations requesting resources are covered by
                        # selected seas:
                        if locs_req_res <= q[-1][1]:
                            # ... distribute wharfs among the locations ...
                            # where is wharf placed?:
                            wharf_locations = list(wharf_location_selection)

                            # seas where wharf needs to be placed
                            required_seas = [l for l in xrange(num_seas) \
                                    if sea_is_covered[l] and len(wharf_location_choices[l]) > 0]
                            # for each such sea, index into list of possible
                            # locations
                            choices = [list(wharf_location_choices[required_seas[0]])] \
                                if len(required_seas) > 0 else [[]]

                            # which sea is considered?
                            reqsea_i = 0
                            while reqsea_i >= 0:
                                pop_back = False
                                # no more wharfs need to be placed:
                                if reqsea_i == len(required_seas):
                                    # - wharf needs to be built at abstract_start
                                    # => for that cart is required
                                    # - for every other abstract location L,
                                    # cart is required only if
                                    #   there is a concrete location l \in
                                    #   L that requires some resource and either
                                    #   l is not at coast or the respective sea
                                    #   does not have any wharf
                                    # otherwise all resources can be transported
                                    # via ship

                                    # distinguish between locations with and
                                    # without access to cart
                                    abstr_loc_requires_cart = [False for x in xrange(abstract_locations)]
                                    abstr_loc_requires_cart[abstr_start] = True
                                    for x in xrange(abstract_locations):
                                        if x == abstr_start:
                                            continue
                                        # resource distributors always need to
                                        # have cart
                                        if len([sea for sea in xrange(len(wharf_locations)) \
                                                if x in wharf_locations[sea]]) >= 1:
                                            abstr_loc_requires_cart[x] = True
                                        elif x in locs_req_res:
                                            for loc in reverse_map[x]:
                                                if resources_required[loc]:
                                                    if self.map.location_to_sea[loc] is None \
                                                            or not sea_is_covered[self.map.location_to_sea[loc]]:
                                                        abstr_loc_requires_cart[x] = True
                                                        break

                                    # generate flow graph
                                    class FlowGraphNode(object):
                                        def __init__(self, iid, ref, is_abstract, required, distr = []):
                                            self.iid = iid
                                            self.ref = ref
                                            self.is_abstract = is_abstract
                                            self.required = required + 12 * len(distr)
                                            self.edges = set()
                                            self.seas = set()
                                            self.distributor = set(distr)
                                            # print iid, ref, self.required
                                        def __eq__(self, other):
                                            return isinstance(other, FlowGraphNode) \
                                                    and self.__hash__() == other.__hash__()
                                        def __hash__(self):
                                            return self.iid
                                    def get_successors(node):
                                        if node.is_abstract:
                                            for sea in loc_to_seas[node.ref]:
                                                for loc in self.map.sea_to_locations[sea]:
                                                    yield (sea, loc)
                                        else:
                                            sea = self.map.location_to_sea[node.ref]
                                            if sea is None:
                                                return
                                                yield
                                            for loc in self.map.sea_to_locations[sea]:
                                                yield (sea, loc)
                                    flow_graph_nodes = [FlowGraphNode(0, abstr_start, True, 0, [sea2  for sea2 in xrange(len(wharf_locations)) if abstr_start in wharf_locations[sea2]])]
                                    node_lookup = [(flow_graph_nodes[0] if x in reverse_map[abstr_start] else None) for x in xrange(len(reachable))]
                                    i = 0
                                    while i < len(flow_graph_nodes):
                                        node = flow_graph_nodes[i]
                                        for sea, loc in get_successors(node):
                                            if node_lookup[loc] is None:
                                                abstr_loc = tmp[min(reachable[loc])]
                                                if abstr_loc_requires_cart[abstr_loc]:
                                                    distributor = [sea2 for sea2 in xrange(len(wharf_locations)) if abstr_loc in wharf_locations[sea2]]
                                                    flow_graph_nodes.append(FlowGraphNode(len(flow_graph_nodes), abstr_loc, True, processed_resources[abstr_loc] + 1, distributor))
                                                    for loc2 in reachable[loc]:
                                                        node_lookup[loc2] = flow_graph_nodes[-1]
                                                else:
                                                    flow_graph_nodes.append(FlowGraphNode(len(flow_graph_nodes), loc, False, num_resources_required[loc]))
                                                    node_lookup[loc] = flow_graph_nodes[-1]
                                            succ = node_lookup[loc]
                                            if succ != node:
                                                node.edges.add((sea, succ.iid))
                                            node.seas.add(sea)
                                        i += 1

                                    integer = [cplex.Cplex().variables.type.integer]
                                    lp_variables = [(["num_%d_%d_%d" % (i, sea, j)], [1.0], [0.0], [], integer) for i in xrange(len(flow_graph_nodes)) for (sea, j) in flow_graph_nodes[i].edges] \
                                                 + [(["amount_%d_%d_%d" % (i, sea, j)], [0.0], [0.0], [], []) for i in xrange(len(flow_graph_nodes)) for (sea, j) in flow_graph_nodes[i].edges]
                                    lp_constraints = [[], [], []]
                                    asgfasas = 0
                                    for i in xrange(len(flow_graph_nodes)):
                                        node = flow_graph_nodes[i]
                                        # sync amount and num
                                        for sea, j in node.edges:
                                            lp_constraints[0] += [[["num_%d_%d_%d" % (i, sea, j), "amount_%d_%d_%d" % (i, sea, j)], [8.0, -1.0]]]
                                            lp_constraints[1] += ["G"]
                                            lp_constraints[2] += [0.0]
                                        # balance equation for moves
                                        for sea in node.seas:
                                            sea_num_in =     ["num_%d_%d_%d" % (j, sea, i) for j in xrange(len(flow_graph_nodes)) if (sea, i) in flow_graph_nodes[j].edges]
                                            sea_num_out =    ["num_%d_%d_%d" % (i, sea, j) for (seaprime, j) in node.edges if seaprime == sea]
                                            lp_constraints[0] += [[sea_num_in + sea_num_out, [1.0] * len(sea_num_in) + [-1.0] * len(sea_num_out)]]
                                            lp_constraints[1] += ["G"]
                                            lp_constraints[2] += [ -1.0 if sea in node.distributor else 0.0 ]
                                        # balance lp_constraints amount
                                        if i > 0:
                                            num_in =     ["num_%d_%d_%d" % (j, sea, i) for j in xrange(len(flow_graph_nodes)) for sea in xrange(num_seas) if (sea, i) in flow_graph_nodes[j].edges]
                                            num_out =    ["num_%d_%d_%d" % (i, sea, j) for (sea, j) in node.edges]
                                            amount_in =  ["amount_%d_%d_%d" % (j, sea, i) for j in xrange(len(flow_graph_nodes)) for sea in xrange(num_seas) if (sea, i) in flow_graph_nodes[j].edges]
                                            amount_out = ["amount_%d_%d_%d" % (i, sea, j) for (sea, j) in node.edges]
                                            lp_constraints[0] += [[amount_in + amount_out + num_out, [1.0] * (len(amount_in)) + [-1.0] * len(amount_out) + [-2.0] * len(num_out)]]
                                            lp_constraints[1] += ["E"]
                                            lp_constraints[2] += [node.required]
                                        # print "NODE", node.iid, i
                                        # while True:
                                        #     if asgfasas >= len(lp_constraints[0]):
                                        #         break
                                        #     print lp_constraints[0][asgfasas], lp_constraints[1][asgfasas], lp_constraints[2][asgfasas]
                                        #     asgfasas += 1


                                    c = cplex.Cplex()
                                    with open(os.devnull, 'w') as f:
                                        c.set_log_stream(f)
                                        c.set_warning_stream(f)
                                        c.set_error_stream(f)
                                        c.set_results_stream(f)
                                        c.parameters.threads.set(1)
                                        c.objective.set_sense(c.objective.sense.minimize)
                                        for names, obj, lb, ub, types in lp_variables:
                                            c.variables.add(names=names, obj=obj, lb=lb, ub=ub, types=types)
                                        c.linear_constraints.add(lin_expr=lp_constraints[0], senses=lp_constraints[1], rhs=lp_constraints[2])
                                        c.solve()
                                        if c.solution.get_status() != cplex.Cplex.solution.status.MIP_optimal:
                                            if c.solution.get_status() == 3:
                                                print ("CPLEX proved that there is no feasible solution")
                                            else:
                                                print ("CPLEX failed to obtain an optimal solution", c.solution.get_status())
                                                print (self.params)
                                                sys.exit(1)

                                        objv = 2 * int(round(c.solution.get_objective_value()))
                                        # print " -- solution --"
                                        # for i in xrange(len(flow_graph_nodes)):
                                        #     node = flow_graph_nodes[i]
                                        #     for sea, j in node.edges:
                                        #         print "  %d-[%d]->%d  ==> %r [%r]" % (i, sea, j, c.solution.get_values("num_%d_%d_%d" % (i, sea, j)), c.solution.get_values("amount_%d_%d_%d" % (i, sea, j)))
                                        # print objv
                                        vehicles = 0
                                        ore = 0
                                        for loc in xrange(abstract_locations):
                                            if loc != abstr_start and abstr_loc_requires_cart[loc]:
                                                objv += 1
                                                vehicles += 1
                                        for sea in xrange(len(wharf_locations)):
                                            assert(len(wharf_locations[sea]) <= 1)
                                            for loc in wharf_locations[sea]:
                                                objv += 2 + 4 + 8 # Docks + Wharf + Ship
                                                ore  += 2 + 4 # Wharf + Ship
                                                vehicles += 1 # Ship
                                        # print objv, ore, vehicles
                                        if best_num_vehicles is None \
                                            or (best_num_vehicles > vehicles) \
                                            or (best_num_vehicles == vehicles and best_num_timber > objv):
                                            best_num_timber = objv
                                            best_num_vehicles = vehicles
                                            best_num_ore = ore
                                        c.end()
                                    pop_back = True

                                # more wharfs to place
                                else:
                                    if len(choices[reqsea_i]) == 0:
                                        # all seas tested, reset indices and
                                        # indicate backtracking
                                        pop_back = True
                                    else:
                                        # more potential wharf locations to test
                                        sea_id = required_seas[reqsea_i]
                                        loc = choices[reqsea_i][-1]
                                        # print "choices for %d %r -> %d" % (sea_id,  choices[reqsea_i], loc)
                                        wharf_locations[sea_id].append(loc)
                                        # print wharf_locations
                                        reqsea_i += 1
                                        choices.append([] if reqsea_i == len(required_seas) \
                                                else list(wharf_location_choices[required_seas[reqsea_i]]))
                                # backtrack
                                if pop_back:
                                    reqsea_i -= 1
                                    del (choices[-1])
                                    if reqsea_i >= 0:
                                        sea_id = required_seas[reqsea_i]
                                        assert (wharf_locations[sea_id][-1] == choices[reqsea_i][-1])
                                        del (wharf_locations[sea_id][-1])
                                        del (choices[reqsea_i][-1])
                            qterm = True
                        # not all locations are covered by currently selected seas:
                        else:
                            # iterate over all remaining seas
                            while q[-1][0] < num_seas:
                                considered_sea = q[-1][0]
                                # if sea is not selected so far
                                if not sea_is_covered[considered_sea] \
                                        and len(q) < num_wharfs:
                                    # is there any location currently reached,
                                    # which is adjacent to the considered sea?
                                    candidate_locations = sea_to_locs[considered_sea] & q[-1][1]
                                    if len(candidate_locations) > 0 \
                                            and not (sea_to_locs[considered_sea] <= q[-1][1]):
                                        sea_is_covered[considered_sea] = True
                                        wharf_location_choices[considered_sea] = \
                                                candidate_locations
                                        q.append([0, q[-1][1] | sea_to_locs[considered_sea]])
                                        # obsolete if no wharf in goal
                                        transitive_closure(q[-1][1], sea_is_covered, wharf_location_selection, considered_sea)
                                        break
                                q[-1][0] += 1
                            qterm = q[-1][0] == num_seas
                        if qterm:
                            del(q[-1])
                            if len(q) > 0:
                                assert len(wharf_location_choices[q[-1][0]]) > 0
                                sea_is_covered[q[-1][0]] = False
                                wharf_location_choices[q[-1][0]] = []
                                q[-1][0] += 1
                if not best_num_timber is None:
                    self.num_stone += num_wharfs * 4
                    self.num_timber += best_num_timber
                    self.num_ore += best_num_ore
                    self.num_vehicles += best_num_vehicles
                    break


    def get_vehicles(self):
        return int((self.params.vehicles * self.num_vehicles))

    def get_resources(self):
        return int((self.params.constrainedness * max(self.num_stone, self.num_timber, self.num_ore)))

    def get_problem(self, INDEPENDENT_COND_EFFECTS, INDEPENDENT_RESOURCE_LEVELS):
        num = {
           "stone"   : int(self.params.constrainedness * self.num_stone),
           "timber"  : int(self.params.constrainedness * self.num_timber),
           "ore"     : int(self.params.constrainedness * self.num_ore),
           "wood"    : int(self.params.constrainedness * self.num_timber),
           "coal"    : int(self.params.constrainedness * self.num_timber),
           "iron"    : int(self.params.constrainedness * self.num_ore),
           "housing" : int(self.params.constrainedness * min(self.num_timber, self.num_stone)),
        }
        for k in num.keys():
            num[k] = num[k] + self.params.constraint_increment
        resources = ["stone", "timber", "ore", "wood", "coal", "iron"]
        base_resources = {
            "stone": self.map.mountain,
            "timber": self.map.woodland,
            "ore": self.map.metalliferous,
        }

        max_r = max([num[r] for r in num])
        vehicles = int((self.params.vehicles * self.num_vehicles)) + self.params.vehicle_increment

        num_locations = len(self.map.landmap)
        num_seas = len(self.map.sea_to_locations)

        res = [";; %s\n\n" % self.comment]
        res.append("(define (problem {0})\n".format(self.params.name))
        res.append("(:domain civ)\n")

        ## Objects; resource values up to 10 are pre-defined as constants in the domain file
        res.append("(:objects\n")
        res.append(" " * 4 + " ".join(["p%d" % i for i in xrange(num_locations)]) + " - place\n")
        if vehicles > 0:
            res.append(" " * 4 + " ".join(["v%d" % i for i in xrange(vehicles)]) + " - vehicle\n")
        if INDEPENDENT_RESOURCE_LEVELS:
            for k in num:
                if num[k] > 10:
                    res.append(" " * 4 + " ".join(["%sl%d" % (k[0], i) for i in xrange(11, num[k] + 1)]) + " - %s_level\n" % k)
        else:
        	if max_r > 10:
        	    res.append(" " * 4 + " ".join(["l%d" % i for i in xrange(11, max_r + 1)]) + " - level\n")
        res.append(")\n")

        ## Initial state
        res.append("(:init\n")

        # Map
        for i in xrange(num_locations):
            for j in sorted(self.map.landmap[i]):
                res.append(" " * 4 + "(connected-by-land p%d p%d)\n" % (i, j))
        for i in xrange(num_seas):
            for j in self.map.sea_to_locations[i]:
                for k in self.map.sea_to_locations[i]:
                    if j != k:
                        res.append(" " * 4 + "(connected-by-sea p%d p%d)\n" % (j, k))
        for i in xrange(num_locations):
            if self.map.location_to_sea[i] != None:
                res.append(" " * 4 + "(by-coast p%d)\n" % i)
        for i in xrange(num_locations):
            if INDEPENDENT_RESOURCE_LEVELS:
                res.append(" " * 4 + "(housing p%d hl0)\n" % i)
            else:
                res.append(" " * 4 + "(housing p%d l0)\n" % i)

        # Initial resource distribution
        for i in xrange(num_locations):
            for r in resources:
                v = num[r] if (r in base_resources and base_resources[r] == i) else 0
                if INDEPENDENT_RESOURCE_LEVELS:
                    res.append(" " * 4 + "(available-%s p%d %sl%d)\n" % (r, i, r[0], v))
                else:
                    res.append(" " * 4 + "(available %s p%d l%d)\n" % (r, i, v))
                if INDEPENDENT_COND_EFFECTS:
                    for d in [1, 2, 4]:
                        if v >= d:
                            if INDEPENDENT_RESOURCE_LEVELS:
                                res.append(" " * 4 + "(available-atleast-%s p%d %sl%d)\n" % (r, i, r[0], d))
                            else:
                                res.append(" " * 4 + "(available-atleast %s p%d l%d)\n" % (r, i, d))

        # Vehicles
        for i in xrange(vehicles):
            res.append(" " * 4 + "(potential v%d)\n" % i)
            for r in resources:
                if INDEPENDENT_RESOURCE_LEVELS:
                    res.append(" " * 4 + "(available-%s v%d %sl0)\n" % (r, i, r[0]))
                else:
                    res.append(" " * 4 + "(available %s v%d l0)\n" % (r, i))

        # Arithmetics
        if INDEPENDENT_RESOURCE_LEVELS:
            for d in [1, 2]:
                for i in xrange(11 - d):
                    res.append(" " * 4 + "(diff-space spl%d spl%d spl%d)\n" % (i+d, d, i))
            for r in ["housing"]:
                for i in xrange(num[r]):
                    res.append(" " * 4 + "(diff-{0} {1}l{2} {1}l{3} {1}l{4})\n".format (r, r[0], i+1, 1, i))
            for r in resources:
                for d in [1, 2, 4]:
                    for i in xrange(num[r] + 1 - d):
                        res.append(" " * 4 + "(diff-{0} {1}l{2} {1}l{3} {1}l{4})\n".format (r, r[0], i+d, d, i))
        else:
            for d in [1, 2, 4]:
                for v in xrange(max_r + 1 - d):
                    res.append(" " * 4 + "(difference l%d l%d l%d)\n" % (v + d, d, v))

        if INDEPENDENT_COND_EFFECTS:
            for r in resources:
                for (al, r_consumed) in [(d, d2) for d in [1, 2, 4] for d2 in [1, 2, 4]]:
                    for r_old in range(0, 8):
                        if     r_old - r_consumed < al and al <= r_old:
                            if INDEPENDENT_RESOURCE_LEVELS:
                                res.append (" " * 4 + "(del-atleast-{0} {1}l{2} {1}l{3} {1}l{4})\n".format (r, r[0], r_old, r_consumed, al))
                            else:
                                res.append (" " * 4 + "(del-atleast {0} l{2} l{3} l{4})\n".format (r, r[0], r_old, r_consumed, al))
                        if     r_old < al and al <= r_old + r_consumed:
                            if INDEPENDENT_RESOURCE_LEVELS:
                                res.append (" " * 4 + "(add-atleast-{0} {1}l{2} {1}l{3} {1}l{4})\n".format (r, r[0], r_old, r_consumed, al))
                            else:
                                res.append (" " * 4 + "(add-atleast {0} l{2} l{3} l{4})\n".format (r, r[0], r_old, r_consumed, al))
        # end initial state
        res.append(")\n")

        ## Goal
        res.append("(:goal (and\n")
        for g in self.goals:
            res.append(" " * 4 + "%s\n" % g)
        res.append("))\n")

        if not self.params.no_cost:
            res.append("(:metric minimize (total-cost))\n")
        res.append(")")

        return "".join(res)

    def get_domain(self):
        coef_lab = self.params.coef_labour
        coef_pol = self.params.coef_pollution
        lab = re.compile("\(increase \(labour\) (\d+)\)")
        pol = re.compile("\(increase \(pollution\) (\d+)\)")
        with open(self.params.domain_source) as f:
            res = f.read()
            if self.params.no_cost:
                res = re.sub('\(:functions \(total-cost\)\)', '', res)
                res = lab.sub("", res)
                res = pol.sub("", res)
            else:
                res = lab.sub(lambda m: "(increase (total-cost) %d)" % (coef_lab * int(m.group(1))), res)
                res = pol.sub(lambda m: "(increase (total-cost) %d)" % (coef_pol * int(m.group(1))), res)
        return res


    def get_num_resource_levels (self):
        return int((self.params.constrainedness * max(self.num_stone, self.num_timber, self.num_ore)))

    def get_num_vehicles (self):
        return int((self.params.vehicles * self.num_vehicles))


def generate():
    p = argparse.ArgumentParser()
    p.add_argument("seed", type=int)
    p.add_argument("locations", type=int)
    p.add_argument("edges", type=int)
    p.add_argument("seas", type=int)
    p.add_argument("goals", type=int)
    p.add_argument("coef_labour", type=int)
    p.add_argument("coef_pollution", type=int)
    p.add_argument("--name", default="settlers")
    p.add_argument("--prob-coast", type=int, default=2)
    p.add_argument("--prob-goal-house", type=int, default=3)
    p.add_argument("--prob-goal-building", type=int, default=3)
    p.add_argument("--prob-goal-raillink", type=int, default=4)
    p.add_argument("--problem", default="p01.pddl")
    p.add_argument("--domain", default="domain-p01.pddl")
    p.add_argument("--domain-source", default=os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "domains", "domain_independent_conditional_effects_independent_levels.pddl"))
    p.add_argument("--constrainedness", "-c", type=float, default=1, help="resource constrainedness")
    p.add_argument("--constraint-increment", "--ci", type=int, default=0, help="absolute resource constraint increment")
    p.add_argument("--vehicles", "-v", type=float, default=1, help="vehicle constrainedness")
    p.add_argument("--vehicle-increment", "--vi", type=int, default=0, help="absolute vehicle increment")
    p.add_argument("--no-cost", action="store_true", default=False)
    params = p.parse_args()
    inst = SettlersInstance(params)


    with open(params.domain, "w") as f:
        f.write(inst.get_domain())
    with open(params.problem, "w") as f:
        f.write(inst.get_problem("independent_conditional_effects" in params.domain_source, \
                                 "independent_levels" in params.domain_source))
        # f.write(inst.get_problem(True, True))


if __name__ == "__main__":
    generate()

