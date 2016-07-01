#!/usr/bin/env python
import sys,os

worldsize = sys.argv[1]
ntables = sys.argv[2]
ncupboards = sys.argv[3]
minsurfacesize = sys.argv[4]
maxsurfacesize = sys.argv[5]
cupboardsize = sys.argv[6]
seed = sys.argv[7]

script_dir = os.path.realpath(sys.path[0])

cmd = "java -jar {script_dir}/tidybot-1.0.1-snapshot-standalone.jar {worldsize} {ntables} {ncupboards} {minsurfacesize} {maxsurfacesize} {cupboardsize} {seed}".format(**locals())
print(cmd)

failure = os.system(cmd)
if failure:
    print "Error when running " + cmd
    exit(-1)

exit(0)
