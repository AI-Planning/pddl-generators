#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <vector>
using namespace std;

class State{

public:
	int delivered;
	int in_truck;
	int location;

	State(int delivered_, int in_truck_, int location_) : delivered(delivered_), in_truck(in_truck_), location(location_){}
	State(vector<int>& truck, vector<int>& delivered, int loc);
	State(const State& curr, int loc);
	State(int loc);
	State(){}

	bool operator==(const State& other);
	void dump();
	size_t hash() const;
};

#endif
