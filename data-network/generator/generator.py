#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# Usage:
# $./generator.py [number of data items] [number of layers] [number of scripts] network
#
# 
# File: network
# --------------------------------------------------------------------
# 1 2 4 8 16 32 % file sizes
# 0.1 0.1 0.2 0.3 0.2 0.1 % probability distribution or frequency distribution over file sizes (write 0 for uniform distribution)
#
# % list of servers
# 56 % capacity
# 1 % base io cost
# 1 2  % base processing costs 
# 0.2 0.2 0.6 % probability distribution over processing costs with additional probability for disabled script (write 0 for uniform distribution)
# 
# 32
# 2
# 2
# 1.0 0.0
# 
# 96
# 1
# 1 3
# 0.1 0.9 0.0
# 
# 96 
# 2
# 2
# 0.5 0.5 
# 
# % list of bidirectional connections
# 1 2  % adjacent servers
# 1 % base send cost
# 
# 2 3 
# 2
# 
# 1 3
# 3
# 
# 1 4
# 2
# -------------------------------------------------------------------

import sys
import numpy
import re
import importlib
from collections import OrderedDict


class DistributedComputing():
    def __init__(self, num_data_items, num_layers, num_scripts, filename, seed):
        self.layer_distr = []
        self.servers = {} 
        self.connections = {}
        self.initial_data_items = []
        self.goal_data_items = []
        self.task_name = ""
        self.max_capacity = 0
        self.data_items = {}
        self.scripts = {}
        self.server_script_pairs = {}
        self.server_data_item_pairs_init = []
        self.server_data_item_pairs_goal = []
        self.process_costs = {}
        self.send_costs = {}
        self.io_costs = {}

        self.num_data_items = num_data_items
        self.num_layers = num_layers
        self.num_scripts = num_scripts
        self.seed = seed

        numpy.random.seed(seed)

        self.task_name = filename.split('.')[0]
        self._parse_module(filename)
            
        self._check_input_values()
        self._generate_data_items()
        self._assign_sizes_to_data_items()
        self._generate_scripts()
        self._determine_max_capacity()
        self._check_required_capacity()
        self._assign_scripts_to_servers()
        self._assign_initial_data_items_to_servers()
        self._assign_goal_data_items_to_servers()
        self._compute_process_costs()
        self._compute_send_costs()
        self._compute_io_costs()
        self._write_pddl()

    def _check_input_values(self):
        if self.num_data_items <= self.num_layers:
            print ("[number of layers] must be smaller than [number of data items]")
            sys.exit(0)
        if self.num_scripts < self.num_data_items-2:
            print ("[number of scripts] must be larger or equal than [number of data items]-2")
            sys.exit(0)


    def _parse_module(self, module):
        m = importlib.import_module(module)
        
        self.data_size_distr = getattr(m, 'FILE_SIZE_DISTRIBUTION')
        self.servers = getattr(m, 'SERVERS')
        self.connections = getattr(m, 'CONNECTIONS')

    def _clean_line(self,line):
        line = line.split('%', 1)[0]
        line = line.rstrip()
        return line

    def _skip_empty_lines(self, lines, line_nr):
        while True:
            try:
                line = self._clean_line(lines[line_nr])
                if line != '':
                    return line_nr
                line_nr += 1;
            except IndexError:
                raise
        return line_nr

    def _read_numbers(self, line, line_nr, error_attribute):
        numbers = []
        try:
            for size in line.split(' '):
                numbers.append(int(size))
            if not numbers:
                raise ValueError
        except ValueError:
            print("Expected '{}' of line {} to be a space separated list of {}.".format(line, line_nr, error_attribute))
            sys.exit(0)
        return numbers

    def _read_distribution(self, line, line_nr):
        probabilities = []
        try:
            for size in line.split(' '):
                probabilities.append(float(size))
            if not probabilities:
                raise ValueError
        except ValueError:
            print("Expected '{}' of line {} to be 0 or a space separated list of probabilities or frequencies.".format(line, line_nr))
            sys.exit(0)
        return probabilities

    def _generate_data_items(self):
        for i in range(self.num_data_items):
            self.data_items["data{}".format(i+1)] = {}

    def _assign_sizes_to_data_items(self):
        self.data_sizes = set()
        for data_item in self.data_items:
            size = self.data_size_distr()
            self.data_items[data_item]['size'] = size
            self.data_sizes.add(size)
        self.data_sizes = list (self.data_sizes)

    def _generate_scripts(self):
        layers = self._distribute_data_items_to_layers()
        self.initial_data_items = layers[0][:]
        items_used_in_scripts = set()
        script_nr = 1
        # Generate at one script for each data item 
        for layer_nr in range(1,self.num_layers):
            for data_item in layers[layer_nr]:
                script = "script{}".format(script_nr)
                properties = {}
                input1 = numpy.random.choice(layers[layer_nr-1])
                # Enforce different data inputs 
                input2 = input1
                while input2 == input1:
                    random_layer = numpy.random.choice(layer_nr)
                    input2 = numpy.random.choice(layers[random_layer])
                items_used_in_scripts.add(input1)
                items_used_in_scripts.add(input2)
                
                properties['input1'] = input1
                properties['input2'] = input2
                properties['output'] = data_item
                self.scripts[script] = properties
                script_nr += 1
        # Distribute remaining scripts among layers
        while script_nr <= self.num_scripts:
            layer_nr = numpy.random.choice(range(1,self.num_layers))
            output = numpy.random.choice(layers[layer_nr])
            script = "script{}".format(script_nr)
            properties = {}
            input1 = numpy.random.choice(layers[layer_nr-1])
            # Enforce different data inputs 
            input2 = input1
            while input2 == input1:
                random_layer = numpy.random.choice(layer_nr)
                input2 = numpy.random.choice(layers[random_layer])
            items_used_in_scripts.add(input1)
            items_used_in_scripts.add(input2)
            properties['input1'] = input1
            properties['input2'] = input2
            properties['output'] = output

            
            self.scripts[script] = properties
            script_nr += 1


        self.goal_data_items = [item for layer in layers for item in layer if not item in items_used_in_scripts]

    def _distribute_data_items_to_layers(self):
        distributed_items = 0
        layers = []
        for data_item in self.data_items:
            # Enforce at least one data item on each layer
            if distributed_items < self.num_layers: 
                layers.append([])
                layers[distributed_items].append(data_item)
            # First layer needs at least two items
            elif distributed_items == self.num_layers:
                layers[0].append(data_item)
            else:
                # TODO: Use other distribution than uniform distribution
                layer = numpy.random.choice(self.num_layers)
                layers[layer].append(data_item)
            distributed_items += 1
        layers = self._add_layer_to_data_item_names(layers)
        return layers

    def _add_layer_to_data_item_names(self, layers):
        renamed_data_items = {}
        renamed_on_layers = []
        layer_nr = 0
        for layer in layers:
            renamed_on_layers.append([])
            for data_item in layer:
                renamed_data_item = "data-{}-{}".format(layer_nr,data_item.replace("data",""))
                renamed_data_items[renamed_data_item] = self.data_items[data_item]
                renamed_on_layers[layer_nr].append(renamed_data_item)
            layer_nr += 1
        self.data_items = renamed_data_items
        return renamed_on_layers


    def _determine_max_capacity(self):
        self.max_capacity = max([properties['capacity'] for server, properties in self.servers.items()])

    def _check_required_capacity(self):
        min_space = max([value['size'] for (key, value) in self.data_items.items()])
        max_space = max([self.data_items[value['input1']]['size'] + 
                         self.data_items[value['input2']]['size'] + 
                         self.data_items[value['output']]['size'] for (key, value) in self.scripts.items()])
        for server, properties in self.servers.items():
            capacity = properties['capacity']
            if capacity < min_space:
                print("Server {} has not enough capacity to cache data item of size {}.".format(server,min_space))
                sys.exit(0)
        if max_space > self.max_capacity:
            print("Any server has not enough capacity to process script that requires {} space.".format(max_space))
            sys.exit(0)
        
    def _assign_scripts_to_servers(self):
        try:
            for server, properties in self.servers.items():
                for script, value in self.scripts.items():
                    self.server_script_pairs[(server,script)] = \
                    {'base_processing_cost' : properties['processing_distr']()}
        except ValueError:
            print("Expect 0 or probabilities sum to 1.")
            sys.exit(0)

    def _assign_initial_data_items_to_servers(self):
        servers = self.servers.keys()
        for data_item in self.initial_data_items:
            
            server = numpy.random.choice([s for s in servers])
            self.server_data_item_pairs_init.append((server,data_item))

    def _assign_goal_data_items_to_servers(self):
        servers = self.servers.keys()
        for data_item in self.goal_data_items:
            server = numpy.random.choice([s for s in servers])

            self.server_data_item_pairs_goal.append((server,data_item))

    def _compute_process_costs(self):
        for pair, props in self.server_script_pairs.items():
            if props['base_processing_cost'] >= 0:
                self.process_costs[(pair[1],pair[0])] = props['base_processing_cost']

    def _compute_send_costs(self):
        for data_size in self.data_sizes:
            for connection, props in self.connections.items():
                self.send_costs[(("server{}".format(connection[0]),"server{}".format(connection[1])), 
                                  "number{}".format(data_size))] = data_size * props['send']

    def _compute_io_costs(self):
        for data_size in self.data_sizes:
            for server, props in self.servers.items():
                self.io_costs[(server,"number{}".format(data_size))] = data_size * props['io']

    def _write_pddl(self):
        print ("(define (problem p{}-{}-{}-{}-{})".format(self.num_data_items, self.num_layers, self.num_scripts, self.task_name, self.seed))
        print ("    (:domain data-network)")
        self._write_objects()
        self._write_initial_state()
        self._write_goals()
        print ("    (:metric minimize (total-cost))")
        print (")")

    def _write_objects(self):
        data_items = sorted([data_item for data_item in self.data_items], key=lambda x: self._natural_keys(x))
        print ("    (:objects")
        for data_item in data_items[:-1]:
            print ("              {}".format(data_item))
        print ("              {} - data".format(data_items[-1]))

        scripts = sorted([script for script in self.scripts], key=lambda x: self._natural_keys(x))
        for script in scripts[:-1]:
            print ("              {}".format(script))
        print ("              {} - script".format(scripts[-1]))
        servers = sorted([server for server in self.servers], key=lambda x: self._natural_keys(x))
        for server in servers[:-1]:
            print ("              {}".format(server))
        print ("              {} - server".format(servers[-1]))
        for number in range(0,self.max_capacity):
            print ("              number{}".format(number))
        print ("              number{} - numbers".format(self.max_capacity))
        print ("    )".format(self.max_capacity))

    def _write_initial_state(self):
        print ("    (:init")
        self._write_constant_predicates()
        self._write_cost_predicates()
        self._write_predicates()
        print("    )")

    def _write_constant_predicates(self):
        scripts = OrderedDict(sorted(self.scripts.items(), key=lambda x: self._natural_keys(x[0])))
        for script, properties in scripts.items():
            print ("           (SCRIPT-IO {} {} {} {})".format(script, properties['input1'], properties['input2'], properties['output']))
        script_server_pairs = sorted([(pair[1],pair[0]) for pair in self.server_script_pairs if self.server_script_pairs[pair]['base_processing_cost'] >= 0],
                                     key=lambda x: self._natural_keys(x[0]+x[1]))
        connections = sorted([connection for connection in self.connections])
        for connection in connections:
            print ("           (CONNECTED server{} server{})".format(connection[0], connection[1]))
            print ("           (CONNECTED server{} server{})".format(connection[1], connection[0]))
        data_items = OrderedDict(sorted(self.data_items.items(), key=lambda x: self._natural_keys(x[0])))
        for data_item, properties in data_items.items():
            print ("           (DATA-SIZE {} number{})".format(data_item, properties['size']))
        servers = OrderedDict(sorted(self.servers.items(), key=lambda x: self._natural_keys(x[0])))
        for server, properties in servers.items():
            print ("           (CAPACITY {} number{})".format(server, properties['capacity']))
        self._write_math()

    def _write_math(self):
        for i in range (0, self.max_capacity+1):
            for j in sorted(self.data_sizes):
                if i+j <= self.max_capacity:
                    print ("           (SUM number{} number{} number{})".format(i, j, i+j))
        capacities = sorted(set([value['capacity'] for (key,value) in self.servers.items()]))
        for i in range (1, self.max_capacity+1):
            for j in capacities:
                if i <= j:
                    print ("           (LESS-EQUAL number{} number{})".format(i, j,))
        
    def _write_predicates(self):
        init_server_data = sorted([(pair[1],pair[0]) for pair in self.server_data_item_pairs_init], key=lambda x: self._natural_keys(x[0]))
        for pair in init_server_data:
            print ("           (saved {} {})".format(pair[0], pair[1]))
        servers = sorted([server for server in self.servers], key=lambda x: self._natural_keys(x))
        for server in servers:
            print ("           (usage {} number0)".format(server))

    def _write_cost_predicates(self):
        print ("           (= (total-cost) 0)")
        process_costs = OrderedDict(sorted(self.process_costs.items(), key=lambda x: self._natural_keys("{}{}".format(x[0],x[1]))))
        for pair,value in process_costs.items():
            print ("           (= (process-cost {} {}) {})".format(pair[0], pair[1], value))
        send_costs = OrderedDict(sorted(self.send_costs.items(), key=lambda x: self._natural_keys("{}{}{}".format(x[0][0],x[0][1],x[1]))))
        for pair,value in send_costs.items():
            print ("           (= (send-cost {} {} {}) {})".format(pair[0][0], pair[0][1], pair[1], value))
            print ("           (= (send-cost {} {} {}) {})".format(pair[0][1], pair[0][0], pair[1], value))
        io_costs = OrderedDict(sorted(self.io_costs.items(), key=lambda x: self._natural_keys("{}{}".format(x[0],x[1]))))
        for pair,value in io_costs.items():
            print ("           (= (io-cost {} {}) {})".format(pair[0], pair[1], value))

    def _write_goals(self):
        print ("    (:goal")
        print ("           (and")
        goal_server_data = sorted([(pair[1],pair[0]) for pair in self.server_data_item_pairs_goal], key=lambda x: self._natural_keys(x[0]))
        for pair in goal_server_data:
            print ("                (saved {} {})".format(pair[0], pair[1]))
        print("           )")
        print("    )")

    def _atoi(self, text):
        return int(text) if text.isdigit() else text

    def _natural_keys(self, text):
        return [ self._atoi(c) for c in re.split('(\d+)', text) ]



if __name__ == "__main__":

    if len(sys.argv) == 2:
        print("Generate IPC instances")
        filename = sys.argv[1]
        seed = 0
        for num_layers in range(2, 6):
            for num_data_items in range (num_layers+2, num_layers*10, num_layers):
                for num_scripts in range(num_data_items + 2, num_data_items*3, num_data_items//2):

                    sys.stdout = open("instances/p{}-{}-{}-{}-{}.pddl".format(num_data_items, num_layers, num_scripts, filename, seed), "w")
                    seed  += 1
                    generator = DistributedComputing(num_data_items, num_layers, num_scripts, filename, seed)


        exit(0)
    
    if len(sys.argv) < 5:
        print ("Call {} [number of data items] [number of layers] [number of scripts] network [seed]".format(sys.argv[0]))
        sys.exit(0)
    num_data_items = int(sys.argv[1])
    num_layers = int(sys.argv[2])
    num_scripts = int(sys.argv[3])
    filename = sys.argv[4]
    seed = int(sys.argv[5])
    generator = DistributedComputing(num_data_items, num_layers, num_scripts, filename, seed)

