#!/usr/bin/env python
# Generator for the child-snack domain
#
# Authors: Raquel Fuentetaja and Tomas de la Rosa
#
# See domain file for description.
# The ratio parameter is the proportion of needed symbols
# that are declared in advance in the problem file. The min ratio
# should be 1.0 to guarantee solvability
#
# The generator is also able to generate problems in alternative
# representations:
#   (1) Pool: The standard pizza domain
#   (2) Control: A modified version with precondition control for
#   creating one slice at a time.
#   (3) Numeric: An equivalent version in numeric PDDL, where
#   no slice symbols have to be declared in advance.
#   

from math import ceil
import random
import sys

class Task(object):
  def __init__(self, seed,  n_children, n_trays, gluten_factor, domain_type="pool", const_ratio=1.2):
    self.type = domain_type
    self.seed = seed 
    self.n_children = n_children
    self.n_trays = n_trays
    self.gluten_factor = gluten_factor
    self.const_ratio = const_ratio
    
    self.n_sandwiches = int(ceil(self.n_children * self.const_ratio))

    self.tables = enum_objects("table", 3)

    self.trays = enum_objects("tray", self.n_trays)
    self.children = enum_objects("child", self.n_children)
    self.sandwiches = enum_objects("sandw", self.n_sandwiches)
    self.breads = enum_objects("bread", self.n_children)
    self.contents = enum_objects("content", self.n_children)

    self.n_allergie = int(self.n_children * self.gluten_factor)
    

  def dump(self, out=None):
    self._dump_header(out)
    self._dump_objects("  ", out)
    self._dump_init("  ", out)
    self._dump_goal("  ", out)
    #self._dump_metric("  ", out)
    print >> out, ")"

  def _dump_header(self, out=None):
    print >> out, "; child-snack task with %s children and %s gluten factor " % (self.n_children, self.gluten_factor)
    print >> out, "; constant factor of %s" % self.const_ratio
    print >> out, "; random seed: %d" % self.seed
    print >> out
    print >> out, "(define (problem prob-snack)"
    print >> out, "  (:domain child-snack)"

  def _dump_objects(self, indent="", out=None):
    print >> out, indent + "(:objects"
    print >> out, "%s  %s - child" % (indent, " ".join(self.children))
    print >> out, "%s  %s - bread-portion" % (indent, " ".join(self.breads))
    print >> out, "%s  %s - content-portion" % (indent, " ".join(self.contents))
    print >> out, "%s  %s - tray" % (indent, " ".join(self.trays))
    print >> out, "%s  %s - place" % (indent, " ".join(self.tables))
    
    if self.type != "numeric":
      print >> out, "%s  %s - sandwich" % (indent, " ".join(self.sandwiches))
    
    print >> out, indent + ")"

  def _dump_init(self, indent="", out=None):
    print >> out, indent + "(:init"

    for t in self.trays:
      print >> out, indent + "%s (at %s kitchen)" % (indent, t)
    for b in self.breads:
      print >> out, indent + "%s (at_kitchen_bread %s)" % (indent, b)
    for c in self.contents:
      print >> out, indent + "%s (at_kitchen_content %s)" % (indent, c)
      
    for b in random.sample(self.breads, self.n_allergie):
      print >> out, indent + "%s (no_gluten_bread %s)" % (indent, b)

    for c in random.sample(self.contents, self.n_allergie):
      print >> out, indent + "%s (no_gluten_content %s)" % (indent, c)
    
    allergie_children = set(random.sample(self.children, self.n_allergie))
    no_allergie_children = set(self.children).difference(allergie_children)

    for c in list(allergie_children):
      print >> out, indent + "%s (allergic_gluten %s)" % (indent, c)
    for c in list(no_allergie_children):
      print >> out, indent + "%s (not_allergic_gluten %s)" % (indent, c)
      
    for c in self.children:
      print >> out, indent + "%s (waiting %s %s)" % (indent, c, random.choice(self.tables))
      
    if self.type == "pool":
      for s in self.sandwiches:
        print >> out, indent + "%s (notexist %s)" % (indent, s)
     
    elif self.type == "control":
      print >> out, "%s   (current %s)" % (indent , self.sandwiches[0])
      for i in range(1, len(self.sandwiches)):
        print >> out, "%s   (next sandw%s sandw%s)" % (indent, i, i+1)
    elif self.type == "numeric":
      print >> out, "%s   (= (nsandwiches-at_kitchen-no_gluten)  0)" % indent
      print >> out, "%s   (= (nsandwiches-at_kitchen)  0)" % indent
      for t in self.trays:
        print >> out, "%s   (= (nsandwiches-ontray-no_gluten %s)  0)" % (indent,t)
        print >> out, "%s   (= (nsandwiches-ontray %s)  0)" % (indent,t)

    print >> out, indent + ")"

  def _dump_goal(self, indent="", out=None):
    print >> out, indent + "(:goal"
    print >> out, indent + "  (and"
    for g in self.children:
      print >> out, indent + "%s (served %s)" % (indent, g)
    print >> out, indent + "  )"
    print >> out, indent + ")"

  def _dump_metric(self, indent="", out=None):
    print >> out, indent + "(:metric minimize (total-cost))"

def enum_objects(pddltype, n):
  return ["%s%s" % (pddltype,i) for i in range(1,n+1)]


def usage():
  print "\n Incorrect input arguments "
  print "\n Usage: " +sys.argv[0] + " [pool | control | numeric] <seed> <num_children> <num_trays> <gluten_factor> <const_ratio>"
  print "\n    <const_ratio>: min 1.0"
  sys.exit(1)

if __name__ == "__main__":

  if len(sys.argv)< 6:
    usage()

  try:
    domain_type = sys.argv[1]
    seed = int(sys.argv[2])
    n_children = int(sys.argv[3])
    n_trays = int(sys.argv[4])
    gluten_factor = float(sys.argv[5])
    const_ratio = float(sys.argv[6])
    
    random.seed(seed)
    pddltask = Task(seed, n_children, n_trays, gluten_factor, domain_type, const_ratio)
    pddltask.dump()

    
  except:
    usage()

