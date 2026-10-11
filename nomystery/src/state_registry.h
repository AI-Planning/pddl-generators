#ifndef STATE_REGISTRY_H
#define STATE_REGISTRY_H

#include "segmented_vector.h"
#include "state.h"

#include <cstddef>
#include <vector>

using PackedStateBin = uint16_t;
using PackedState = std::vector<PackedStateBin>;

class StateRegistry {
    int state_size;
    segmented_vector::SegmentedArrayVector<PackedStateBin> state_data_pool;
    // Open-addressing hash set of state IDs with linear probing. slots holds
    // the IDs (-1 for empty slots) and slot_hashes their hash values.
    std::vector<StateID> slots;
    std::vector<std::size_t> slot_hashes;
    int num_states;
    // Buffer for packing states, reused to avoid an allocation per state.
    PackedState buffer;

    std::size_t compute_hash(const PackedStateBin *data) const {
        std::size_t h = 0;
        for (int i = 0; i < state_size; ++i) {
            h ^= data[i] + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }

    void grow();
    void insert_without_check(StateID id, std::size_t hash);

public:
    StateRegistry();

    State lookup_state(StateID id);
    StateID get_state_id(const State &state);

    int size() {
        return num_states;
    }
};


#endif
