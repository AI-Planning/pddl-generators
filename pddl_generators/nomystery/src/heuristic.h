#ifndef HEURISTIC_H
#define HEURISTIC_H

class State;

class AdmissibleHeuristic {
public:
    int compute(const State &state);
};

#endif
