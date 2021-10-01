#ifndef __VARIABLES__
#define __VARIABLES__

#include "biochem.h"

global settings;
problem_desc problem;
token_list *lines = NULL;
reaction_list *reactions = NULL;
results_list *results = NULL;
pref_struct preferences;
int used_pref[MAX_PREFERENCES];
char *symb;
int extra_weight;
char String[MAX_LENGHT][MAX_NUM_GOAL];

#endif
