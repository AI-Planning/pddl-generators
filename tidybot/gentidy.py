#!/usr/bin/env python

import os.path
import subprocess
import sys

JAR_NAME = "tidybot-1.0.1-snapshot-standalone.jar"

worldsize = sys.argv[1]
ntables = sys.argv[2]
ncupboards = sys.argv[3]
minsurfacesize = sys.argv[4]
maxsurfacesize = sys.argv[5]
cupboardsize = sys.argv[6]

script_dir = os.path.abspath(os.path.dirname(__file__))

cmd = [
    "java",
    "-jar",
    "{script_dir}/{JAR_NAME}".format(**locals()),
    worldsize,
    ntables,
    ncupboards,
    minsurfacesize,
    maxsurfacesize,
    cupboardsize,
]

if len(sys.argv) == 8:
    cmd.append(sys.argv[7])

subprocess.check_call(cmd)
