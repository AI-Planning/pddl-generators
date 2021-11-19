#ifndef STATE_H
#define STATE_H

#include <cstdint>
#include <set>
#include <vector>
#include <map>
#include <iostream>

using StateID = int;

struct State {
    int truck_at;
    std::vector<bool> locations_to_load;
    std::vector<int> locations_to_deliver; //An int larger than 0 means that we still neeed to load a package going to that destination. -1 means that we are done.
    
public:
    State() = default;

    
State(int loc, const std::vector<bool> & l_load, const std::vector<int> & l_deliver) :
    truck_at(loc),  locations_to_load (l_load), locations_to_deliver(l_deliver) {
    }

    
    bool is_goal() const {
        for (int x : locations_to_deliver) {
            if (x >= 0) return false;
        }
        return true;
    }
    void dump() const;

    std::vector<std::pair<int, State>> get_successors() const;

    void print_action_name(const State &successor); 
    
    bool operator==(const State &other) const {
        return (truck_at == other.truck_at &&
                locations_to_load == locations_to_load &&
                locations_to_deliver == locations_to_deliver);
    }

    bool operator!=(const State &other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const State& c) ;
};


#endif
