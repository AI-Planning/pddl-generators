#ifndef SUCCESSOR_GENERATOR_H
#define SUCCESSOR_GENERATOR_H

#include <iostream>
#include <vector>
#include "state.h"

using namespace std;


class SuccessorGenerator {
public:
    void generate_successors(const State &curr, vector<State> & succs, vector<int>& costs);
    void get_neighbor_costs(int curr_loc, vector<pair<int, int> >& neighbors);
};

#endif
