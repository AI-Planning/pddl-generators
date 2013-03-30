/*********************************************************************
 * Author: Hootan Nakhost, adapted from LAMA's code
 *********************************************************************/
#include "solver.h"
#include "successor_generator.h"
#include "string.h"
#include "globals.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <climits>
#include <algorithm>

using namespace std;

void Solver::solve(vector<State>& states) {
	for (int i = 0; i < states.size(); ++i) {
		SearchNode node = search_space.get_node(&states[i]);
		node.open_initial(0);
		open_list.insert(0, node.get_state_buffer());
	}
	while (step() != SOLVED);
}

int Solver::step() {
	bool complete = false;
	SearchNode node = fetch_next_node(complete);
	//TODO: also check for initial state
	if(complete){
		return SOLVED;
	}

	vector<State> successors;
	vector<int> costs;
	g_successor_generator.generate_successors(node.get_state(), successors, costs);
	for (int i = 0; i < successors.size(); i++) {
		//generated_states++;

		SearchNode succ_node = search_space.get_node(&successors[i]);
		int cost = costs[i];

		if (succ_node.is_dead_end()) {
			// Previously encountered dead end. Don't re-evaluate.
			continue;
		} else if (succ_node.is_closed()) {
			// Node already closed. No need to investigate further.
			continue;
		} else if (succ_node.is_open()) {
			// Node is open, so we do not have to re-evaluate it.
			// However, we may have reached it on a cheaper path
			// than previously.
			if (node.get_h() + cost < succ_node.get_h()) {
				succ_node.reopen(node.get_h() + cost);
				open_list.insert(succ_node.get_h(), succ_node.get_state_buffer());
			}
		} else {
			// We have not seen this state before.
			// Evaluate and create a new node.
			//evaluated_states++;
			int succ_h = node.get_h() + cost;
			succ_node.open(succ_h);
			open_list.insert(succ_h, succ_node.get_state_buffer());
		}
	}
	return IN_PROGRESS;
}

SearchNode Solver::fetch_next_node(bool& complete) {
	while (true) {
		if (open_list.empty()) {
			SearchNodeInfo node_info;
			SearchNode node(0, node_info);
			complete = true;
			return node;
		}
		State* state = open_list.remove_min();
		SearchNode node = search_space.get_node(state);

		if (!node.is_closed()) {
			node.close();
			assert(!node.is_dead_end());
			complete = false;
			return node;
		}
	}
}

void Solver::dump(){
	search_space.dump();
}

int Solver::get_cost(State& state){
	SearchNode node = search_space.get_node(&state);
	return node.get_h();
}

int main(int argc, char * argv[]) {


	g_num_trucks = 1;

	/* gn will stay fixed: I don't expect to go up to more than 10
	* locations; of 45 possible edges, we'll have 15 = a 3rd then,
	* which seems generous for a roadmap. at -l < 10, it'll be even
	* more than a 3rd.
	*/
	g_num_locations = 6;
	g_num_packages = 6;
	g_n = 1.5;
	g_m = 25;
	g_c = 1;
	g_seed = 1;
	g_encoding = 0;

	if ( !process_command_line( argc, argv ) ) {
		usage();
		exit( 1 );
	}


	srandom(g_seed);
	create_random_graph();
	select_initials_goals();
	//Exchanging goal with initial_state for backward search
	compile_problem(g_p_goals, g_p_initials);
	vector<State> goals;
	for (int i = 0; i < g_num_locations; ++i) {
		goals.push_back(State(i));
	}
	int all_delivered = 0;
	for (int i = 0; i < g_masks.size(); ++i) {
		all_delivered = all_delivered | g_masks[i];
	}
	State initial_state(all_delivered, 0, g_t_initials[0]);
	Solver solver;
	solver.solve(goals);
	//dump_problem();
	int initial_fuel = solver.get_cost(initial_state);
	output_pddl_file(initial_fuel * g_c);
	return 0;
}

