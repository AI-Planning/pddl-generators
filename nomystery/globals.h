#ifndef GLOBALS_H
#define GLOBALS_H

#include "successor_generator.h"

#include <iostream>
#include <string>
#include <vector>
using namespace std;
#define NOT_CONNECTED -1

void output_pddl_file(int initial_fuel);
void usage(void);
bool process_command_line(int argc, char *argv[]);
void dump_problem();
void g_dump(int input);
void create_random_graph();
void compile_problem(vector<int>& initial_state, vector<int>& goal);
void select_initials_goals();

struct LocationInfo{
	int initials;
	int goals;
	LocationInfo(){
		initials = 0;
		goals = 0;
	}
	void dump(){
		cout << "initials: ";
		g_dump(initials);
		cout << "goals: ";
		g_dump(goals);
	}
};

extern int g_encoding;
extern int g_num_trucks;
extern int g_num_edges;
extern int g_num_locations;
extern int g_num_packages;
extern float g_n; /* number of edges will be (int) (g_n * g_num_locations) */
extern int g_m; /* random edge weights will be drawn from {1..g_m} */
extern float g_c;
extern int g_seed;
extern vector< vector<int> > g_graph;
extern vector<LocationInfo> g_loc_info;
extern vector<int> g_masks;
extern vector<int> g_p_initials;
extern vector<int> g_p_goals;
extern vector<int> g_t_initials;

extern SuccessorGenerator g_successor_generator;

#endif
