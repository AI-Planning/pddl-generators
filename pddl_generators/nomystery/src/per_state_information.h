#ifndef PER_STATE_INFORMATION_H
#define PER_STATE_INFORMATION_H

#include "segmented_vector.h"
#include "state_registry.h"

#include <cassert>
#include <unordered_map>

template<class Entry>
class PerStateInformation {
    const Entry default_value;
    segmented_vector::SegmentedVector<Entry> entries;
public:
    PerStateInformation()
        : default_value() {
    }

    explicit PerStateInformation(const Entry &default_value_)
        : default_value(default_value_) {
    }

    Entry &operator[](int state_id) {
        size_t virtual_size = state_id + 1;
        if (entries.size() < virtual_size) {
            entries.resize(virtual_size, default_value);
        }
        return entries[state_id];
    }

    const Entry &operator[](int state_id) const {
        int num_entries = entries.size();
        if (state_id >= num_entries) {
            return default_value;
        }
        return entries[state_id];
    }

    bool contains(int state_id) const {
        return state_id < entries.size();
    }
};

#endif
