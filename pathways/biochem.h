#ifndef __BIOCHEM__
#define __BIOCHEM__

#include <stdio.h>

#define MAX_LENGHT 256
#define MAX_NUM_GOAL 128

#define SAME 0
#define TRUE 1
#define FALSE 0

#define PLUS_TO_DASH      0
#define DASH_TO_PLUS      1
#define PLUS_TO_PLUS_CAT  2
#define PLUS_CAT          3

#define DUMMY_ACTION_DURATION 0.1

#define MAX_LINE_SIZE 100

#define SEED_OPT       "--seed"
#define OBJECTS_OPT    "-o"
#define REACTIONS_OPT  "-r"
#define PROBLEM_OPT    "-n"
#define OUT_OPT        "-out"
#define NUM_OBJ_OPT    "-O"
#define NUM_ACT_OPT    "-R"
#define HELP_OPT       "-h"
#define VERBOSE_OPT    "-v"
#define GOAL_P_OPT     "-p"
#define GOAL_Q_OPT     "-q"
#define COUNTSUBS_OPT  "--countsubs"
#define SKIP_EMPTY_OPT "--skip-empty"
#define DOWNWARD_OPT   "-dw"
#define NUM_GOAL_OPT   "-G"
#define NUMERIC_OPT    "-N"
#define LEVELS_OPT     "-L"
#define TOTAL_OPT      "--total"
#define TOTAL_USED_OPT "--total_used"
#define DJ_OPT         "-dj"
#define DURATIVE_OPT   "--durative"
#define RND_CONSTANTS  "-C"
#define MAXSUBS        "--max_subs"
#define CONSTR_PREF_OPT "--constraints-pref"
#define SIMPLE_PREFERENCE "--simple-pref"
#define TOTAL_TIME "--total-time"

#define FULLPROB       100
#define INITIAL_PROB_P  40
#define INITIAL_PROB_Q  20

#define COMMENT_CHAR   '%'

#define STR_EMPTY      "_"

#define DEFAULT_PROBLEM_NAME "PathwaysProb"
#define DEFAULT_PROBLEM_FILE "pfile"

#define DECODE_INDEX(x) ((x >= 0) ? (x) : (-x - 1))
#define GET_SUBSTANCE(x) ((x >= 0) ? settings.simple_substances[x] : settings.dashed_substances[-x - 1])

#define GET_BIT(bit_vect, nbit) (!(!(bit_vect[nbit >> 5] & (1 << (nbit & 31)))))
#define SET_BIT(bit_vect, nbit) (bit_vect[nbit >> 5] |= (1 << (nbit & 31)))
#define RESET_BIT(bit_vect, nbit) (bit_vect[nbit >> 5] &= ~(1 << (nbit & 31)))

#define MAX(x, y) ((x > y) ? x : y)

#define MAX_PREFERENCES 100

#define PREF_START 'A'
#define PREF_BASE ('Z' - 'A' + 1)

typedef char *string;
typedef unsigned short Bool;

typedef struct _token_list {
    string item;
    struct _token_list *next;
} token_list;

typedef struct _reaction_desc {
    int s1, s2, s3;
    unsigned short type;
} reaction_desc;

typedef struct _reaction_list {
    reaction_desc reaction;
    struct _reaction_list *next;
} reaction_list;

typedef struct _results_list {
    int *dashed;
    int num_dashed;
    struct _results_list *next;
} results_list;


typedef struct _global {
    FILE *objects_file;
    FILE *reactions_file;
    FILE *problem_file;
    FILE *dw_problem_file;

    string prob_name;

    string *simple_substances;
    int num_of_simple_substances;

    string *dashed_substances;
    int num_of_dashed_substances;

    reaction_desc *reactions;
    int num_of_reactions;

    int max_init;
    int max_act;
    int orig_max_act;

    int num_goals;

    int p;
    int q;

    Bool skip_empty;

    Bool countsubs;
    Bool quantity_levels;

    Bool dw_out;

    Bool verbose;

    Bool numeric;

    Bool total;

    Bool total_used;

    int levels_to_use;

    int seed;

    Bool dj_goals;

    Bool durative;

    float total_value;

    Bool rnd_constants;

    Bool constraints_pref;

    Bool simple_preference;

    float totaltime;
} global;


typedef struct _problem_desc {
    int *initial_simple_available;
    int *simple_available;
    int block_simple_available;

    int *to_apply;
    int *applied;
    int block_reactions;

    int *dashed_available;
    int block_dashed_available;

    int *simple_used;
    int *dashed_used;
    int *dashed_already_used;
    int *simple_already_used;

    int *reaction_already_used;

    int *initial_substances;
    int num_of_initial_substances;

    int *goal_substances;
    int num_of_goal_substances;

    int *last_dashed;
    int num_last_dashed;

    int *simple_levels;
    int *dashed_levels;

    int max_level;
    int max_sub_level;

    int num_simple_used;

    int num_act;
} problem_desc;



typedef struct _pref_struct {
    int weight[MAX_PREFERENCES];
    int count[MAX_PREFERENCES];
} pref_struct;


#endif
