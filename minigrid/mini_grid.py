#! /usr/bin/env python

import argparse
from collections import deque
from copy import deepcopy
import logging
from pathlib import Path
import random
import sys
from typing import Optional, Set


class Map:
    def __init__(self, nodes: int):
        self.nodes = nodes
        self.edges = [[] for n in range(self.nodes)]

    def add_edge(self, src: int, dst: int):
        if dst not in self.edges[src]:
            self.edges[src].append(dst)
            self.edges[dst].append(src)

    def remove_edges_at(self, node: int):
        assert 0 <= node and node <= self.nodes
        for i, edges in enumerate(self.edges):
            self.edges[i] = [dst for dst in edges if dst != node]
        self.edges[node] = []

    def clone(self):
        return deepcopy(self)

    # do BFS exploration
    def path(self, init: int, goal: int):
        assert 0 <= init and init < self.nodes and goal < self.nodes
        q = deque()
        q.append(init)
        explored = set([init])
        parent = [None for _ in range(self.nodes)]
        while len(q) > 0:
            src = q.popleft()
            assert src in explored
            if src == goal: break
            for dst in random.sample(self.edges[src], len(self.edges[src])):
                if dst not in explored:
                    explored.add(dst)
                    parent[dst] = src
                    q.append(dst)

        if goal < 0:
            return explored
        elif parent[goal] is None:
            return None
        else:
            src = goal
            rpath = []
            while src != init:
                rpath.append(src)
                src = parent[src]
                assert src is not None
            rpath.append(init)
            return list(reversed(rpath))

    def reachable_nodes(self, init: int):
        return self.path(init, -1)

class Floorplan:
    def __init__(self, fname: Path, logger):
        # read template file
        with fname.open('r') as fd:
            self.lines = [line.strip('\n') for line in fd.readlines()]

        # initialize data structures
        self.cells, self.cmap = [], dict()
        self.locks = set()
        self.logger = logger
        self.robot = None
        self.goal = None

        # first pass, read cells
        for r, line in enumerate(self.lines):
            for c, char in enumerate(line):
                if char not in [' ', 'w']:
                    cell = (c, r)
                    self.cmap[cell] = len(self.cells)
                    self.cells.append(cell)
                    if char == 'L': self.locks.add(cell)
                    if char == 'R': self.robot = cell
                    if char == 'G': self.goal = cell

        # second pass, read connections
        self.map = Map(len(self.cells))
        for (c, r) in self.cells:
            cell = (c, r)
            src = self.cmap[cell]
            leftcell = (c-1, r)
            if leftcell in self.cmap:
                dst = self.cmap[leftcell]
                self.map.add_edge(src, dst)
            abovecell = (c, r-1)
            if abovecell in self.cmap:
                dst = self.cmap[abovecell]
                self.map.add_edge(src, dst)
        logger.info(f'Floorplan: ncells={len(self.cells)}, locks={self.locks}')

    def get_map(self, locks: Optional[Set] = None):
        map = self.map.clone()
        if locks != None:
            for lock in locks:
                map.remove_edges_at(self.cmap[lock])
        return map


class Instance:
    def __init__(self, name: str, floorplan: Floorplan, nshapes: int, logger):
        self.name = name
        self.logger = logger
        self.floorplan = floorplan

        # assign shapes to locks, there would be one key per shape
        nlocks = len(floorplan.locks)
        if nshapes == 0:
            nshapes = random.randint(1, nlocks) if nlocks > 0 else 0
        self.lock_shapes = list(map(lambda x: x, random.choices(range(nshapes), k=nlocks)))
        self.key_shapes = list(range(0, nshapes))

        self.shape_map = dict()
        for i, cell in enumerate(floorplan.locks):
            self.shape_map[cell] = self.lock_shapes[i]
        logger.info(f'Locks: nlocks={nlocks}, nshapes={nshapes}, shape_map={self.shape_map}')

        # place robot and goal
        free_cells = list(set(floorplan.cells) - floorplan.locks)
        self.robot = random.sample(free_cells, k=1)[0] if floorplan.robot is None else floorplan.robot
        if floorplan.goal is None:
            reachable_cells = floorplan.map.reachable_nodes(floorplan.cmap[self.robot])
            self.goal = floorplan.cells[random.sample(list(reachable_cells), k=1)[0]]
        else:
            self.goal = floorplan.goal
        logger.debug(f'Init/goal: robot={self.robot}, goal={self.goal}')

        # construct high-level path from robot to goal
        robot_to_goal_path = floorplan.map.path(floorplan.cmap[self.robot], floorplan.cmap[self.goal])
        assert robot_to_goal_path is not None, f"With all locks open, cell {self.goal} isn't reachable from {self.robot}"
        lock_path = [floorplan.cells[cell] for cell in robot_to_goal_path if floorplan.cells[cell] in floorplan.locks]
        logger.info(f'Path: init={self.robot}, goal={self.goal}, path={list(map(lambda i: floorplan.cells[i], robot_to_goal_path))}, locks={lock_path}')

        # place keys so that resulting placement is solvable
        locks = set(floorplan.locks)
        self.key_locations = [None for _ in range(nshapes)] # one key per shape
        current_map = floorplan.get_map(locks)
        current_pos = floorplan.cmap[self.robot]
        previous_loc = set([current_pos])
        for i, lock in enumerate(lock_path):
            shape = self.shape_map[lock]
            logger.debug(f'i={i}, current_pos={floorplan.cells[current_pos]}, lock={lock}, shape={shape}')
            if self.key_locations[shape] == None:
                reachable_cells = current_map.reachable_nodes(current_pos) - previous_loc
                self.key_locations[shape] = random.sample(list(reachable_cells), k=1)[0]
                logger.debug(f'key_location: shape={shape}, loc={floorplan.cells[self.key_locations[shape]]}, reachable_cells={list(map(lambda i: floorplan.cells[i], reachable_cells))}')
                current_pos = self.key_locations[shape]
                previous_loc.add(current_pos)
                open_locks = set([lock for lock in floorplan.locks if self.shape_map[lock] == shape])
                logger.debug(f'open_locks={open_locks}')
                locks -= open_locks
                current_map = floorplan.get_map(locks)

        # keys that have not yet placed can be anywhere
        for i, loc in enumerate(self.key_locations):
            if loc is None:
                self.key_locations[i] = floorplan.cmap[random.sample(list(floorplan.cells), k=1)[0]]

    def write(self, fname: Path):
        with fname.open('w') as fd:
            fd.write(f'(define (problem {self.name})\n')
            fd.write(f'  (:domain grid)\n')

            # objects
            fd.write(f'  (:objects\n')
            fd.write(f'   ')
            for i, (c, r) in enumerate(self.floorplan.cells):
                fd.write(f' p{i}')
            fd.write(f'\n   ')
            for i, shape in enumerate(self.key_shapes):
                fd.write(f' shape{shape}')
            fd.write(f'\n   ')
            for i, shape in enumerate(self.key_shapes):
                fd.write(f' key{i}')
            fd.write('\n  )\n')

            # init: types
            fd.write(f'  (:init')
            fd.write(f'\n    ; Object types\n   ')
            for i, (c, r) in enumerate(self.floorplan.cells):
                fd.write(f' (place p{i})')
            fd.write(f'\n   ')
            for i, shape in enumerate(self.key_shapes):
                fd.write(f' (shape shape{shape})')
            fd.write(f'\n   ')
            for i, shape in enumerate(self.key_shapes):
                fd.write(f' (key key{i})')

            # init: open/locked cells
            fd.write(f'\n    ; Open/locked cells\n   ')
            for i, (c, r) in enumerate(self.floorplan.cells):
                if (c, r) not in self.floorplan.locks:
                    fd.write(f' (open p{i})')
            if len(self.floorplan.locks) > 0:
                fd.write(f'\n   ')
                for i, (c, r) in enumerate(self.floorplan.cells):
                    if (c, r) in self.floorplan.locks:
                        fd.write(f' (locked p{i})')
            fd.write('\n')

            # init: connected cells
            fd.write(f'    ; Connected cells\n')
            for src in range(self.floorplan.map.nodes):
                for dst in self.floorplan.map.edges[src]:
                    fd.write(f'    (conn p{src} p{dst})\n')

            # init: assign shapes
            fd.write(f'    ; Lock and key shapes\n')
            for cell in self.shape_map:
                fd.write(f'    (lock-shape p{self.floorplan.cmap[cell]} shape{self.shape_map[cell]})\n')
            for i, shape in enumerate(self.key_shapes):
                fd.write(f'    (key-shape key{i} shape{shape})\n')

            # init: place keys
            fd.write(f'    ; Key placement\n')
            for i, loc in enumerate(self.key_locations):
                fd.write(f'    (at key{i} p{loc})\n')

            # init: robot
            fd.write(f'    ; Robot placement\n')
            fd.write(f'    (at-robot p{self.floorplan.cmap[self.robot]})\n')
            fd.write(f'    (arm-empty)\n')
            fd.write(f'  )\n')

            # goal
            fd.write(f'  (:goal (at-robot p{self.floorplan.cmap[self.goal]}))\n')
            fd.write(f')\n')


def get_logger(name: str, log_file: Path, level = logging.INFO):
    logger = logging.getLogger(name)
    logger.propagate = False
    logger.setLevel(level)

    # add stdout handler
    #formatter = logging.Formatter('[%(levelname)s] %(message)s')
    formatter = logging.Formatter('%(asctime)s [%(levelname)s] [%(funcName)s:%(lineno)d] %(message)s')
    console = logging.StreamHandler(sys.stdout)
    console.setFormatter(formatter)
    logger.addHandler(console)

    # add file handler
    if log_file != '':
        formatter = logging.Formatter('%(asctime)s [%(levelname)s] [%(funcName)s:%(lineno)d] %(message)s')
        file_handler = logging.FileHandler(str(log_file), 'a')
        file_handler.setFormatter(formatter)
        logger.addHandler(file_handler)

    return logger

def close_logger(logger):
    handlers = logger.handlers
    for handler in handlers:
       logger.removeHandler(handler)
       handler.close()

def get_args():
    # argument parser
    default_seed = 0
    default_debug_level = 0
    default_num_instances = 1
    parser = argparse.ArgumentParser('mini_grid.py')
    parser.add_argument('--seed', type=int, default=default_seed, help=f'Seed for random generator (default={default_seed})')
    parser.add_argument('--debug_level', type=int, default=default_debug_level, help=f'Set debug level (default={default_debug_level})')
    parser.add_argument('--num_instances', type=int, default=default_num_instances, help=f'Number of instances to generate (default={default_num_instances})')
    parser.add_argument('floorplan', type=Path, help='Filename for floorplan')
    parser.add_argument('nshapes', type=int, help='Number of shapes for locks (0 means choose it randomly)')

    # paths
    default_results_path = ''
    default_floorplans_path = ''
    paths = parser.add_argument_group('paths')
    paths.add_argument('--results', type=Path, default=default_results_path, help=f"Path to results folders (default='{default_results_path}')")
    paths.add_argument('--floorplans_path', type=Path, default=default_floorplans_path, help=f"Path to floorplans (default='{default_floorplans_path}')")

    # parse arguments
    args = parser.parse_args()
    return args

def main(args: argparse.Namespace):
    # setup logger and identify call
    log_file = Path('./log.txt')
    log_level = logging.INFO if args.debug_level == 0 else logging.DEBUG
    logger = get_logger('mini_grid.py', log_file, log_level)
    logger.info(f"Using log file '{log_file}'")
    logger.info(f'call=|{" ".join(sys.argv)}|')

    # set random seed
    random.seed(args.seed)

    # read floorplan
    floorplan = Floorplan(args.floorplans_path / args.floorplan, logger)

    # generate random instances
    name = args.floorplan.name
    args.results.mkdir(parents=True, exist_ok=True)
    for i in range(args.num_instances):
        name = f'grid_{name}_s{args.nshapes}_seed{args.seed}_n{i}'.replace('.', '_')
        pddl_name = args.results / Path(name).with_suffix('.pddl')
        instance = Instance(name, floorplan, args.nshapes, logger)
        instance.write(pddl_name)
        logger.info(f'{pddl_name} written!')


if __name__ == '__main__':
    # read arguments and do job
    args = get_args()
    main(args)

