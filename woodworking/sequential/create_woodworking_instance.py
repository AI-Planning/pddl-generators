#!/usr/bin/env python

from math import ceil
import random

_colours = ["black", "white", "red", "green", "blue", "mauve"]
_woods = ["oak", "pine", "cherry", "teak", "mahogany", "beech", "walnut"]
_default_machines = {"highspeed-saw":3, "glazer":3, "grinder":3, 
                    "immersion-varnisher":3, "planer":3, 
                    "saw":3, "spray-varnisher":3}
_boardsize_prefix = "s"

class Part(object):
  def __init__(self, index, woods, colours, problemtype=None):
    self.name = "p%d" % index
    self.problemtype = problemtype
    self.generate_random_goal(woods, colours)
    self.generate_random_init(woods, colours)
  def generate_random_goal(self, woods, colours):
    self.goalprops = dict()
    self.goalprops["treatment"] = \
        random.choice(["varnished", "glazed"])
    self.goalprops["surface-condition"] = \
          random.choice(["smooth", "verysmooth"])
    if self.problemtype != "painting":
      colours = colours + ["natural"]
    self.goalprops["colour"] = random.choice(colours)
    self.goalprops["wood"] = random.choice(woods)
    self.generate_goal_selection()
  def generate_goal_selection(self, nr_goals=None):
    nr_goals = random.choice([2,2,2,3,4])
    self.goalselection = set(random.sample(self.goalprops, nr_goals))
  def generate_random_init(self, woods, colours):
    def gen_preprocessing_status():
        self.initprops["treatment"] = \
              random.choice(["varnished", "glazed", "colourfragments"])
        self.initprops["surface-condition"] = \
              random.choice(["rough", "smooth", "verysmooth"])
        self.initprops["wood"] = self.goalprops["wood"]
        poss_colours = (set(colours + ["natural"]) - 
                        set([self.goalprops["colour"]]))
        self.initprops["colour"] = random.choice(list(poss_colours))
      
    self.initprops = dict()
    status = random.choice(["unused"]*4 + ["available"])
    if status!="unused":
      while True:
        gen_preprocessing_status()
        goal_initially_satisfied = True
        for prop in self.goalselection:
          if self.initprops[prop] != self.goalprops[prop]:
            goal_initially_satisfied = False
            break
        if not goal_initially_satisfied:
          break
        self.generate_goal_selection(len(self.goalselection))
    self.size = random.randint(1,3)
  def dump_init(self, indent="", out=None):
    if self.initprops:
      print >> out, "%s(available %s)" % (indent, self.name)
      for prop, val in self.initprops.iteritems():
        print >> out, "%s(%s %s %s)" % (indent, prop, self.name, val)
    else:
      print >> out, "%s(unused %s)" % (indent, self.name)
    print >> out, "%s(goalsize %s %s)" % (indent, self.name, 
                            ["small", "medium", "large"][self.size -1])
    print >> out, "%s(= (spray-varnish-cost %s) %s)" % (indent, self.name, 
                            self.size * 5 )
    print >> out, "%s(= (glaze-cost %s) %d)" % (indent, self.name, self.size *5 + 5)
    print >> out, "%s(= (grind-cost %s) %d)" % (indent, self.name, self.size * 15)
    print >> out, "%s(= (plane-cost %s) %d)" % (indent, self.name, self.size * 10)
  def dump_goal(self, indent="", out=None):
    print >> out, "%s(available %s)" % (indent, self.name)
    for choice in self.goalselection:
      print >> out, "%s(%s %s %s)" % (indent, choice, self.name, 
                                      self.goalprops[choice])

class Board(object):
  def __init__(self, index, wood, size):
    self.wood = wood
    self.name = "b%d" % index
    self.size = size
    self.surface = random.choice(["rough","rough","rough","smooth"])
  def dump_init(self, indent="", out=None):
    print >> out, "%s(boardsize %s %s%d)" % (indent, self.name, 
                                              _boardsize_prefix, self.size)
    print >> out, "%s(wood %s %s)" % (indent, self.name, self.wood) 
    print >> out, "%s(surface-condition %s %s)" % (indent, self.name, 
                                           self.surface)
    print >> out, "%s(available %s)" % (indent, self.name)

class Machine(object):
  def __init__(self, name, type):
    self.name = name
    self.type = type
    self.colours = set()
  def dump_init(self, indent, out=None):
    if self.type == "highspeed-saw":
      print >> out, "%s(empty %s)" % (indent, self.name)
    for colour in self.colours:
      print >> out, "%s(has-colour %s %s)" % (indent, self.name, colour)

class Task(object):
  def __init__(self, seed, nr_parts, wood_factor=1.0, problemtype=None, **additional_machines):
    self.seed = seed
    self.problemtype = problemtype
    self.wood_factor = wood_factor
    nr_colours = max(int(round(0.7*nr_parts)), 2)
    self.colours = random.sample(_colours, min(nr_colours, len(_colours)))
    nr_woods = max(int(round(0.25*nr_parts)), 2)
    self.woods = random.sample(_woods, min(nr_woods, len(_woods)))
    self.parts = [Part(nr, self.woods, self.colours, problemtype) 
                  for nr in xrange(nr_parts)]
    self.max_board_size = self._generate_boards()
    self._generate_machines(**additional_machines)
    self._assign_colours_to_machines()
  def _generate_boards(self):
    quantities = dict()
    for part in [p for p in self.parts if not p.initprops]:
      wood = part.goalprops["wood"]
      quantities.setdefault(wood,[]).append(part.size)

    self.boards = []
    counter = 0
    maxsize = 0
    for wood, quantity in quantities.iteritems():
      if not quantity:
        self.woods.remove("wood")
      else:
        additional_wood = int(ceil(sum(quantity) * (self.wood_factor-1)))
        while additional_wood > 3:
          rand = random.randint(1,3)
          quantity.append(rand)
          additional_wood -= rand
        quantity.append(additional_wood)
        random.shuffle(quantity)
        for amounts in zip(quantity[::4], 
                           quantity[1::4] + [0], 
                           quantity[2::4] + [0], 
                           quantity[3::4] + [0]):
          size = sum(amounts)
          maxsize = max(maxsize, size)
          self.boards.append(Board(counter, wood, size))
          counter += 1
    return maxsize
  def _generate_machines(self, **changes):
    machines = dict(_default_machines)
    machines.update(changes)
    self.machines = dict()
    for type, number in machines.iteritems():
      if number:
        m = [Machine("%s%s" % (type,nr), type) 
             for nr in xrange(number)]
        self.machines[type] = m
  def _assign_colours_to_machines(self):
    necessary = self._determine_necessary_colours()
    spray_varnishers = self.machines.get("spray-varnisher",[])
    immersion_varnishers = self.machines.get("immersion-varnisher",[])
    glazers = self.machines.get("glazer",[])
    for colour in necessary["varnished"]:
      machines = random.sample(spray_varnishers, 
                               int(ceil(0.6*len(spray_varnishers))))
      machines += random.sample(immersion_varnishers, 
                               int(ceil(0.6*len(immersion_varnishers))))
      for m in machines:
        m.colours.add(colour)
    for colour in necessary["glazed"]:
      for m in random.sample(glazers, int(ceil(0.6*len(glazers)))):
        m.colours.add(colour)
    for colour in necessary["unspecified"]:
      machines = random.sample(spray_varnishers, 
                               int(ceil(0.3*len(spray_varnishers))))
      machines += random.sample(immersion_varnishers, 
                               int(ceil(0.3*len(immersion_varnishers))))
      machines += random.sample(glazers, int(ceil(0.3*len(glazers))))
      for m in machines:
        m.colours.add(colour)
    for type in ["spray-varnisher", "immersion-varnisher", "glazer"]:
      for m in self.machines.get(type, []):
        if not m.colours:
          m.colours.add(random.choice(list(self.colours)))
  def _determine_necessary_colours(self):
    used_colours = dict()
    used_colours["varnished"] = set()
    used_colours["glazed"] = set()
    used_colours["unspecified"] = set()
    for part in self.parts:
      if "colour" in part.goalselection:
        colour = part.goalprops["colour"]
        if "treatment" in part.goalselection:
          used_colours[part.goalprops["treatment"]].add(colour)
        else:
          used_colours["unspecified"].add(colour)
    return used_colours 
  def dump(self, out=None):
    self._dump_header(out)
    self._dump_objects("  ", out)
    self._dump_init("  ", out)
    self._dump_goal("  ", out)
    self._dump_metric("  ", out)
    print >> out, ")"
  def _dump_header(self, out=None):
    print >> out, "; woodworking task with %s parts and %d%% wood"\
                    % (len(self.parts), self.wood_factor * 100)
    print >> out, "; Machines:"
    for type, machines in self.machines.iteritems():
      print >> out, ";   %d %s" % (len(machines), type)
    print >> out, "; random seed: %d" % self.seed
    print >> out
    print >> out, "(define (problem wood-prob)"
    print >> out, "  (:domain woodworking)"
  def _dump_objects(self, indent="", out=None):
    print >> out, indent + "(:objects"
    for type, machines in self.machines.iteritems():
      print >> out, "%s  %s - %s" % (indent, 
                          " ".join([m.name for m in machines]), type)
    print >> out, "%s  %s - acolour" % (indent, " ".join(self.colours))
    print >> out, "%s  %s - awood" % (indent, " ".join(self.woods))
    print >> out, "%s  %s - part" % (indent, 
                          " ".join([p.name for p in self.parts]))
    print >> out, "%s  %s - board" % (indent, 
                           " ".join([b.name for b in self.boards]))
    boardsizes = ["%s%d" % (_boardsize_prefix, size)  
                  for size in range(self.max_board_size + 1)]
    print >> out, "%s  %s - aboardsize" % (indent, " ".join(boardsizes))
    print >> out, indent + ")"
  def _dump_init(self, indent="", out=None):
    print >> out, indent + "(:init"
    print >> out, indent + "  (grind-treatment-change varnished colourfragments)"
    print >> out, indent + "  (grind-treatment-change glazed untreated)"
    print >> out, indent + "  (grind-treatment-change untreated untreated)"
    print >> out, indent + "  (grind-treatment-change colourfragments untreated)"
    print >> out, indent + "  (is-smooth smooth)"
    print >> out, indent + "  (is-smooth verysmooth)"
    print >> out, indent + "  (= (total-cost) 0)"
    for size in range(self.max_board_size):
      print >> out, "%s  (boardsize-successor %s%d %s%d)" % (indent,
                          _boardsize_prefix, size,  _boardsize_prefix, size + 1)
    for type in self.machines:
      for m in self.machines[type]:
        m.dump_init(indent + "  ", out)
    for part in self.parts:
      part.dump_init(indent + "  ", out)
    for board in self.boards:
      board.dump_init(indent + "  ", out)
    print >> out, indent + ")"
  def _dump_goal(self, indent="", out=None):
    print >> out, indent + "(:goal"
    print >> out, indent + "  (and"
    for part in self.parts:
      part.dump_goal(indent + "    ", out)
    print >> out, indent + "  )"
    print >> out, indent + ")"
  def _dump_metric(self, indent="", out=None):
    print >> out, indent + "(:metric minimize (total-cost))"
   
def reseed():
  new_seed = random.randrange(10 ** 6)
  random.seed(new_seed)
  return new_seed

def generate_instance(nr_parts, wood_factor):
  seed = reseed()
  return Task(seed, nr_parts, wood_factor) 

if __name__ == "__main__":
  import random
  random.seed(2011 * 2011)

  tasks = [generate_instance(size, wood_factor)
         for wood_factor in [1.2, 1.4]
         for size in range (20, 26, 3)]
  for no, task in enumerate(tasks):
    task.dump(out=open("seq-sat/p%02d.pddl" % (no + 17), "w"))
  tasks = [generate_instance(size, wood_factor)
         for wood_factor in [1.4, 1.2, 1.0]
         for size in range (3, 13)]
  for no, task in enumerate(tasks):
    task.dump(out=open("seq-opt/p%02d.pddl" % (no + 1), "w"))

