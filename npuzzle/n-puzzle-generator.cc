
// PDDL problem generator for the N-puzzle
// Neville Mehta


#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <vector>
#include <string.h>
using namespace std;


// Number of inversions ignoring zero (blank)
int inversion_counter (const vector<int>& permutation)
{	int num_inversions = 0;
	int n = permutation.size();

	for (int i = 0; i < n - 1; i++)
		for (int j = i + 1; j < n; j++)
			if (permutation[i] != 0 && permutation[j] != 0 && permutation[i] > permutation[j])
				num_inversions++;

	return num_inversions;
}

void usage()
{

  printf("\nusage:\n");

  printf("\nOPTIONS   DESCRIPTIONS\n\n");
  printf("-n <num>    rows / columns (minimal 1)\n");

}

int main (int argc, char* argv[])
{	int board_size = 0;
	srand((unsigned)time(0));
	string typed_name, untyped_name;
    	ofstream typed_file, untyped_file;

  
	// Input
	if (argc >= 3)
	{	istringstream str(argv[2]);
	    	str >> board_size;
		//typed_name = argv[2];
		//typed_file.open(typed_name);
		//typed_file << "Hello world.\n";
		//typed_file.close();
	}
	if (board_size < 2 || strcmp(argv[1], "-n") != 0)
	{	cerr << " Command-line: " << argv[0] << " -n " << "<number >= 2>\n";
		exit(1);
	}

	int num_tiles = board_size * board_size - 1;


	//------- Goal configuration -------//
	vector<int> goal_board(num_tiles + 1);
	for (int i = 0; i < board_size; i++)
		for (int j = 0; j < board_size; j++)
			goal_board[i * board_size + j] = i % 2 == 0 ? i * board_size + j + 1 : i * board_size + (board_size - j);   // Boustrophedon ordering

	if (board_size % 2)
		goal_board[num_tiles] = 0;
	else
		goal_board[num_tiles - board_size + 1] = 0;
	int num_inversions_goal = inversion_counter(goal_board);


	//------- Initial configuration within the same parity/orbit as the goal -------//
	vector<int> init_board(goal_board);
	do
		random_shuffle(init_board.begin(), init_board.end());
	while (inversion_counter(init_board) % 2 != num_inversions_goal % 2);


	//------- PDDL output -------//
	cout << "(define (problem n-puzzle1)\n  (:domain n-puzzle-typed)\n  (:objects";
	for (int i = 1; i <= board_size; i++)
		for (int j = 1; j <= board_size; j++)
			cout << " p_" << i << "_" << j;
	cout << " - position";
	for (int i = 1; i <= num_tiles; i++)
		cout << " t_" << i;
	cout << " - tile";

	cout << ")\n  (:init";
	for (int i = 0; i < board_size; i++)
	{	for (int j = 0; j < board_size; j++)
		{	int index = i % 2 == 0 ? i * board_size + j : i * board_size + (board_size - j) - 1;
			if (init_board[index] != 0)
				cout << "\n    (at t_" << init_board[index] << " p_" << i + 1 << "_" << j + 1 << ")";
			else
				cout << "\n    (empty p_" << i + 1 << "_" << j + 1 << ")";
		}
	}

	// Horizontal neighbors
	for (int i = 1; i <= board_size; i++)
		for (int j = 1; j < board_size; j++)
			cout << "\n    (neighbor p_" << i << "_" << j << " p_" << i << "_" << j + 1 << ")\n    (neighbor p_" << i << "_" << j + 1 << " p_" << i << "_" << j << ")";

	// Vertical neighbors
	for (int i = 1; i < board_size; i++)
		for (int j = 1; j <= board_size; j++)
			cout << "\n    (neighbor p_" << i << "_" << j << " p_" << i + 1 << "_" << j << ")\n    (neighbor p_" << i + 1 << "_" << j << " p_" << i << "_" << j << ")";

	cout << ")\n  (:goal (and";
	for (int i = 0; i < board_size; i++)
	{	for (int j = 0; j < board_size; j++)
		{	int index = i % 2 == 0 ? i * board_size + j : i * board_size + (board_size - j) - 1;
			if (goal_board[index] != 0)
				cout << "\n    (at t_" << goal_board[index] << " p_" << i + 1 << "_" << j + 1 << ")";
		}
	}
	cout << ")))\n";

	return 0;
}
