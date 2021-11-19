#include "heuristic.h"

#include "globals.h"
#include "state.h"

#include <cassert>
#include <vector>
#include <unordered_map>
#include <limits>

using namespace std;

int AdmissibleHeuristic::compute(const State &state) {
    std::vector<int> active_locs;
    std::vector<int> minimum_distance;

    int minimo = std::numeric_limits<int>::max();
    int min_index = 0;
    
    for (int loc = 0; loc < state.locations_to_load.size(); ++loc) {
        if (state.locations_to_load [loc] || state.locations_to_deliver[loc] >= 0) {
            active_locs.push_back(loc);
            minimum_distance.push_back(g_distances[state.truck_at] [loc]);
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
            for (int i = 0; i < active_locs.size(); ++i) {
                minimum_distance[i] = min(minimum_distance[i], g_distances[new_loc][active_locs[i]]);
                if (minimum_distance[i] < minimo) {
                    minimo = minimum_distance[i];
                    min_index = i;
                }           
            }
        }
    }

    return h;
}

