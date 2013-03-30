#include "globals.h"
#include "state.h"
#include "successor_generator.h"

#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

void SuccessorGenerator::generate_successors(const State &curr, vector<State> & succs, vector<int>& costs){

	vector<pair<int, int> > neighbors;
	get_neighbor_costs(curr.location, neighbors);
	for (int i = 0; i < neighbors.size(); ++i) {
		succs.push_back(State(curr, neighbors[i].first));
		costs.push_back(neighbors[i].second);
	}
}

void SuccessorGenerator::get_neighbor_costs(int curr_loc, vector<pair<int, int> >& neighbors){
	for (int i = 0; i < g_graph[curr_loc].size(); ++i) {
		int cost = g_graph[curr_loc][i];
		if(cost != NOT_CONNECTED){
			neighbors.push_back(make_pair(i, cost));
		}
	}
}
