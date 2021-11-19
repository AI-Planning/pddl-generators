#include "globals.h"
#include "state.h"
#include "search.h"
#include "heuristic.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

int main(int argc, char * argv[]) {
	g_num_trucks = 1;

	/* gn will stay fixed: I don't expect to go up to more than 10
	* locations; of 45 possible edges, we'll have 15 = a 3rd then,
	* which seems generous for a roadmap. at -l < 10, it'll be even
	* more than a 3rd.
	*/
	g_num_locations = 6;
	g_num_packages = 6;
	g_n = 1.5;
	g_m = 25;
	g_c = 1;
	g_seed = 1;
	g_encoding = 0;

	if ( !process_command_line( argc, argv ) ) {
		usage();
		exit( 1 );
	}


	srandom(g_seed);
	create_random_graph();
	select_initials_goals();

	time_t current_time = time(NULL);

        vector<double> w_list = {1000, 100, 10, 5, 2, 1.5, 1.2, 1.0};
        int initial_fuel = std::numeric_limits<int>::max();
        for (double w : w_list) {
            time_t elapsed_time = time(NULL) - current_time;
            if (elapsed_time > 60) {
                break;
            }
            SearchEngine<OpenList, AdmissibleHeuristic> engine;
            if (initial_fuel == std::numeric_limits<int>::max()) {
                initial_fuel = engine.search(initial_fuel, w);
            } else {
                initial_fuel = engine.search(initial_fuel-1, w, 60 - elapsed_time);
            }
            //cout << elapsed_time << " "<<  w << " " << initial_fuel << endl;
        }
        //cout << initial_fuel << endl;
	output_pddl_file(initial_fuel * g_c);
	return 0;
}


