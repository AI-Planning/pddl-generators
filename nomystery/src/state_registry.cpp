#include "state_registry.h"

#include "globals.h"

#include <algorithm>
#include <cassert>

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
    state.locations_to_load.reserve(num_locs);

    while (num_locs-- > 0) {
        state.locations_to_load.push_back((*data) == 1);
        ++data;
    }

    num_locs = *data;
    ++data;
    state.locations_to_deliver.reserve(num_locs);
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
      slots(1 << 10, -1),
      slot_hashes(1 << 10),
      num_states(0),
      buffer(state_size) {
}

State StateRegistry::lookup_state(int id) {
    PackedStateBin *data = state_data_pool[id];
    return packed_state_to_state(data);
}

void StateRegistry::insert_without_check(StateID id, size_t hash) {
    size_t mask = slots.size() - 1;
    size_t i = hash & mask;
    while (slots[i] != -1) {
        i = (i + 1) & mask;
    }
    slots[i] = id;
    slot_hashes[i] = hash;
}

void StateRegistry::grow() {
    vector<StateID> old_slots(slots.size() * 2, -1);
    vector<size_t> old_hashes(slot_hashes.size() * 2);
    old_slots.swap(slots);
    old_hashes.swap(slot_hashes);
    for (size_t i = 0; i < old_slots.size(); ++i) {
        if (old_slots[i] != -1) {
            insert_without_check(old_slots[i], old_hashes[i]);
        }
    }
}

int StateRegistry::get_state_id(const State &state) {
    state_to_packed_state(state, buffer.data());
    size_t hash = compute_hash(buffer.data());
    size_t mask = slots.size() - 1;
    for (size_t i = hash & mask; slots[i] != -1; i = (i + 1) & mask) {
        if (slot_hashes[i] == hash) {
            const PackedStateBin *data = state_data_pool[slots[i]];
            if (equal(data, data + state_size, buffer.data())) {
                return slots[i];
            }
        }
    }
    // push_back copies the data.
    state_data_pool.push_back(buffer.data());
    StateID id = state_data_pool.size() - 1;
    ++num_states;
    if (2 * num_states > static_cast<int>(slots.size())) {
        grow();
    }
    insert_without_check(id, hash);
    assert(num_states == static_cast<int>(state_data_pool.size()));
    return id;
}
