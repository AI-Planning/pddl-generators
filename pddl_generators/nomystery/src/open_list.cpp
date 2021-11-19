#include "open_list.h"

#include <cassert>
#include <iostream>

using namespace std;

void OpenList::skip_empty_buckets() {
    while (!entries.empty() && entries.begin()->second.empty()) {
        entries.erase(entries.begin());
    }
}

bool OpenList::empty() const {
    return entries.empty();
}

void OpenList::insert(StateID s_id, const State & , int g, int h) {
    entries[make_pair(g+h, h)].push_back(s_id);
}

StateID OpenList::pop_min() {
    skip_empty_buckets();
    assert(!entries.empty());

    int f = entries.begin()->first.first;
    if (f > best_f_value) {
        best_f_value = f;
        // cout << "New best f value: " << f << " (expanded: " << expanded
        //      << ", size of current bucket: " << entries.begin()->second.size()
        //      << ")" << endl;
    }
    ++expanded;

    //if (expanded % 1000 == 0) cout << "Expanded: " << expanded << " " << f << endl;
    
    auto &min_bucket = entries.begin()->second;
    assert(!min_bucket.empty());
    StateID s_id = min_bucket.front();
    min_bucket.pop_front();
    skip_empty_buckets();
    return s_id;
}
