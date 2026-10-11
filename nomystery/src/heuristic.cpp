#include "heuristic.h"

#include "globals.h"
#include "state.h"

#include <cassert>
#include <vector>
#include <unordered_map>
#include <limits>

using namespace std;

int AdmissibleHeuristic::compute(const State &state) {
    // Reuse the buffers to avoid allocations.
    static std::vector<int> active_locs;
    static std::vector<int> minimum_distance;
    active_locs.clear();
    minimum_distance.clear();

    int minimo = std::numeric_limits<int>::max();
    int min_index = 0;
    
    const vector<int> &truck_distances = g_distances[state.truck_at];
    for (int loc = 0; loc < state.locations_to_load.size(); ++loc) {
        if (state.locations_to_load [loc] || state.locations_to_deliver[loc] >= 0) {
            active_locs.push_back(loc);
            minimum_distance.push_back(truck_distances[loc]);
            if (minimum_distance.back() < minimo) {
                minimo = minimum_distance.back();
                min_index = minimum_distance.size() - 1;
            }            
        }
    }

    int h = 0;
    while (!active_locs.empty()) {
        int new_loc = active_locs[min_index];
        h += minimum_distance[min_index];
        
        //Replace min_index by the last element and pop out
        active_locs[min_index] = active_locs.back();
        minimum_distance[min_index] = minimum_distance.back();
        active_locs.pop_back();
        minimum_distance.pop_back();

        if (!active_locs.empty()){

            //Recompute min_distances
            minimo = std::numeric_limits<int>::max();
            min_index = 0;
            const vector<int> &new_loc_distances = g_distances[new_loc];
            for (int i = 0; i < active_locs.size(); ++i) {
                minimum_distance[i] = min(minimum_distance[i], new_loc_distances[active_locs[i]]);
                if (minimum_distance[i] < minimo) {
                    minimo = minimum_distance[i];
                    min_index = i;
                }           
            }
        }
    }

    return h;
}

