#include "state.h"

#include "globals.h"

#include <iostream>
#include <cassert>

using namespace std;


std::vector<std::pair<int, State>> State::get_successors() const {
    std::vector<std::pair<int, State>> res;
    res.reserve(locations_to_load.size() + locations_to_deliver.size());
    
    for (int loc = 0; loc < locations_to_load.size(); loc++) {
        if (!locations_to_load[loc]) continue;
        
        vector<bool> new_locations_to_load = locations_to_load;
        vector<int> new_locations_to_deliver = locations_to_deliver;

        if (locations_to_deliver[loc] == 0) {
            new_locations_to_deliver[loc]--;
        }
        new_locations_to_load[loc] = false;

        
        for (int goal : g_goals_per_initial[loc]) {
            assert(new_locations_to_deliver[goal] > 0);
            new_locations_to_deliver[goal] --;
        }

        res.push_back(make_pair(g_distances[truck_at][loc],
                                State(loc, new_locations_to_load, new_locations_to_deliver)));
    }

    for (int loc = 0; loc < locations_to_deliver.size(); loc++) {
        if (locations_to_deliver[loc] == 0 && !locations_to_load[loc]) {
            auto new_locations_to_deliver = locations_to_deliver;
            new_locations_to_deliver[loc]--;
                
            res.push_back(make_pair(g_distances[truck_at][loc],
                                    State(loc, locations_to_load, new_locations_to_deliver)));
            
        }
    }
    // cout << "Expanding " << *this << endl;
    // for (auto & x : res) {
    //     cout << "     "<<x.first << ": " << x.second << endl;
    // }
    // cout << endl;
    return res;    
}




ostream& operator<<(ostream& os, const State& state)  {
    os << "At: " << state.truck_at << " load: ";
    for (int loc = 0; loc < state.locations_to_load.size(); loc++) {
        if (state.locations_to_load[loc]) {
            os << loc << " " ;
        }
    }

    os << " deliver: "; 
    for (int loc = 0; loc < state.locations_to_deliver.size(); loc++) {
        if (state.locations_to_deliver[loc] >= 0) {
            os << loc << " (" << state.locations_to_deliver[loc] << ") ";
        }
    }

    return os;
}  
