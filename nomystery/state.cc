#include "state.h"
#include "globals.h"

bool State::operator==(const State& other){
	return (delivered == other.delivered) && (in_truck == other.in_truck) && (location == other.location);
}

void State::dump(){
	cout << "delivered:";
	g_dump(delivered);
	cout << " in_truck:";
	g_dump(in_truck);
	cout << " location:" << location;
}


size_t State::hash() const {
    // hash function adapted from Python's hash function for tuples.
    size_t hash_value = 0x345678;
    size_t mult = 1000003;

	hash_value = (hash_value ^ delivered) * mult;
	mult += 82520 + 4;

	hash_value = (hash_value ^ in_truck) * mult;
	mult += 82520 + 2;

	hash_value = (hash_value ^ location) * mult;
	mult += 82520;

    hash_value += 97531;
    return hash_value;
}

State::State(vector<int>& truck, vector<int>& dlv, int loc){
	location = loc;
	in_truck = 0;
	delivered = 0;
	for (int i = 0; i < truck.size(); ++i) {
		in_truck = in_truck | g_masks[truck[i]];
	}

	for (int i = 0; i < dlv.size(); ++i) {
		delivered = delivered | g_masks[dlv[i]];
	}
}

State::State(const State& curr, int loc){
	location = loc;
	delivered = curr.delivered | (g_loc_info[loc].goals & curr.in_truck);
	in_truck = (curr.in_truck & ~g_loc_info[loc].goals) | (g_loc_info[loc].initials & ~curr.delivered);
}


State::State(int loc){
	location = loc;
	delivered = 0;
	in_truck = g_loc_info[loc].initials;
}

