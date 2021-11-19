#ifndef STATE_REGISTRY_H
#define STATE_REGISTRY_H

#include "segmented_vector.h"
#include "state.h"

#include <deque>
#include <unordered_set>

using PackedStateBin = uint16_t;
using PackedState = std::vector<PackedStateBin>;

class StateRegistry {
    struct StateIDSemanticHash {
        const segmented_vector::SegmentedArrayVector<PackedStateBin> &state_data_pool;
        int state_size;
        StateIDSemanticHash(
            const segmented_vector::SegmentedArrayVector<PackedStateBin> &state_data_pool,
            int state_size)
            : state_data_pool(state_data_pool),
              state_size(state_size) {
        }

        size_t operator()(StateID id) const {
            const PackedStateBin *data = state_data_pool[id];
            std::size_t h = 0;
            for (int i = 0; i < state_size; ++i) {
                h ^= data[i] + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };

    struct StateIDSemanticEqual {
        const segmented_vector::SegmentedArrayVector<PackedStateBin> &state_data_pool;
        int state_size;
        StateIDSemanticEqual(
            const segmented_vector::SegmentedArrayVector<PackedStateBin> &state_data_pool,
            int state_size)
            : state_data_pool(state_data_pool),
              state_size(state_size) {
        }

        bool operator()(StateID lhs, StateID rhs) const {
            const PackedStateBin *lhs_data = state_data_pool[lhs];
            const PackedStateBin *rhs_data = state_data_pool[rhs];
            return std::equal(lhs_data, lhs_data + state_size, rhs_data);
        }
    };

    using StateIDSet = std::unordered_set<StateID, StateIDSemanticHash, StateIDSemanticEqual>;

    int state_size;
    segmented_vector::SegmentedArrayVector<PackedStateBin> state_data_pool;
    StateIDSet registered_states;

public:
    StateRegistry();

    State lookup_state(StateID id);
    StateID get_state_id(const State &state);

    int size() {
        return registered_states.size();
    }
};


#endif
