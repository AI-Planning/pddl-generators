#include "globals.h"
#include "state.h"
#include "search.h"
#include "heuristic.h"
#include "utils.h"
#include "open_list_deals.h"

#include <iostream>
#include <fstream>  

using namespace std;

int main(int argc, char** argv) {
    read_everything(cin);

    int bound = 100000;
    
    if (argc >= 3) {
	bound = atoi(argv[2]);
    }
 
   if (argc >= 2) {
       int v = atoi(argv[1]);
       if (v > g_num_piles_heuristic) {
	   g_num_piles_heuristic = 0;
       } else {
	   g_num_piles_heuristic -= v;
       }
	
	
    }

    cout << g_num_decks << " decks" << endl
         << g_num_suits << " suits" << endl
         << g_num_values << " values" << endl;
    cout << "Deals:" << endl;
    for (const Pile &pile : g_deals) {
        for (Card c : pile) {
            c.dump();
            cout << " ";
        }
        cout << endl;
    }
    cout << "Initial state:" << endl;
    g_initial_state->dump();
    cout << endl;

    int best_cost = 100000000;
    SearchEngine<OpenListDeals, AdmissibleHeuristic> engine;
    Plan plan = engine.search(bound);

    if (plan.empty() && !g_initial_state->is_goal()) {
	cout << "no plan found" << endl;
	
    } else {
	cout << "plan found " << (plan.size() -1)<< endl;
	// int i = 0;
	// for (const State &state : plan) {
	//     cout << "Plan step " << i << ":" << endl;
	//     state.dump();
	//     cout << endl;
	//     ++i;
	// }
    
	if (argc >= 4) {
	    cout << "Writing plan to: " << argv[3] << endl;
	    std::ofstream outfile (argv[3]);

	    for (int i = 1; i< plan.size(); ++i) {
		print_action_name (plan[i-1], plan[i], *g_initial_state_piles, outfile);
	    }
	    outfile.close();
	} else {
	    for (int i = 1; i< plan.size(); ++i) {
		print_action_name (plan[i-1], plan[i], *g_initial_state_piles, std::cout);
	    }
	}

    }

    cout << "Peak memory: " << get_peak_memory_in_kb() << endl;

    return 0;
}
