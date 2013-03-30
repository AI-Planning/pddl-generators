#!/usr/bin/env python
import sys,os

worldsize=sys.argv[1]
ntables=sys.argv[2]
ncupboards=sys.argv[3]
minsurfacesize=sys.argv[4]
maxsurfacesize=sys.argv[5]
cupboardsize=sys.argv[6]


cmd= "cd /data/software/sayphi/domains/tidybot/generator/tidybot/generator/;java -jar tidybot-1.0.0-snapshot-standalone.jar "+worldsize+" "+ntables+" "+ncupboards+" "+minsurfacesize+" "+maxsurfacesize+" "+cupboardsize
print(cmd)
failure= os.system(cmd)
if failure:
    print "Error when running " + cmd
    exit(-1)
exit(0)
