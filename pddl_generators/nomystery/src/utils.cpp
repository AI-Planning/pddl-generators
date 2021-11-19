#include "utils.h"

#include <fstream>
#include <iostream>
#include <limits>

using namespace std;

int get_peak_memory_in_kb() {
    // On error, produces a warning on cerr and returns -1.
    int memory_in_kb = -1;
    ifstream procfile;
    procfile.open("/proc/self/status");
    string word;
    while (procfile.good()) {
        procfile >> word;
        if (word == "VmPeak:") {
            procfile >> memory_in_kb;
            break;
        }
        // Skip to end of line.
        procfile.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    if (procfile.fail())
        cerr << "warning: could not determine peak memory" << endl;
    return memory_in_kb;
}
