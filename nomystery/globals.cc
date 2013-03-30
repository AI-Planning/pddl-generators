#include "globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage() {
	printf("\nusage:\n");
	printf("\nOPTIONS   DESCRIPTIONS\n\n");
	printf("-l <int>    number of locations (>= 2; preset: %d)\n", g_num_locations);
	// printf("-t <int>    number of trucks (>= 1; preset: %d)\n", gt);
	printf("-p <int>    number of packages (>= 1; preset: %d)\n\n", g_num_packages);
	printf("-n <float>  (int) (<-n> * <-l>) edges in location graph (>= 1; preset: %f)\n", g_n);
	printf("-m <int>    edge weights drawn from 1 .. <-m> (>= 1; preset: %d)\n\n", g_m);
	printf(
			"-c <float>  constrainedness: (int) (<-c> * <optimalcost>) will be the initial fuel supply (>= 1; preset: %f)\n\n",
			g_c);
	printf("-s <int>    random seed (>= 1; preset: %d)\n", g_seed);
	printf("-e < 0 | 1 > type of encoding (0 = hard; 1 = hard_cost; preset: %d)\n", g_encoding);
}

bool process_command_line(int argc, char *argv[]) {
	char option;
	while (--argc && ++argv) {
		if (*argv[0] != '-' || strlen(*argv) != 2) {
			return false;
		}
		option = *++argv[0];
		switch (option) {
		default:
			if (--argc && ++argv) {
				switch (option) {
				case 'e':
					sscanf(*argv, "%d", &g_encoding);
					break;
				case 'l':
					sscanf(*argv, "%d", &g_num_locations);
					break;
					/*case 't':
					 sscanf(*argv, "%d", &g_num_trucks);
					 break;*/
				case 'p':
					sscanf(*argv, "%d", &g_num_packages);
					break;
				case 'n':
					sscanf(*argv, "%f", &g_n);
					break;
				case 'm':
					sscanf(*argv, "%d", &g_m);
					break;
				case 'c':
					sscanf(*argv, "%f", &g_c);
					break;
				case 's':
					sscanf(*argv, "%d", &g_seed);
					break;
				default:
					cout << endl << endl << "unknown option: " << option << " entered" << endl << endl;
					return false;
				}
			} else {
				return false;
			}
		}
	}
	if (g_num_locations < 2 || g_num_trucks < 1 || g_num_packages < 1 || g_n < 1 || g_m < 1 || g_seed < 1) {
		return false;
	}
	return true;
}

void output_pddl_file(int initial_fuel) {

	int max_init_fuel = max(initial_fuel, g_m);
	int printn = (int) (((float) 100) * g_n);
	int printc = (int) (((float) 100) * g_c);
	printf("(define (problem transport-l%d-t%d-p%d---int100n%d-m%d---int100c%d---s%d---e%d)\n", g_num_locations,
			g_num_trucks, g_num_packages, printn, g_m, printc, g_seed, g_encoding);
	printf("(:domain transport-strips)\n\n");

	printf("(:objects\n");
	int i = 0;
	for (i = 0; i < g_num_locations; i++) {
		printf("l%d ", i);
	}

	printf("- location\n");
	for (i = 0; i < g_num_trucks; i++) {
		printf("t%d ", i);
	}
	printf("- truck\n");
	for (i = 0; i < g_num_packages; i++) {
		printf("p%d ", i);
	}
	printf("- package\n");
	for (i = 0; i <= max_init_fuel; i++) {
		printf("level%d ", i);
	}
	printf("- fuellevel\n");

	printf(")\n\n");

	printf("(:init\n");
	for (int post = 0; post <= max_init_fuel; post++) {
		for (int pre = post; pre <= max_init_fuel; pre++) {
			printf("(sum level%d level%d level%d)\n", post, pre - post, pre);
		}
	}
	printf("\n");

	for (i = 0; i < g_num_locations; i++) {
		for (int j = 0; j < g_num_locations; j++) {
			if (g_graph[i][j] != NOT_CONNECTED) {
				printf("(connected l%d l%d)\n", i, j);
				printf("(fuelcost level%d l%d l%d)\n", g_graph[i][j], i, j);
				if(g_encoding == 1)
					printf("(= (fuelcost l%d l%d) %d)", i, j, g_graph[i][j]);
			}
		}
	}
	printf("\n");

	for (i = 0; i < g_num_trucks; i++) {
		printf("(at t%d l%d)\n", i, g_t_initials[i]);
		printf("(fuel t%d level%d)\n", i, (initial_fuel));
	}
	printf("(= (total-cost) 0)\n");
	printf("\n");

	for (i = 0; i < g_num_packages; i++) {
		printf("(at p%d l%d)\n", i, g_p_initials[i]);
	}
	printf(")\n\n");
	printf("(:goal\n");
	printf("(and\n");
	for (i = 0; i < g_num_packages; i++) {
		printf("(at p%d l%d)\n", i, g_p_goals[i]);
	}
	printf(")\n");
	printf(")\n");
	printf("(:metric minimize (total-cost))");
	printf(")\n");
}

void create_random_graph() {

	/* first, create a spanning tree:
	 * keep a set have (initially one random node x) and a set
	 * havenot (initially all nodes except x); iteratively draw
	 * one node x from have and one node y from havenot,
	 * connect x and y, and move y from havenot to have.
	 *
	 * will insert exactly g_num_locations-1 edges. well whatever...
	 * compute requested number of edges, count.
	 */
	int num_edges = 0;
	g_graph.resize(g_num_locations);
	for (int i = 0; i < g_graph.size(); ++i) {
		g_graph[i].resize(g_num_locations, NOT_CONNECTED);
	}

	g_num_edges = (int) (g_n * ((float) g_num_locations));

	vector<bool> have(g_num_locations, false);
	int rnd = random() % g_num_locations;
	have[rnd] = true;
	int i, x, y = 0;
	while (true) {
		for (i = 0; i < g_num_locations; i++) {
			if (!have[i]) {
				break;
			}
		}
		if (i == g_num_locations) {
			break;
		}
		while (true) {
			x = random() % g_num_locations;
			if (have[x]) {
				break;
			}
		}
		while (true) {
			y = random() % g_num_locations;
			if (!have[y]) {
				break;
			}
		}
		have[y] = true;
		/* have to specify both directions, in this encoding.
		 */
		int c = (random() % g_m) + 1;
		g_graph[x][y] = c;
		g_graph[y][x] = c;
		num_edges++;
	}
	/* now insert the requested nr of additional edges.
	 */
	int j = 0;
	while (num_edges < g_num_edges) {
		for (i = 0; i < g_num_locations - 1; i++) {
			for (j = i + 1; j < g_num_locations; j++) {
				if (g_graph[i][j] == NOT_CONNECTED) {
					break;
				}
			}
			if (j < g_num_locations) {
				break;
			}
		}
		if (i == g_num_locations - 1) {
			/* aready fully connected!
			 */
			break;
		}
		while (true) {
			x = random() % g_num_locations;
			y = random() % g_num_locations;
			if (x != y && g_graph[x][y] == NOT_CONNECTED) {
				break;
			}
		}
		int c = (random() % g_m) + 1;
		g_graph[x][y] = c;
		g_graph[y][x] = c;
		num_edges++;
	}
}

void g_dump(int input) {
	//cout << "--- " << input << endl;
	if (input == 0) {
		cout << "--";
		return;
	}

	for (int i = 0; i < g_masks.size(); ++i) {
		if ((g_masks[i] & input) != 0) {
			//cout << (g_masks[i] & input) << endl;
			//cout << g_masks[i] << " = " << input << endl;
			cout << " p" << i;
		}
	}
}

void select_initials_goals() {

	for (int i = 0; i < g_num_trucks; i++) {
		// int r = random();
		// cout << "r: " << r << endl;
		g_t_initials.push_back(random() % g_num_locations);
		// cout << "t: " << g_t_initials[0] << endl;
	}

	for (int i = 0; i < g_num_packages; i++) {
		g_p_initials.push_back(random() % g_num_locations);
		while (true) {
			int goal = random() % g_num_locations;
			if (g_p_initials.back() != goal) {
				g_p_goals.push_back(goal);
				break;
			}
		}
	}
}

void dump_problem() {
	cout << "truck location: " << g_t_initials[0] << endl;
	for (int var = 0; var < g_p_initials.size(); ++var) {
		cout << " p" << var << " " << g_p_initials[var] << " -- " << g_p_goals[var];
	}
	cout << endl;
	cout << "graph: " << endl;
	for (int i = 0; i < g_graph.size(); ++i) {
		for (int j = 0; j < g_graph[i].size(); ++j) {
			if (g_graph[i][j] == NOT_CONNECTED)
				cout << " -";
			else
				cout << " " << g_graph[i][j];
		}
		cout << endl;
	}
	cout << endl;
}

void compile_problem(vector<int>& initial_state, vector<int>& goal) {
	g_masks.resize(g_num_packages, 0);
	int m = 1;
	for (int i = 0; i < g_masks.size(); ++i) {
		g_masks[i] = m;
		m = m << 1;
	}
	/*for (int i = 0; i < g_masks.size(); ++i) {
	 //cout << g_masks[i] << endl;
	 g_dump(g_masks[i]);
	 }*/
	int num_locations = g_graph.size();
	g_loc_info.resize(num_locations);

	for (int i = 0; i < initial_state.size(); ++i) {
		int loc = initial_state[i];
		g_loc_info[loc].initials = g_loc_info[loc].initials | g_masks[i];
	}

	for (int i = 0; i < goal.size(); ++i) {
		int loc = goal[i];
		g_loc_info[loc].goals = g_loc_info[loc].goals | g_masks[i];
	}
}

int g_num_packages;
int g_num_locations;
int g_num_edges;
int g_num_trucks;

int g_encoding;
int g_m;
float g_n;
float g_c;
int g_seed;

vector<int> g_p_initials;
vector<int> g_p_goals;
vector<int> g_t_initials;

vector<vector<int> > g_graph;
vector<LocationInfo> g_loc_info;
vector<int> g_masks;
SuccessorGenerator g_successor_generator;

