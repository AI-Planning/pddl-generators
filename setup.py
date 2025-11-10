#! /bin/env python3


import subprocess

print("building all PDDL generator binaries ...")
subprocess.run(["make", "-C", "pddl_generators"])


from setuptools import setup, find_packages


setup(name='pddl-generators',
      version='1.0',
      description='An unofficial collection of PDDL generators, '
      'some of which have been used to generate benchmarks for '
      'the International Planning Competitions (IPC).',
      author='Jendrik Seipp',
      author_email='jendrikseipp@gmail.com',
      url='https://github.com/AI-Planning/pddl-generators',
      # note: the hyphen in the "package name" is converted to underscore here
      entry_points={"console_scripts":["pddl-generators = pddl_generators.main:main"]},
      packages=find_packages(),
      install_requires=["z3","numpy"],
      include_package_data=True,
      )

