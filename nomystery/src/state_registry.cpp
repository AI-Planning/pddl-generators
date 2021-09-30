#include "state_registry.h"

#include "globals.h"

using namespace std;

void state_to_packed_state(const State &state, PackedStateBin *data) {
    *data = state.truck_at;
    ++data;
    *data = state.locations_to_load.size();
    ++data;
    for (bool l : state.locations_to_load) {
        *data = l ? 1 : 0;
        ++data;
    }
    *data = state.locations_to_deliver.size();
    ++data;
    for (int l : state.locations_to_deliver) {
        *data = l;
        ++data;
    }
}

State packed_state_to_state(const PackedStateBin *data) {
    State state;
    state.truck_at = *data;
    ++data;

    int num_locs = *data;
    ++data;

    while (num_locs-- > 0) {
        state.locations_to_load.push_back((*data) == 1);
        ++data;
    }

    num_locs = *data;
    ++data;
    while (num_locs-- > 0) {
        int l = *data;
        if (l == 65535) l = -1;
        ++data;
        state.locations_to_deliver.push_back(l);
    }

        
    return state;
}

int compute_state_size() {
    return 3 + 2*g_num_locations;
}

StateRegistry::StateRegistry()
    : state_size(compute_state_size()),
      state_data_pool(state_size),
      registered_states(0,
                        StateIDSemanticHash(state_data_pool, state_size),
                        StateIDSemanticEqual(state_data_pool, state_size)) {
}

State StateRegistry::lookup_state(int id) {
    PackedStateBin *data = state_data_pool[id];
    return packed_state_to_state(data);
}

int StateRegistry::get_state_id(const State &state) {
    PackedStateBin *data = new PackedStateBin[state_size];
    state_to_packed_state(state, data);
    state_data_pool.push_back(data);

    StateID id = state_data_pool.size() - 1;
    pair<StateIDSet::iterator, bool> result = registered_states.insert(id);
    bool is_new_entry = result.second;
    if (!is_new_entry) {
        state_data_pool.pop_back();
    }
    assert(registered_states.size() == state_data_pool.size());
    return *result.first;
}
