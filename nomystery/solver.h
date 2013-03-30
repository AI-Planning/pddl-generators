/*********************************************************************
 * Author: Hootan Nakhost
 *********************************************************************/

#ifndef SOLVER_H
#define SOLVER_H

#include "open_list.h"
#include "state.h"
#include "search_space.h"
#include "string.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>



using namespace std;



class Solver{
private:
	SearchSpace search_space;
	OpenList<State*> open_list;
    enum {FAILED, SOLVED, IN_PROGRESS};

public:
	Solver(){}
	void solve(vector<State>& states);
	int get_cost(State& state);
	int step();
	void dump();
	SearchNode fetch_next_node(bool& complete);

};

#endif
