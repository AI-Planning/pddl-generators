#ifndef OPEN_LIST_H
#define OPEN_LIST_H

#include "state.h"

#include <deque>
#include <map>

class OpenList {
    std::map<std::pair<int, int>, std::deque<StateID>> entries;
    void skip_empty_buckets();

    int best_f_value;
    int expanded;
public:
    OpenList() : best_f_value(0), expanded(0) {
    }

    void insert(StateID s_id, const State & state, int g, int h);
    StateID pop_min();

    bool empty() const;
};

#endif
