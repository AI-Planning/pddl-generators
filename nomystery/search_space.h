#ifndef SEARCH_SPACE_H
#define SEARCH_SPACE_H

#include "state.h"
#include <vector>

class SearchNodeInfo {
    friend class SearchNode;
    friend class SearchSpace;

    enum NodeStatus {NEW = 0, OPEN = 1, CLOSED = 2, DEAD_END = 3};


    unsigned int status: 2;
    int h: 30;
public:
    SearchNodeInfo()
	: status(NEW), h(-1) {

    }

};

class SearchNode {
    State *state_buffer;
    SearchNodeInfo& info;
public:
    SearchNode(State *state_buffer_, SearchNodeInfo& info_);
    State *get_state_buffer() {
      return state_buffer;
    }
    State get_state() const;

    bool is_goal() const;
    bool is_open() const;
    bool is_closed() const;
    bool is_dead_end() const;

    int get_f() const;
    int get_g() const;
    int get_h() const;

    void open_initial(int h);
    void open(int h);
    void reopen(int h);
    void close();
    void mark_as_dead_end();

    void dump();
};


class SearchSpace {
    class HashTable;
    HashTable *nodes;
public:
    SearchSpace();
    ~SearchSpace();
    int size() const;
    SearchNode get_node(const State* state);

    void dump();
    void statistics() const;
};

#endif
