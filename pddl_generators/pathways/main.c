#include "pddldef.h"
#include "biochem.h"
#include "variables.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

int num_goals;
float uno, cinque, dieci, coeff;


int is_a_goal_substance(int g) {
    int i;

    for (i = 0; i < problem.num_last_dashed; i++)
        if (DECODE_INDEX(g) == DECODE_INDEX(problem.last_dashed[i]))
            return 1;

    return 0;
}

int num_rand() {
    return (random() % 4) + 1;
}

float fnum_rand() {
    float f;

    f = 0.5 + 3.5 * ((float)(rand() % 100) / 100);

    return f;
}

void usage() {
    printf("Pathways-Propositional Problem file generator");
    printf("\nUsage: pathways [options]");
    printf("\n\nCommand line options:");
    printf("\n %s <file>\tSet the problem file name", OUT_OPT, DEFAULT_PROBLEM_FILE);
    printf("\n %s <string>\tSet the problem name (default %s)", PROBLEM_OPT, DEFAULT_PROBLEM_NAME);
    printf("\n %s <num>\tSet the min number of reactions used in the reach. analysis", NUM_ACT_OPT);
    printf("\n %s <num>\tSet the number of goals", NUM_GOAL_OPT);
    printf("\n %s <num>\tSet the desired number of initial substances (in metric field)", LEVELS_OPT);
    printf("\n%s <num>\tSet the seed for the random number generator", SEED_OPT);
    /*
    printf("\n%s <file> \t: file of simple substances", OBJECTS_OPT);
    printf("\n%s <file> \t: file of reactions", REACTIONS_OPT);
    printf("\n%s name   \t: problem name (default %s)", PROBLEM_OPT, DEFAULT_PROBLEM_NAME);
    printf("\n%s #      \t: number of substances in initial state", NUM_OBJ_OPT);
    printf("\n%s #      \t: min number of reactions to use", NUM_ACT_OPT);
    printf("\n%s #      \t: probability to choose a goal from last level", GOAL_P_OPT);
    printf("\n%s #      \t: probability to choose a goal from last-1 level", GOAL_Q_OPT);
    printf("\n%s\t: skip reactions with empty substance", SKIP_EMPTY_OPT);
    printf("\n%s\t: use countsubs constraint", COUNTSUBS_OPT);
    printf("\n%s        \t: save problem with downward typing", DOWNWARD_OPT);
    printf("\n%s        \t: number of goals", NUM_GOAL_OPT);
    printf("\n%s        \t: disjunctive goals", DJ_OPT);
    printf("\n%s        \t: numeric problem", NUMERIC_OPT);
    printf("\n%s\t: durative domain", DURATIVE_OPT);
    printf("\n%s        \t: number of levels", LEVELS_OPT);
    printf("\n%s        \t: use \"total\" counter", TOTAL_OPT);
    printf("\n%s\t: use \"total-used\" counter", TOTAL_USED_OPT);
    printf("\n%s        \t: verbose", VERBOSE_OPT);
    printf("\n%s        \t: print this help", HELP_OPT);
    printf("\n%s\t: generate constraints with preferences", CONSTR_PREF_OPT);
    */
    printf("\n\n");
    exit(0);
}

void comment(FILE *f) {
    fprintf(f, ";PROBLEM %s", settings.prob_name);
    fprintf(f, "\n;Max number of substances in initial state = %d (USED = %d)",
            settings.max_init, problem.num_simple_used);
    fprintf(f, "\n;Min number of reaction to use = %d (USED = %d)",
            settings.orig_max_act, problem.num_act);
    fprintf(f, "\n;Number of goals = %d",
            settings.num_goals);
    fprintf(f, "\n;Skip reactions with empty substance = %s",
            (settings.skip_empty) ? "ON" : "OFF");
    fprintf(f, "\n;Countsubs constraint = %s",
            (settings.countsubs) ? "ON" : "OFF");
    if (settings.numeric)
        fprintf(f, "\n;NUMERIC");
    fprintf(f, "\n\n");
}

void print_all() {
    int i;

    printf("\n\nPARSED SIMPLE SUBSTANCES [%d]:", settings.num_of_simple_substances);
    for (i = 0; i < settings.num_of_simple_substances; i++) {
        printf("\n\t\t%s", settings.simple_substances[i]);
    }

    printf("\n\nPARSED DASHED SUBSTANCES [%d]:", settings.num_of_dashed_substances);
    for (i = 0; i < settings.num_of_dashed_substances; i++) {
        printf("\n\t\t%s", settings.dashed_substances[i]);
    }

    printf("\n\nPARSED REACTIONS [%d]:", settings.num_of_reactions);
    for (i = 0; i < settings.num_of_reactions; i++) {
        switch (settings.reactions[i].type) {
        case PLUS_TO_DASH:
            printf("\n\tPLUS_TO_DASH");
            break;
        case DASH_TO_PLUS:
            printf("\n\tDASH_TO_PLUS");
            break;
        case PLUS_TO_PLUS_CAT:
            printf("\n\tPLUS_TO_PLUS_CAT");
            break;
        case PLUS_CAT:
            printf("\n\tPLUS_CAT");
            break;
        default:
            printf("\nERROR IN REACTION TYPE!\n");
            break;
        }

        printf("\n\t\tS1: %s -- S2: %s -- S3: %s", GET_SUBSTANCE(settings.reactions[i].s1),
               GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3));
    }

    printf("\n\n");
}


string newString(int size) {
    string s = NULL;
    s = (string)calloc(size, sizeof(char));

    return s;
}

FILE *open_file(string name, string mode) {
    FILE *f;
    f = fopen(name, mode);
    if (f == NULL) {
        fprintf(stderr, "ERROR opening file %s\n", name);
        exit(1);
    }
    return f;
}


void parse_command_line(int argc, char *argv[]) {
    // Change into the directory that contains the executable.
    char *dir;
    dir = strdup(argv[0]);
    dirname(dir);
    chdir(dir);

    int i;
    int mandatory = 1;
    char *out_name = NULL, *dw_out_name = NULL;

    out_name = newString(strlen(DEFAULT_PROBLEM_FILE) + 1);
    sprintf(out_name, "%s", DEFAULT_PROBLEM_FILE);
    settings.problem_file = NULL;
    settings.dw_problem_file = NULL;
    settings.max_init = settings.max_act = -1;
    settings.verbose = FALSE;
    settings.prob_name = DEFAULT_PROBLEM_NAME;
    settings.p = INITIAL_PROB_P;
    settings.q = INITIAL_PROB_Q;
    settings.skip_empty = FALSE;
    settings.countsubs = FALSE;
    settings.quantity_levels = FALSE;
    problem.max_level = 0;
    problem.max_sub_level = 0;
    settings.dw_out = FALSE;
    settings.num_goals = -1;
    settings.numeric = FALSE;
    settings.seed = -1;
    settings.levels_to_use = -1;
    settings.total = settings.total_used = FALSE;
    settings.dj_goals = FALSE;
    settings.durative = FALSE;
    settings.total_value = 1000;
    settings.rnd_constants = TRUE;
    settings.constraints_pref = FALSE;
    settings.simple_preference = FALSE;
    settings.totaltime = 0.0;


    settings.objects_file = open_file("Pathways-SimpleSubs", "r");
    settings.reactions_file = open_file("Pathways-Reactions", "r");
    settings.dj_goals = TRUE;
    settings.countsubs = TRUE;


    for (i = 0; i < MAX_PREFERENCES; i++) {
        preferences.weight[i] = preferences.count[i] = 0;
    }

    for (i = 0; i < argc; i++) {
        /*
        if (strcmp(argv[i], TOTAL_TIME) == SAME)
      {
        settings.totaltime = atof(argv[++i]);
        continue;
      }
        if (strcmp(argv[i], SIMPLE_PREFERENCE) == SAME)
      {
        settings.simple_preference = TRUE;
        continue;
      }
        if (strcmp(argv[i], CONSTR_PREF_OPT) == SAME)
      {
        settings.constraints_pref = TRUE;
        continue;
      }
        if (strcmp(argv[i], DURATIVE_OPT) == SAME)
      {
        settings.durative = TRUE;
        continue;
      }
        if (strcmp(argv[i], RND_CONSTANTS) == SAME)
      {
        settings.rnd_constants = FALSE;
        continue;
      }
        if (strcmp(argv[i], DJ_OPT) == SAME)
      {
        settings.dj_goals = TRUE;
        continue;
      }
        */
        if (strcmp(argv[i], SEED_OPT) == SAME) {
            settings.seed = atoi(argv[++i]);
            continue;
        }

        if (strcmp(argv[i], HELP_OPT) == SAME) {
            usage();
        }
        /*
        if (strcmp(argv[i], OBJECTS_OPT) == SAME)
      {
        settings.objects_file = open_file(argv[++i], "r");
        mandatory--;
        continue;
      }
        if (strcmp(argv[i], REACTIONS_OPT) == SAME)
      {
        settings.reactions_file = open_file(argv[++i], "r");
        mandatory--;
        continue;
      }
        */

        if (strcmp(argv[i], PROBLEM_OPT) == SAME) {
            i++;
            mandatory--;
            settings.prob_name = newString(strlen(argv[i]) + 1);
            sprintf(settings.prob_name, "%s", argv[i]);
            continue;
        }

        if (strcmp(argv[i], OUT_OPT) == SAME) {
            i++;
            out_name = newString(strlen(argv[i]) + 1);
            sprintf(out_name, "%s", argv[i]);
            continue;
        }
        /*
        if (strcmp(argv[i], NUM_OBJ_OPT) == SAME)
      {
        settings.max_init = atoi(argv[++i]);
        continue;
      }
        */
        if (strcmp(argv[i], NUM_ACT_OPT) == SAME) {
            settings.orig_max_act = settings.max_act = atoi(argv[++i]);
            continue;
        }
        /*
        if (strcmp(argv[i], GOAL_P_OPT) == SAME)
      {
        settings.p = atoi(argv[++i]);
        continue;
      }
        if (strcmp(argv[i], GOAL_Q_OPT) == SAME)
      {
        settings.q = atoi(argv[++i]);
        continue;
      }
        if (strcmp(argv[i], SKIP_EMPTY_OPT) == SAME)
      {
        settings.skip_empty = TRUE;
        continue;
      }
        if (strcmp(argv[i], COUNTSUBS_OPT) == SAME)
      {
        settings.countsubs = TRUE;
        continue;
      }
        if (strcmp(argv[i], DOWNWARD_OPT) == SAME)
      {
        settings.dw_out = TRUE;
        continue;
      }
        */
        if (strcmp(argv[i], NUM_GOAL_OPT) == SAME) {
            settings.num_goals = atoi(argv[++i]);
            continue;
        }
        /*
        if (strcmp(argv[i], VERBOSE_OPT) == SAME)
      {
        settings.verbose = TRUE;
        continue;
      }
        if (strcmp(argv[i], NUMERIC_OPT) == SAME)
      {
        settings.numeric = TRUE;
        continue;
      }
        */
        if (strcmp(argv[i], LEVELS_OPT) == SAME) {
            settings.levels_to_use = atoi(argv[++i]);
            continue;
        }
        /*
        if (strcmp(argv[i], TOTAL_OPT) == SAME)
      {
        settings.total_value = atof(argv[++i]);
        settings.total = TRUE;
        continue;
      }
        if (strcmp(argv[i], TOTAL_USED_OPT) == SAME)
      {
        settings.total_used = TRUE;
        continue;
      }
        */

        if (i > 0)
            printf("\nWARNING: unknown option %s (skipped) \n", argv[i]);
    }

    if (mandatory > 0)
        usage();

    if (settings.p + settings.q > FULLPROB) {
        printf("\nError p[%d] + q[%d] must be < %d\n", settings.p, settings.q, FULLPROB);
        exit(0);
    }

    settings.problem_file = open_file(out_name, "w");

    if (settings.dw_out) {
        dw_out_name = newString(strlen(out_name) + 4);
        sprintf(dw_out_name, "%s.dw", out_name);
        settings.dw_problem_file = open_file(dw_out_name, "w");
    }
}


int retrieve_simple(string name) {
    int i;
    for (i = 0; i < settings.num_of_simple_substances; i++) {
        if (strcmp(settings.simple_substances[i], name) == SAME)
            return i;
    }
    return -1;
}



int retrieve_dashed(string name) {
    int i;
    for (i = 0; i < settings.num_of_dashed_substances; i++) {
        if (strcmp(settings.dashed_substances[i], name) == SAME)
            return i;
    }
    return -1;
}



Bool is_dashed_already_collected(string name, token_list *collected) {
    token_list *tmp;
    for (tmp = collected; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->item, name) == SAME)
            return TRUE;
    }
    return FALSE;
}



int collect_dashed(token_list *toscan) {
    int i;
    string token;
    char aux[MAX_LINE_SIZE];
    token_list *tmp, *new_item;
    token_list *dashed = NULL;
    settings.num_of_dashed_substances = 0;
    for (tmp = toscan; tmp != NULL; tmp = tmp->next) {
        sprintf(aux, "%s", tmp->item);
        token = strtok(aux, "+=[]>");
        while (token != NULL) {
            if (retrieve_simple(token) < 0) {
                if (!is_dashed_already_collected(token, dashed)) {
                    settings.num_of_dashed_substances++;
                    new_item = (token_list *)calloc(1, sizeof(token_list));
                    new_item->item = newString(strlen(token) + 1);
                    sprintf(new_item->item, "%s", token);
                    new_item->next = dashed;
                    dashed = new_item;
                }
            }
            token = strtok(NULL, "+=[]>");
        }
    }

    settings.dashed_substances = (string *)calloc(settings.num_of_dashed_substances, sizeof(string));
    for (new_item = dashed, i = 0; new_item != NULL; new_item = new_item->next)
        settings.dashed_substances[i++] = new_item->item;

    if (i != settings.num_of_dashed_substances)
        printf("\nWARNING wrong number of substances. Debug me.");
}



void collect_reactions(reaction_list *toscan) {
    int num;
    reaction_list *tmp;

    for (tmp = toscan, num = 0; tmp != NULL; tmp = tmp->next, num++);
    settings.reactions = (reaction_desc *)calloc(num, sizeof(reaction_desc));

    for (tmp = toscan, num = 0; tmp != NULL; tmp = tmp->next, num++) {
        memcpy(&settings.reactions[num], &tmp->reaction, sizeof(reaction_desc));
    }

    settings.num_of_reactions = num;
}


void parse_objects() {
    int numC, numL, i;
    char c;
    token_list *new_item;
    Bool end_of_file = FALSE;

    string temp = newString(MAX_LINE_SIZE);
    lines = NULL;
    numL = 0;

    new_item = (token_list *)calloc(1, sizeof(token_list));
    new_item->item = newString(strlen(STR_EMPTY) + 1);
    sprintf(new_item->item, "%s", STR_EMPTY);
    new_item->next = lines;
    lines = new_item;
    numL++;

    while (!end_of_file) {
        numC = 0;
        while (TRUE) {
            if (fscanf(settings.objects_file, "%c", &c) < 0) {
                end_of_file = TRUE;
                break;
            }
            if (c == '\n' || c == '\r')
                break;
            if (c == COMMENT_CHAR) { //skip comments
                while ((c != '\n') && (c != '\r') && (fscanf(settings.objects_file, "%c", &c) >= 0));
                break;
            }
            if (c != ' ') //skip spaces
                temp[numC++] = c;
        }

        if (numC == 0) //skip empty lines
            continue;

        temp[numC] = '\0';
        new_item = (token_list *)calloc(1, sizeof(token_list));
        new_item->item = newString(strlen(temp) + 1);
        sprintf(new_item->item, "%s", temp);
        new_item->next = lines;
        lines = new_item;
        numL++;
    }

    free(temp);

    settings.simple_substances = (string *)calloc(numL, sizeof(string));
    for (new_item = lines, i = 0; new_item != NULL; new_item = new_item->next)
        settings.simple_substances[i++] = new_item->item;

    settings.num_of_simple_substances = i;

    if (i != numL)
        printf("\nWARNING wrong number of substances. Debug me.");
}


int retrieve_substance_index(string name) {
    int s;

    s = retrieve_simple(name);
    if (s >= 0)
        return s;

    s = retrieve_dashed(name);
    if (s >= 0)
        return -s - 1;

    printf("\nERROR substance %s not found\n", name);
    exit(1);

    return -1;
}


void check_token(string token) {
    if (token == NULL) {
        printf("\nERROR building reaction %s\n", token);
        exit(1);
    }
}


void build_reaction(string toscan) {
    string token;
    reaction_list *new_reaction;
    Bool catalyze = FALSE;
    Bool dash_to_plus = TRUE;

    //decide if it is a composition or a decomposition
    if (strchr(toscan, '+') != NULL) {
        if (strchr(toscan, '>') == NULL) {
            printf("ERROR scanning reaction %s [ can not find > ]\n", toscan);
            exit(1);
        }

        if (strchr(toscan, '+') < strchr(toscan, '>'))
            dash_to_plus = FALSE; //it is a composition
    }

    new_reaction = (reaction_list *)calloc(1, sizeof(reaction_list));

    catalyze = (strchr(toscan, '[') != NULL); //true iff there's a catalyst

    //first substance
    token = strtok(toscan, ">[]+");
    check_token(token);
    if (strcmp(token, STR_EMPTY) == SAME) {
        if (settings.skip_empty)
            return;

        if (!catalyze) {
            printf("\nERROR empy in left size, not catalyst");
        }

        new_reaction->reaction.type = PLUS_CAT;
        token = strtok(NULL, ">[]+");
        check_token(token);
        new_reaction->reaction.s1 = retrieve_substance_index(token);
        token = strtok(NULL, ">[]+");
        check_token(token);
        new_reaction->reaction.s2 =
            new_reaction->reaction.s3 = retrieve_substance_index(token);
    } else {
        if (catalyze) {
            new_reaction->reaction.type = PLUS_TO_PLUS_CAT;
            new_reaction->reaction.s1 = retrieve_substance_index(token);
            token = strtok(NULL, ">[]+");
            check_token(token);
            new_reaction->reaction.s2 = retrieve_substance_index(token);
            token = strtok(NULL, ">[]+");
            check_token(token);

            if (strcmp(token, STR_EMPTY) == SAME) {
                if (settings.skip_empty)
                    return;

                new_reaction->reaction.s3 = new_reaction->reaction.s2;
            } else {
                new_reaction->reaction.s3 = retrieve_substance_index(token);
            }
        } else {
            if (dash_to_plus) {
                new_reaction->reaction.type = DASH_TO_PLUS;
                new_reaction->reaction.s1 = retrieve_substance_index(token);
                token = strtok(NULL, ">[]+");
                check_token(token);
                new_reaction->reaction.s2 = retrieve_substance_index(token);
                token = strtok(NULL, ">[]+");
                check_token(token);
                new_reaction->reaction.s3 = retrieve_substance_index(token);
            } else {
                new_reaction->reaction.type = PLUS_TO_DASH;
                new_reaction->reaction.s1 = retrieve_substance_index(token);
                token = strtok(NULL, ">[]+");
                check_token(token);
                new_reaction->reaction.s2 = retrieve_substance_index(token);
                token = strtok(NULL, ">[]+");
                check_token(token);
                new_reaction->reaction.s3 = retrieve_substance_index(token);
            }
        }
    }

    new_reaction->next = reactions;
    reactions = new_reaction;
}


void parse_reactions() {
    int numC, numL, i;
    char c;
    token_list *new_item;
    Bool end_of_file = FALSE;

    string temp = newString(MAX_LINE_SIZE);
    lines = NULL;
    numL = 0;
    while (!end_of_file) {
        numC = 0;
        while (TRUE) {
            if (fscanf(settings.reactions_file, "%c", &c) < 0) {
                end_of_file = TRUE;
                break;
            }
            if (c == '\n' || c == '\r')
                break;
            if (c == COMMENT_CHAR) { //skip comments
                while ((c != '\n') && (c != '\r') && (fscanf(settings.reactions_file, "%c", &c) >= 0));
                break;
            }
            if (c != ' ') //skip spaces
                temp[numC++] = c;
        }

        if (numC == 0) //skip empty lines
            continue;

        temp[numC] = '\0';
        new_item = (token_list *)calloc(1, sizeof(token_list));
        new_item->item = newString(strlen(temp) + 1);
        sprintf(new_item->item, "%s", temp);
        new_item->next = lines;
        lines = new_item;
        numL++;
    }

    collect_dashed(lines);

    for (new_item = lines; new_item != NULL; new_item = new_item->next)
        build_reaction(new_item->item);

    collect_reactions(reactions);
}


/*************************************************************************/
/*************************************************************************/

void increase_level(int index) {
    int l = 0;

    if (!settings.quantity_levels)
        return;

    if (index > 0) {
        l = ++problem.simple_levels[index];
    } else {
        l = ++problem.dashed_levels[DECODE_INDEX(index)];
    }

    if (l > problem.max_level)
        problem.max_level = l;
}

void set_used(int index) {
    if (index >= 0) {
        if (!GET_BIT(problem.simple_used, index)) {
            SET_BIT(problem.simple_used, index);
            problem.num_simple_used++;
        }
    } else {
        if (!GET_BIT(problem.dashed_used, DECODE_INDEX(index))) {
            SET_BIT(problem.dashed_used, DECODE_INDEX(index));
        }
    }
}



Bool test_available(int index) {
    if (index >= 0) {
        if (GET_BIT(problem.simple_available, index)) {
            increase_level(index);
            return TRUE;
        }
    } else {
        if (GET_BIT(problem.dashed_available, DECODE_INDEX(index))) {
            increase_level(index);
            return TRUE;
        }
    }

    return FALSE;
}

Bool set_avail_sub(int index) {
    increase_level(index);

    if (index >= 0) {
        if (!GET_BIT(problem.simple_available, index)) {
            SET_BIT(problem.simple_available, index);
            if (settings.verbose)
                printf("\nNew available: %s", GET_SUBSTANCE(index));
            return TRUE;
        }
    } else {
        if (!GET_BIT(problem.dashed_available, DECODE_INDEX(index))) {
            SET_BIT(problem.dashed_available, DECODE_INDEX(index));
            if (settings.verbose)
                printf("\nNew available: %s", GET_SUBSTANCE(index));
            return TRUE;
        }
    }

    return FALSE;
}

void alloc_problem_data() {
    problem.num_of_initial_substances = 0;
    problem.initial_substances = (int *)calloc(settings.max_init, sizeof(int));

    problem.block_simple_available = (settings.num_of_simple_substances >> 5) + 1;
    problem.simple_available = (int *)calloc(problem.block_simple_available, sizeof(int));
    problem.simple_used = (int *)calloc(problem.block_simple_available, sizeof(int));
    problem.initial_simple_available = (int *)calloc(problem.block_simple_available, sizeof(int));

    problem.block_dashed_available = (settings.num_of_dashed_substances >> 5) + 1;
    problem.dashed_available = (int *)calloc(problem.block_dashed_available, sizeof(int));
    problem.dashed_used = (int *)calloc(problem.block_dashed_available, sizeof(int));
    problem.dashed_already_used = (int *)calloc(problem.block_dashed_available, sizeof(int));

    problem.dashed_already_used = (int *)calloc(problem.block_simple_available, sizeof(int));

    problem.reaction_already_used = (int *)calloc(1000, sizeof(int));

    problem.block_reactions = (settings.num_of_reactions >> 5) + 1;
    problem.to_apply = (int *)calloc(problem.block_reactions, sizeof(int));
    problem.applied = (int *)calloc(problem.block_reactions, sizeof(int));

    problem.last_dashed = (int *)calloc(settings.num_of_dashed_substances, sizeof(int));

    if (settings.quantity_levels) {
        problem.simple_levels = (int *)calloc(settings.num_of_simple_substances, sizeof(int));
        problem.dashed_levels = (int *)calloc(settings.num_of_dashed_substances, sizeof(int));
    }

    problem.num_simple_used = 0;
}

void reset_problem() {
    memset(problem.simple_available, 0, problem.block_simple_available * sizeof(int));
    memset(problem.simple_used, 0, problem.block_simple_available * sizeof(int));

    memset(problem.dashed_available, 0, problem.block_dashed_available * sizeof(int));
    memset(problem.dashed_used, 0, problem.block_dashed_available * sizeof(int));

    memset(problem.dashed_already_used, 0, problem.block_dashed_available * sizeof(int));

    memset(problem.reaction_already_used, 0, 1000 * sizeof(int));

    memset(problem.to_apply, 0, problem.block_reactions * sizeof(int));
    memset(problem.applied, 0, problem.block_reactions * sizeof(int));

    memcpy(problem.simple_available, problem.initial_simple_available, problem.block_simple_available * sizeof(int));

    if (settings.quantity_levels) {
        memset(problem.simple_levels, 0, settings.num_of_simple_substances * sizeof(int));
        memset(problem.dashed_levels, 0, settings.num_of_dashed_substances * sizeof(int));
    }

    problem.num_simple_used = 0;
}

void build_goals() {
    int r, clev, cgoal, num_lev, tot, offset, i, j;
    Bool break_it;
    results_list *tmp_res;

    problem.num_last_dashed = 0;

    for (tmp_res = results, tot = 0, num_lev = 0; tmp_res != NULL; tmp_res = tmp_res->next) {
        num_lev++;
        tot += tmp_res->num_dashed;
    }

    if (settings.num_goals < 0) {
        offset = tot / results->num_dashed;
        settings.num_goals = results->num_dashed + rand() % offset;
    }

    if (settings.verbose) {
        printf("\n\nBuilding %d goals. Num levels = %d, p = %d, q = %d", settings.num_goals, num_lev, settings.p, settings.q);
    }

    if (settings.dj_goals)
        settings.num_goals *= 2;

    for (i = 0; i < settings.num_goals; i++) {
        break_it = FALSE;
        r = rand() % FULLPROB;

        /* INVERTO LA PROBABILITA'... */
        if (settings.dj_goals && (i % 2))
            r = FULLPROB - r;
        /******************************/

        if (r < settings.p) { //choose a goal from last level with probability p
            while (results->num_dashed <= 0) {
                if (results->next == NULL)
                    return;
                memcpy(results, results->next, sizeof(results_list));
                num_lev--;
            }

            cgoal = rand() % results->num_dashed;
            problem.last_dashed[problem.num_last_dashed++] = results->dashed[cgoal];
            results->dashed[cgoal] = results->dashed[--results->num_dashed];

            if (settings.verbose) {
                printf("\nChoosing goal from last level: ");
                printf("%s", GET_SUBSTANCE(problem.last_dashed[problem.num_last_dashed - 1]));
            }
        } else if (r < (settings.p + settings.q)) { //choose a goal from last-1 with probability q
            if (num_lev > 1)
                tmp_res = results->next;
            else
                tmp_res = results;

            while (tmp_res->num_dashed <= 0) {
                if (tmp_res->next == NULL) {
                    i--;
                    break_it = TRUE;
                    break;
                }
                memcpy(tmp_res, tmp_res->next, sizeof(results_list));
                num_lev--;
            }

            if (break_it)
                continue;

            cgoal = rand() % tmp_res->num_dashed;
            problem.last_dashed[ problem.num_last_dashed++] = tmp_res->dashed[cgoal];
            tmp_res->dashed[cgoal] = tmp_res->dashed[--tmp_res->num_dashed];


            if (settings.verbose) {
                printf("\nChoosing goal from last-1 level: ");
                printf("%s", GET_SUBSTANCE(problem.last_dashed[problem.num_last_dashed - 1]));
            }
        } else { //choose a goal from another level with probability 1-(p+q)
            clev = rand() % num_lev;
            for (tmp_res = results, j = clev - 1; (tmp_res->next != NULL) && (j > 0); tmp_res = tmp_res->next, j--);

            while (tmp_res->num_dashed <= 0) {
                if (tmp_res->next == NULL) {
                    i--;
                    break_it = TRUE;
                    break;
                }

                memcpy(tmp_res, tmp_res->next, sizeof(results_list));
                num_lev--;
            }

            if (break_it)
                continue;

            cgoal = rand() % tmp_res->num_dashed;
            problem.last_dashed[ problem.num_last_dashed++] = tmp_res->dashed[cgoal];
            tmp_res->dashed[cgoal] = tmp_res->dashed[--tmp_res->num_dashed];

            if (settings.verbose) {
                printf("\nChoosing goal from level %d: ", num_lev - clev);
                printf("%s", GET_SUBSTANCE(problem.last_dashed[problem.num_last_dashed - 1]));
            }
        }
    }
}


int build_problem() {
    int i, max, sel, num_lev;
    Bool changed, reset;
    Bool incremental, rebuild;
    reaction_desc *reaction;
    results_list *tmp_res;
    int simple[settings.num_of_simple_substances];

    if (settings.seed >= 0)
    {
        srand(settings.seed);
    }
    else
    {
        /* seed the random() function */
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);
        srand(tv.tv_usec);
    }

    incremental = TRUE;
    rebuild = FALSE;

    while (incremental) {
        if (rebuild)
            incremental = FALSE;

        if (settings.max_init < 0)
            settings.max_init = settings.num_of_simple_substances;

        if (settings.max_act < 0)
            settings.max_act = settings.num_of_reactions;

        if (incremental)
            alloc_problem_data();
        else
            reset_problem();

        if (settings.verbose)
            printf("\nBuilding problem...");

        if (incremental) {
            if (settings.verbose)
                printf("\nMode: incremental initial state.");

            for (i = 0; i < settings.num_of_simple_substances; i++)
                simple[i] = i;

            max = settings.num_of_simple_substances;
        } else {
            if (settings.verbose)
                printf("\nMode: fix point mode from initial state.");
        }

        problem.num_act = num_lev = 0;
        changed = TRUE;
        problem.num_last_dashed = 0;
        while (TRUE) {
            reset = TRUE;

            if (!changed) {
                if (settings.verbose)
                    printf("\nFix point reached. Initialize next flip.");

                if (problem.num_act >= settings.max_act) {
                    if (settings.verbose)
                        printf("\nSTOP. %d reaction used (min = %d)", problem.num_act, settings.max_act);
                    break;
                }

                if (incremental) {
                    if ((max > 0) && (problem.num_of_initial_substances < settings.max_init)) {
                        sel = rand() % max;
                        if (settings.verbose)
                            printf("\nSELECTED: %s", GET_SUBSTANCE(simple[sel]));
                        problem.initial_substances[problem.num_of_initial_substances++] = simple[sel];
                        SET_BIT(problem.simple_available, simple[sel]);
                        SET_BIT(problem.initial_simple_available, simple[sel]);
                        simple[sel] = simple[--max];
                    } else {
                        if (settings.verbose)
                            printf("\nSTOP. %d simple substances used (max = %d)",
                                   problem.num_of_initial_substances, settings.max_init);
                        break;
                    }
                } else {
                    if (settings.verbose)
                        printf("\nSTOP. FIXPOINT REACHED.");
                    break;
                }
            }


            if (settings.verbose)
                printf("\nREACH flip %d", ++num_lev);

            changed = FALSE;
            memset(problem.to_apply, 0, problem.block_reactions * sizeof(int));

            for (i = 0; i < settings.num_of_reactions; i++) {
                reaction = &settings.reactions[i];
                switch (reaction->type) {
                case PLUS_TO_DASH:
                    if (test_available(reaction->s1) && test_available(reaction->s2)) {
                        if (!GET_BIT(problem.applied, i))
                            SET_BIT(problem.to_apply, i);
                    }
                    break;
                case DASH_TO_PLUS:
                    if (test_available(reaction->s1)) {
                        if (!GET_BIT(problem.applied, i))
                            SET_BIT(problem.to_apply, i);
                    }
                    break;
                case PLUS_TO_PLUS_CAT:
                    if (test_available(reaction->s1) && test_available(reaction->s2)) {
                        if (!GET_BIT(problem.applied, i))
                            SET_BIT(problem.to_apply, i);
                    }
                    break;
                case PLUS_CAT:
                    if (test_available(reaction->s1)) {
                        if (!GET_BIT(problem.applied, i))
                            SET_BIT(problem.to_apply, i);
                    }
                    break;
                default:
                    printf("\nERROR IN REACTION TYPE!\n");
                    break;
                }
            }

            for (i = 0; i < settings.num_of_reactions; i++) {
                if (GET_BIT(problem.to_apply, i)) {
                    problem.num_act++;
                    SET_BIT(problem.applied, i);
                    reaction = &settings.reactions[i];
                    set_used(reaction->s1);
                    set_used(reaction->s2);
                    set_used(reaction->s3);
                    switch (reaction->type) {
                    case PLUS_TO_DASH:
                        if (set_avail_sub(reaction->s3)) {
                            if (reset) {
                                problem.num_last_dashed = 0;
                                reset = FALSE;
                            }
                            problem.last_dashed[problem.num_last_dashed++] = reaction->s3;
                            changed = TRUE;
                        }
                        break;
                    case DASH_TO_PLUS:
                        if (set_avail_sub(reaction->s2)) {
                            if (reset) {
                                problem.num_last_dashed = 0;
                                reset = FALSE;
                            }
                            problem.last_dashed[problem.num_last_dashed++] = reaction->s2;
                            changed = TRUE;
                        }
                        if (set_avail_sub(reaction->s3)) {
                            if (reset) {
                                problem.num_last_dashed = 0;
                                reset = FALSE;
                            }
                            problem.last_dashed[problem.num_last_dashed++] = reaction->s3;
                            changed = TRUE;
                        }
                        break;
                    case PLUS_TO_PLUS_CAT:
                        if (set_avail_sub(reaction->s3)) {
                            if (reset) {
                                problem.num_last_dashed = 0;
                                reset = FALSE;
                            }
                            problem.last_dashed[problem.num_last_dashed++] = reaction->s3;
                            changed = TRUE;
                        }
                        break;
                    case PLUS_CAT:
                        if (set_avail_sub(reaction->s3)) {
                            if (reset) {
                                problem.num_last_dashed = 0;
                                reset = FALSE;
                            }
                            problem.last_dashed[problem.num_last_dashed++] = reaction->s3;
                            changed = TRUE;
                        }
                        break;
                    default:
                        printf("\nERROR IN REACTION TYPE!\n");
                        break;
                    }
                }
            }


            if (!incremental && changed) {
                tmp_res = (results_list *)calloc(1, sizeof(results_list));
                tmp_res->dashed = (int *)calloc(problem.num_last_dashed, sizeof(int));
                tmp_res->num_dashed = problem.num_last_dashed;
                memcpy(tmp_res->dashed, problem.last_dashed, tmp_res->num_dashed * sizeof(int));
                tmp_res->next = results;
                results = tmp_res;
            }
        }

        rebuild = TRUE;
        settings.max_act = settings.num_of_reactions;
    }

    if (settings.countsubs) {
        //int gap;

        problem.max_sub_level = problem.num_simple_used;

        /*
        gap = (problem.num_of_initial_substances - problem.num_simple_used) / 3;

        if (gap > 0)
      problem.max_sub_level += gap + rand() % gap;

         if (settings.verbose)
      {
        printf("\n\nNumber of initial substances: %d", problem.num_of_initial_substances);
        printf("\nNumber of simple substances used: %d", problem.num_simple_used);
        printf("\nGap: %d", gap);
        printf("\nMax number of simple substances to choose: %d", problem.max_sub_level);
      }
        */
    }

    build_goals();

    return num_lev;
}



char *pref_symb(int p, int num) {
    char *res;
    int r, q, l;

    res = (char *)calloc(10, sizeof(char));
    sprintf(res, "p%d", p);

    l = strlen(res);

    q = num - 1;
    do {
        r = q % PREF_BASE;
        q = q / PREF_BASE;
        res[l++] = (char)(r + PREF_START);
    } while (q > 0);

    res[l] = '\0';

    return res;
}


int build_reaction_vector_from_reach_tree(int goalSubstance, int *reactions, int *size, int *tabu) {
    static int *nodes = NULL;
    int num, choice;
    int i, pos, next, sub;

    num = 0;

    if (nodes == NULL)
        nodes = (int *)calloc(settings.num_of_reactions, sizeof(int));
    else
        memset(nodes, 0, settings.num_of_reactions * sizeof(int));

    for (i = 0; i < settings.num_of_reactions; i++) {
        if (!GET_BIT(problem.applied, i))
            continue;

        if (GET_BIT(tabu, i)) //evito cicli
            continue;

        if (settings.reactions[i].s3 == goalSubstance)
            nodes[num++] = i;
    }

    do {
        if (num <= 0) {
            choice = -1;
            break;
        }

        pos = rand() % num;
        choice = nodes[pos];
        nodes[pos] = nodes[--num];
    } while ((settings.reactions[choice].s1 >= 0) && (settings.reactions[choice].s2 >= 0));


    if (choice >= 0) { //esiste almeno una reazione ==> posso continuare ricorsivamente!
        SET_BIT(tabu, choice);
        reactions[(*size)++] = choice; //costruisco il vettore delle reazioni

        if (FALSE /*rand() % 2*/) //SCENDO SEMPRE RICORSIVAMENTE
            return choice;
        else {
            if (rand() % 2) {
                next = settings.reactions[choice].s1;
                if (next >= 0)
                    next = settings.reactions[choice].s2;
            } else {
                next = settings.reactions[choice].s2;
                if (next >= 0)
                    next = settings.reactions[choice].s1;
            }

            if (next >= 0)
                return choice;

            sub = build_reaction_vector_from_reach_tree(next, reactions, size, tabu);

            if (sub >= 0)
                return sub;
            else
                return choice;
        }
    }

    return choice;
}


int select_random_reaction_in_reach_tree(int goalSubstance) {
    int num, selection;
    static int *reactions = NULL;
    static int *tabu = NULL;

    if (tabu == NULL)
        tabu = (int *)calloc(problem.block_reactions, sizeof(int));
    else
        memset(tabu, 0, problem.block_reactions * sizeof(int));

    if (reactions == NULL)
        reactions = (int *)calloc(settings.num_of_reactions, sizeof(int));
    else
        memset(reactions, 0, settings.num_of_reactions * sizeof(int));

    num = 0;
    build_reaction_vector_from_reach_tree(goalSubstance, reactions, &num, tabu);

    if (num > 0)
        selection = reactions[rand() % num];
    else
        selection = -1;

    return selection;
}



void print_constraints_preferences(FILE *output) {
    int i, reaction, s1, s2;

    fprintf(output, CONSTR_LBL);

    /**
    for (i = 0; i < problem.num_last_dashed; i++) //for all goal substances
      fprintf(output, STR_P_SOMETIME, "p", GET_SUBSTANCE(problem.last_dashed[i]));
    **/

    fprintf(output, "\n");

    for (i = 0; i < problem.num_last_dashed; i++) { //for all goal substances
        if (problem.last_dashed[i] >= 0) //skip simple if any
            continue;

        reaction = select_random_reaction_in_reach_tree(problem.last_dashed[i]);

        if (reaction >= 0 && !(random() % 5) &&
            !GET_BIT(problem.reaction_already_used, reaction) &&
            ((settings.reactions[reaction].s1 < 0 &&
              !GET_BIT(problem.dashed_already_used,
                       DECODE_INDEX(settings.reactions[reaction].s1))) ||
             (settings.reactions[reaction].s2 < 0 &&
              !GET_BIT(problem.dashed_already_used,
                       DECODE_INDEX(settings.reactions[reaction].s2))))) {
            SET_BIT(problem.reaction_already_used, reaction);

            if (settings.reactions[reaction].s1 < 0 &&
                settings.reactions[reaction].s2 >= 0 &&
                !GET_BIT(problem.dashed_already_used,
                         DECODE_INDEX(settings.reactions[reaction].s1))) {
                SET_BIT(problem.dashed_already_used,
                        DECODE_INDEX(settings.reactions[reaction].s1));

                used_pref[1]++;
                symb = pref_symb(1, used_pref[1]);

                if (settings.numeric) {
                    extra_weight++;

                    fprintf(output, STR_P_SOMETIME_BEFORE_NUM, symb,
                            GET_SUBSTANCE(problem.last_dashed[i]),
                            GET_SUBSTANCE(settings.reactions[reaction].s1));
                } else {
                    extra_weight++;

                    fprintf(output, STR_P_SOMETIME_BEFORE, symb,
                            GET_SUBSTANCE(problem.last_dashed[i]),
                            GET_SUBSTANCE(settings.reactions[reaction].s1));
                }
            } else if (settings.reactions[reaction].s2 < 0 &&
                       settings.reactions[reaction].s1 >= 0 &&
                       !GET_BIT(problem.dashed_already_used,
                                DECODE_INDEX(settings.reactions[reaction].s2))) {
                SET_BIT(problem.dashed_already_used,
                        DECODE_INDEX(settings.reactions[reaction].s2));

                used_pref[1]++;
                symb = pref_symb(1, used_pref[1]);

                if (settings.numeric) {
                    extra_weight++;

                    fprintf(output, STR_P_SOMETIME_BEFORE_NUM, symb,
                            GET_SUBSTANCE(problem.last_dashed[i]),
                            GET_SUBSTANCE(settings.reactions[reaction].s2));
                } else {
                    extra_weight++;

                    fprintf(output, STR_P_SOMETIME_BEFORE, symb,
                            GET_SUBSTANCE(problem.last_dashed[i]),
                            GET_SUBSTANCE(settings.reactions[reaction].s2));
                }
            } else if (settings.reactions[reaction].s2 < 0 &&
                       settings.reactions[reaction].s1 < 0 &&
                       (!GET_BIT(problem.dashed_already_used,
                                 DECODE_INDEX(settings.reactions[reaction].s1)) ||
                        !GET_BIT(problem.dashed_already_used,
                                 DECODE_INDEX(settings.reactions[reaction].s2)))) {
                if ((random() % 2) ||
                    problem.last_dashed[i] == settings.reactions[reaction].s2 ||
                    !GET_BIT(problem.dashed_already_used,
                             DECODE_INDEX(settings.reactions[reaction].s1))) { //ZZZ
                    SET_BIT(problem.dashed_already_used,
                            DECODE_INDEX(settings.reactions[reaction].s1));

                    used_pref[1]++;
                    symb = pref_symb(1, used_pref[1]);

                    if (settings.numeric) {
                        extra_weight++;

                        fprintf(output, STR_P_SOMETIME_BEFORE_NUM, symb,
                                GET_SUBSTANCE(problem.last_dashed[i]),
                                GET_SUBSTANCE(settings.reactions[reaction].s1));
                    } else {
                        extra_weight++;

                        fprintf(output, STR_P_SOMETIME_BEFORE, symb,
                                GET_SUBSTANCE(problem.last_dashed[i]),
                                GET_SUBSTANCE(settings.reactions[reaction].s1));
                    }
                } else if (!GET_BIT(problem.dashed_already_used,
                                    DECODE_INDEX(settings.reactions[reaction].s2))) {
                    SET_BIT(problem.dashed_already_used,
                            DECODE_INDEX(settings.reactions[reaction].s2));

                    used_pref[1]++;
                    symb = pref_symb(1, used_pref[1]);

                    if (settings.numeric) {
                        extra_weight++;

                        fprintf(output, STR_P_SOMETIME_BEFORE_NUM, symb,
                                GET_SUBSTANCE(problem.last_dashed[i]),
                                GET_SUBSTANCE(settings.reactions[reaction].s2));
                    } else {
                        extra_weight++;

                        fprintf(output, STR_P_SOMETIME_BEFORE, symb,
                                GET_SUBSTANCE(problem.last_dashed[i]),
                                GET_SUBSTANCE(settings.reactions[reaction].s2));
                    }
                }
            } else {
                printf("\nWarning: No simple substance in reaction!!\n\n");
                exit(0);
            }
        }
    }

    fprintf(output, "\n");

    /* Calcolo di extra-weight dovuto a total-time per versione con constraints */
    if (settings.constraints_pref && settings.numeric) {
        float tmp;
        tmp = settings.totaltime;
        coeff = 1.0;
        while (tmp > 10.0) {
            tmp /= 10.0;
            coeff *= 10.0;
        }
        extra_weight += ceilf(tmp);
    }

    for (i = 0; i < problem.num_last_dashed; i++) { //for all goal substances
        if (problem.last_dashed[i] >= 0) //skip simple if any
            continue;

        reaction = select_random_reaction_in_reach_tree(problem.last_dashed[i]);

        if (reaction >= 0 && !(random() % 5) &&
            !GET_BIT(problem.reaction_already_used, reaction) &&
            ((settings.reactions[reaction].s1 < 0 &&
              !GET_BIT(problem.dashed_already_used,
                       DECODE_INDEX(settings.reactions[reaction].s1))) ||
             (settings.reactions[reaction].s2 < 0 &&
              !GET_BIT(problem.dashed_already_used,
                       DECODE_INDEX(settings.reactions[reaction].s2))))) {
            SET_BIT(problem.reaction_already_used, reaction);

            if (settings.reactions[reaction].s1 < 0 &&
                settings.reactions[reaction].s2 >= 0 &&
                !GET_BIT(problem.dashed_already_used,
                         DECODE_INDEX(settings.reactions[reaction].s1))) {
                if (is_a_goal_substance(DECODE_INDEX(settings.reactions[reaction].s1)))
                    continue;

                extra_weight++;

                used_pref[1]++;
                symb = pref_symb(1, used_pref[1]);


                SET_BIT(problem.dashed_already_used,
                        DECODE_INDEX(settings.reactions[reaction].s1));

                if (settings.numeric) {
                    fprintf(output, STR_P_ALWAYS_NOT_NUM, symb,
                            GET_SUBSTANCE(settings.reactions[reaction].s1));
                } else {
                    fprintf(output, STR_P_ALWAYS_NOT, symb,
                            GET_SUBSTANCE(settings.reactions[reaction].s1));
                }
            } else if (settings.reactions[reaction].s2 < 0 &&
                       settings.reactions[reaction].s1 >= 0 &&
                       !GET_BIT(problem.dashed_already_used,
                                DECODE_INDEX(settings.reactions[reaction].s2))) {
                if (is_a_goal_substance(DECODE_INDEX(s2)))
                    continue;

                extra_weight++;

                used_pref[1]++;
                symb = pref_symb(1, used_pref[1]);

                SET_BIT(problem.dashed_already_used,
                        DECODE_INDEX(settings.reactions[reaction].s2));

                if (settings.numeric) {
                    fprintf(output, STR_P_ALWAYS_NOT_NUM, symb,
                            GET_SUBSTANCE(settings.reactions[reaction].s2));
                } else {
                    fprintf(output, STR_P_ALWAYS_NOT, symb,
                            GET_SUBSTANCE(settings.reactions[reaction].s2));
                }
            } else if (settings.reactions[reaction].s2 < 0 &&
                       settings.reactions[reaction].s1 < 0 &&
                       (!GET_BIT(problem.dashed_already_used,
                                 DECODE_INDEX(settings.reactions[reaction].s1)) ||
                        !GET_BIT(problem.dashed_already_used,
                                 DECODE_INDEX(settings.reactions[reaction].s2)))) {
                if (random() % 2 &&
                    !GET_BIT(problem.dashed_already_used,
                             DECODE_INDEX(settings.reactions[reaction].s1))) { //ZZZ
                    if (is_a_goal_substance(DECODE_INDEX(s1)))
                        continue;

                    extra_weight++;

                    used_pref[1]++;
                    symb = pref_symb(1, used_pref[1]);

                    SET_BIT(problem.dashed_already_used,
                            DECODE_INDEX(settings.reactions[reaction].s1));

                    if (settings.numeric) {
                        fprintf(output, STR_P_ALWAYS_NOT_NUM, symb,
                                GET_SUBSTANCE(settings.reactions[reaction].s1));
                    } else {
                        fprintf(output, STR_P_ALWAYS_NOT, symb,
                                GET_SUBSTANCE(settings.reactions[reaction].s1));
                    }
                } else if (!GET_BIT(problem.dashed_already_used,
                                    DECODE_INDEX(settings.reactions[reaction].s2))) {
                    if (is_a_goal_substance(DECODE_INDEX(s2)))
                        continue;

                    extra_weight++;

                    used_pref[1]++;
                    symb = pref_symb(1, used_pref[1]);

                    SET_BIT(problem.dashed_already_used,
                            DECODE_INDEX(settings.reactions[reaction].s2));

                    if (settings.numeric) {
                        fprintf(output, STR_P_ALWAYS_NOT_NUM, symb,
                                GET_SUBSTANCE(settings.reactions[reaction].s2));
                    } else {
                        fprintf(output, STR_P_ALWAYS_NOT, symb,
                                GET_SUBSTANCE(settings.reactions[reaction].s2));
                    }
                }
            } else {
                printf("\nWarning: No simple substance in reaction!!\n\n");
                exit(0);
            }
        }
    }


    fprintf(output, "\n");


    for (i = 0; i < problem.num_last_dashed; i++) {
        if (problem.last_dashed[i] >= 0) //skip simple if any
            continue;

        if (!(random() % 5)) {
            do {
                s1 = random() % settings.num_of_dashed_substances;
            } while (!GET_BIT(problem.dashed_used, s1) ||
                     GET_BIT(problem.dashed_already_used, s1) ||
                     is_a_goal_substance(DECODE_INDEX(s1)));

            SET_BIT(problem.dashed_already_used, DECODE_INDEX(s1));


            do {
                s2 = random() % settings.num_of_dashed_substances;
            } while (!GET_BIT(problem.dashed_used, s2) ||
                     GET_BIT(problem.dashed_already_used, s2) ||
                     s1 == s2 ||
                     is_a_goal_substance(DECODE_INDEX(s2)));

            SET_BIT(problem.dashed_already_used, DECODE_INDEX(s2));

            if (s1 != s2) {
                if (settings.numeric) {
                    used_pref[1]++;
                    symb = pref_symb(1, used_pref[1]);

                    fprintf(output, STR_P_SOMETIME_NUM, symb,
                            settings.dashed_substances[s1]);

                    used_pref[1]++;
                    symb = pref_symb(1, used_pref[1]);

                    fprintf(output, STR_P_SOMETIME_AFTER_NUM, symb,
                            settings.dashed_substances[s1],
                            settings.dashed_substances[s2]);
                } else {
                    used_pref[1]++;
                    symb = pref_symb(1, used_pref[1]);


                    fprintf(output, STR_P_SOMETIME, symb,
                            settings.dashed_substances[s1]);

                    used_pref[1]++;
                    symb = pref_symb(1, used_pref[1]);


                    fprintf(output, STR_P_SOMETIME_AFTER, symb,
                            settings.dashed_substances[s1],
                            settings.dashed_substances[s2]);
                }
            }
        }
    }



    /**
    for (i = 0; i < settings.levels_to_use; i++)
      {
        fprintf(output, STR_P_ALWAYS, "p", i);
      }
    **/

    fprintf(output, "))");
}



void print_preferences_metric(FILE *f) {
    int i, j;
    Bool first;

    first = TRUE;

    fprintf(f, "\n\n(:metric minimize (+ ");
    for (i = 0; i < MAX_PREFERENCES; i++) {
        if (preferences.count[i] <= 0)
            continue;

        for (j = 0; j < preferences.count[i]; j++) {
            if (!first)
                fprintf(f, "\n\t\t     ");
            fprintf(f, "(* %d (is-violated %s))", preferences.weight[i], pref_symb(i, j + 1));
            first = FALSE;
        }
    }

    fprintf(f, "\n\t\t     ");
    fprintf(f, "(* 1 (is-violated p-drive))");

    fprintf(f, "))");
}



/*************************************************************************/
/*************************************************************************/


void print_pddl_problem(FILE *output, Bool dw_style) {
    int i, j, first, g, un_s, un_r;

    if (dw_style)
        settings.verbose = FALSE;

    //  comment(output);

    fprintf(output, "; Domain designed by Yannis Dimopoulos, Alfonso Gerevini and Alessandro Saetti\n; This file has been automatically generated by the generator available from\n; http://zeus.ing.unibs.it/ipc-5/generators/index.html\n\n");

    fprintf(output, PROBLEM_HEADER, settings.prob_name);

    if (settings.constraints_pref && settings.numeric)
        fprintf(output, "%s%s)", DOMAIN_REF, "ComplexPreferences");
    else if (settings.constraints_pref)
        fprintf(output, "%s%s)", DOMAIN_REF, "PropositionalPreferences");
    else if (settings.simple_preference)
        fprintf(output, "%s%s)", DOMAIN_REF, "SimplePreferences");
    else if (settings.numeric)
        fprintf(output, "%s%s)", DOMAIN_REF, "MetricTime");
    else
        fprintf(output, "%s%s)", DOMAIN_REF, "Propositional");

    fprintf(output, LBL_OBJECTS);

    for (i = 0; i < settings.num_of_simple_substances; i++) {
        if (!GET_BIT(problem.simple_used, i))
            continue;
        if (strcmp(settings.simple_substances[i], STR_EMPTY) == SAME)
            continue;
        fprintf(output, "\n\t%s - %s", settings.simple_substances[i],
                (dw_style) ? "substance" : "simple");
    }

    for (i = 0; i < settings.num_of_dashed_substances; i++) {
        if (!GET_BIT(problem.dashed_used, i))
            continue;
        fprintf(output, "\n\t%s - %s", settings.dashed_substances[i],
                (dw_style) ? "substance" : "complex");
    }

    if (settings.levels_to_use < 0)
        settings.levels_to_use = MAX(problem.max_sub_level, problem.max_level);

    if (!settings.numeric) {
        if (settings.countsubs || settings.quantity_levels) {
            for (i = 0; i <= settings.levels_to_use; i++) {
                fprintf(output, "\n\tl%d - level", i);
            }
        }
    }

    fprintf(output, ")\n");

    fprintf(output, LBL_INIT);

    /**
    if (settings.total)
      fprintf(output, STR_TOTAL, 0);
      //    fprintf(output, STR_TOTAL, settings.total_value);
    if (settings.total_used)
      fprintf(output, STR_TOTAL_USED);
      **/

    if (dw_style) {
        for (i = 0; i < settings.num_of_simple_substances; i++) {
            if (!GET_BIT(problem.simple_used, i))
                continue;
            if (strcmp(settings.simple_substances[i], STR_EMPTY) == SAME)
                continue;
            fprintf(output, STR_SIMPLE, settings.simple_substances[i]);
        }

        for (i = 0; i < settings.num_of_dashed_substances; i++) {
            if (!GET_BIT(problem.dashed_used, i))
                continue;
            fprintf(output, STR_DASHED, settings.dashed_substances[i]);
        }
    }

    for (i = 0; i < settings.num_of_simple_substances; i++) {
        if (!GET_BIT(problem.simple_used, i))
            continue;
        if (strcmp(settings.simple_substances[i], STR_EMPTY) == SAME)
            continue;
        fprintf(output, STR_INIT_POSS, settings.simple_substances[i]);

        if (settings.numeric) {
            fprintf(output, STR_NUM_AVAIL, settings.simple_substances[i], 0.0);
            //      fprintf(output, STR_NUM_USED, settings.simple_substances[i], 0.0);
        }
    }

    if (settings.numeric) {
        for (i = 0; i < settings.num_of_dashed_substances; i++) {
            if (!GET_BIT(problem.dashed_used, i))
                continue;
            fprintf(output, STR_NUM_AVAIL, settings.dashed_substances[i], 0.0);
            //      fprintf(output, STR_NUM_USED, settings.dashed_substances[i], 0.0);
        }
    }

    if (settings.verbose) {
        un_r = 0;
        printf("\nUnapplied reactions:");
    }

    for (i = 0; i < settings.num_of_reactions; i++) {
        if (!GET_BIT(problem.applied, i)) {
            un_r++;
            if (settings.verbose) {
                switch (settings.reactions[i].type) {
                case PLUS_TO_PLUS_CAT:
                    printf(STR_INIT_PCCAT, GET_SUBSTANCE(settings.reactions[i].s1),
                           GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3));
                    break;
                case PLUS_CAT:
                    printf(STR_INIT_PCAT, GET_SUBSTANCE(settings.reactions[i].s1),
                           GET_SUBSTANCE(settings.reactions[i].s3));
                    break;
                case PLUS_TO_DASH:
                    printf(STR_INIT_PC, GET_SUBSTANCE(settings.reactions[i].s1),
                           GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3));
                    break;
                case DASH_TO_PLUS:
                    printf(STR_INIT_DD, GET_SUBSTANCE(settings.reactions[i].s1),
                           GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3));
                }
            }

            continue;
        }

        switch (settings.reactions[i].type) {
        case PLUS_TO_PLUS_CAT:
            fprintf(output, STR_INIT_PCCAT, GET_SUBSTANCE(settings.reactions[i].s1),
                    GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3));
            if (settings.numeric && settings.rnd_constants) {
                fprintf(output, STR_NEED_PCCAT, GET_SUBSTANCE(settings.reactions[i].s1),
                        GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3),
                        num_rand());
                fprintf(output, STR_NEED_PCCAT, GET_SUBSTANCE(settings.reactions[i].s2),
                        GET_SUBSTANCE(settings.reactions[i].s1), GET_SUBSTANCE(settings.reactions[i].s3),
                        num_rand());
                fprintf(output, STR_PROD_PCCAT, GET_SUBSTANCE(settings.reactions[i].s1),
                        GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3),
                        num_rand());
            }

            break;
        case PLUS_CAT:
            fprintf(output, STR_INIT_PCAT, GET_SUBSTANCE(settings.reactions[i].s1),
                    GET_SUBSTANCE(settings.reactions[i].s3));
            if (settings.numeric && settings.rnd_constants) {
                fprintf(output, STR_NEED_PCAT, GET_SUBSTANCE(settings.reactions[i].s1),
                        GET_SUBSTANCE(settings.reactions[i].s3),
                        num_rand());

                fprintf(output, STR_PROD_PCAT, GET_SUBSTANCE(settings.reactions[i].s1),
                        GET_SUBSTANCE(settings.reactions[i].s3),
                        num_rand());
            }

            break;
        case PLUS_TO_DASH:
            fprintf(output, STR_INIT_PC, GET_SUBSTANCE(settings.reactions[i].s1),
                    GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3));
            if (settings.numeric && settings.rnd_constants) {
                fprintf(output, STR_NEED_PC, GET_SUBSTANCE(settings.reactions[i].s1),
                        GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3),
                        num_rand());
                fprintf(output, STR_NEED_PC, GET_SUBSTANCE(settings.reactions[i].s2),
                        GET_SUBSTANCE(settings.reactions[i].s1), GET_SUBSTANCE(settings.reactions[i].s3),
                        num_rand());
                fprintf(output, STR_PROD_PC, GET_SUBSTANCE(settings.reactions[i].s1),
                        GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3),
                        num_rand());
            }

            break;
        case DASH_TO_PLUS:
            fprintf(output, STR_INIT_DD, GET_SUBSTANCE(settings.reactions[i].s1),
                    GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3));
            if (settings.numeric && settings.rnd_constants) {
                fprintf(output, STR_NEED_DD, GET_SUBSTANCE(settings.reactions[i].s1),
                        GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3),
                        num_rand());
                fprintf(output, STR_PROD_DD, GET_SUBSTANCE(settings.reactions[i].s1),
                        GET_SUBSTANCE(settings.reactions[i].s2), GET_SUBSTANCE(settings.reactions[i].s3),
                        num_rand());
                fprintf(output, STR_PROD_DD, GET_SUBSTANCE(settings.reactions[i].s1),
                        GET_SUBSTANCE(settings.reactions[i].s3), GET_SUBSTANCE(settings.reactions[i].s2),
                        num_rand());
            }
        }
    }


    if (settings.countsubs) {
        if (!settings.numeric) {
            fprintf(output, STR_COUNT_SUBS, 0);
            //      fprintf(output, STR_COUNT_SUBS, settings.levels_to_use);
        } else {
            fprintf(output, STR_NUM_COUNTSUBS, 0);
            //      fprintf(output, STR_MAX_NUMSUBS, settings.levels_to_use);
        }
    }



    if (!settings.numeric) {
        if (settings.countsubs || settings.quantity_levels) {
            for (i = 0; i < settings.levels_to_use; i++) {
                fprintf(output, STR_NEXT, i + 1, i);
            }
        }

        if (settings.quantity_levels) {
            for (i = 0; i < settings.num_of_simple_substances; i++) {
                if (GET_BIT(problem.simple_used, i))
                    fprintf(output, STR_LEVEL, settings.simple_substances[i], problem.simple_levels[i]);
            }

            for (i = 0; i < settings.num_of_dashed_substances; i++) {
                if (GET_BIT(problem.dashed_used, i))
                    fprintf(output, STR_LEVEL, settings.dashed_substances[i], 0);
            }
        }
    }


    if (!settings.numeric || !settings.durative) {
        fprintf(output, ")\n");

        fprintf(output, LBL_GOAL);
        fprintf(output, AND);
    }

    if (!settings.dj_goals) {
        for (i = 0; i < problem.num_last_dashed; i++) {
            if (!settings.numeric) {
                if (!settings.simple_preference)
                    fprintf(output, STR_AVAIL, GET_SUBSTANCE(problem.last_dashed[i]));
                else if (settings.simple_preference) {
                    used_pref[0]++;
                    symb = pref_symb(0, used_pref[0]);

                    fprintf(output, STR_PREF_AVAIL,
                            symb,
                            GET_SUBSTANCE(problem.last_dashed[i]));
                }
            } else
                fprintf(output, STR_NUM_GOAL, GET_SUBSTANCE(problem.last_dashed[i]), num_rand());
        }
    } else {
        if (!settings.numeric && !settings.simple_preference) {
            for (i = 0, g = 0; i < problem.num_last_dashed; i += 2, g++) {
                if (!dw_style) {
                    if (!settings.durative)
                        printf("\n\n(:action DUMMY-ACTION-%d", g + 1);
                    else
                        printf("\n\n(:durative-action DUMMY-ACTION-%d", g + 1);

                    printf("\n :parameters ()");

                    if (!settings.durative)
                        printf("\n :precondition");
                    else {
                        printf("\n :duration (= ?duration %.1f)", DUMMY_ACTION_DURATION);
                        printf("\n :condition");
                    }

                    if (!settings.durative) {
                        printf(STR_DJ_AVAIL, GET_SUBSTANCE(problem.last_dashed[i]), GET_SUBSTANCE(problem.last_dashed[i + 1]));
                    } else {
                        printf(STR_DDJ_AVAIL, GET_SUBSTANCE(problem.last_dashed[i]), GET_SUBSTANCE(problem.last_dashed[i + 1]));
                    }

                    if (!settings.durative)
                        printf("\n :effect (and ");
                    else
                        printf("\n :effect (and (at end ");

                    printf(STR_GOAL, g + 1);

                    if (!settings.durative)
                        printf("))");
                    else
                        printf(")))");
                }
            }

            //printf("\n\nGOALS:");
            for (i = 0; i < g; i++) {
                //printf("\n\t");
                //printf(STR_GOAL, i);
                fprintf(output, "\n\t");
                fprintf(output, STR_GOAL, i + 1);
            }
        } else if (settings.numeric) {
            if (settings.simple_preference) {
                for (i = 0, num_goals = 0; i < problem.num_last_dashed; i += 2, num_goals++) {
                    used_pref[0]++;
                    symb = pref_symb(0, used_pref[0]);

                    sprintf(String[num_goals], STR_PREF_NUM_DJ_GOAL, symb,
                            GET_SUBSTANCE(problem.last_dashed[i]),
                            GET_SUBSTANCE(problem.last_dashed[i + 1]),
                            num_rand() + num_rand());

                    /**
                    fprintf(output, STR_PREF_NUM_DJ_GOAL, symb,
                        GET_SUBSTANCE(problem.last_dashed[i]),
                        GET_SUBSTANCE(problem.last_dashed[i+1]),
                        num_rand() + num_rand());
                    **/
                }
            } else {
                for (i = 0, num_goals = 0; i < problem.num_last_dashed; i += 2, num_goals++) {
                    sprintf(String[num_goals], STR_NUM_DJ_GOAL,
                            GET_SUBSTANCE(problem.last_dashed[i]),
                            GET_SUBSTANCE(problem.last_dashed[i + 1]),
                            num_rand() + num_rand());
                    /**
                    fprintf(output, STR_NUM_DJ_GOAL,
                        GET_SUBSTANCE(problem.last_dashed[i]),
                        GET_SUBSTANCE(problem.last_dashed[i+1]),
                        num_rand() + num_rand());
                    **/
                }
            }
        } else if (settings.simple_preference) {
            for (i = 0; i < problem.num_last_dashed; i += 2) {
                used_pref[0]++;
                symb = pref_symb(0, used_pref[0]);

                fprintf(output, STR_PREF_DJ_GOAL, symb, GET_SUBSTANCE(problem.last_dashed[i]),
                        GET_SUBSTANCE(problem.last_dashed[i + 1]));
            }

            for (i = 0; i < settings.levels_to_use; i++) {
                used_pref[i + 1]++;
                symb = pref_symb(i + 1, used_pref[i + 1]);

                fprintf(output, STR_PREF_NUMSUBS, symb, i + 1);
            }
        }
    }


    if (!settings.numeric || !settings.durative)
        fprintf(output, "))\n");



    if (settings.durative) {
        for (i = 0; i < settings.num_of_reactions; i++) {
            if (GET_BIT(problem.applied, i)) {
                switch (settings.reactions[i].type) {
                case PLUS_TO_PLUS_CAT:
                    fprintf(output, DUR_PCCAT, GET_SUBSTANCE(settings.reactions[i].s1),
                            GET_SUBSTANCE(settings.reactions[i].s2),
                            GET_SUBSTANCE(settings.reactions[i].s3),
                            2 + (0.4 - fnum_rand() / 5));

                    break;


                case PLUS_CAT:
                    fprintf(output, DUR_PCAT, GET_SUBSTANCE(settings.reactions[i].s1),
                            GET_SUBSTANCE(settings.reactions[i].s2),
                            4 + (0.8 - fnum_rand() / 2.5));
                    break;

                case PLUS_TO_DASH:

                    fprintf(output, DUR_PC, GET_SUBSTANCE(settings.reactions[i].s1),
                            GET_SUBSTANCE(settings.reactions[i].s2),
                            GET_SUBSTANCE(settings.reactions[i].s3),
                            1 + (0.2 - fnum_rand() / 10));
                    break;

                case DASH_TO_PLUS:

                    fprintf(output, DUR_DD,
                            GET_SUBSTANCE(settings.reactions[i].s1),
                            GET_SUBSTANCE(settings.reactions[i].s2),
                            GET_SUBSTANCE(settings.reactions[i].s3),
                            1 + (0.2 - fnum_rand() / 10));
                    break;
                }
            }
        }


        fprintf(output, ")");

        fprintf(output, LBL_GOAL);
        fprintf(output, AND);

        for (i = 0; i < num_goals; i++) {
            fprintf(output, String[i]);
        }

        fprintf(output, "))\n");
    }


    if (settings.constraints_pref)
        print_constraints_preferences(output);

    if (settings.numeric) {
        if (!settings.simple_preference)
            fprintf(output, STR_METRIC_USED, settings.levels_to_use);
        else { // complex-preference version
            int tmp = 0.0, tot = 0.0;

            fprintf(output, STR_METRIC_FOR_PREF);

            first = TRUE;
            for (i = 0; i < MAX_PREFERENCES; i++) {
                if (used_pref[i] <= 0)
                    continue;

                for (j = 0; j < used_pref[i]; j++) {
                    if (!first)
                        fprintf(output, "\n\t\t     ");

                    if (i > 0)
                        fprintf(output, "(* %d (is-violated %s))", i, pref_symb(i, j + 1));
                    else {
                        float cost;

                        if (settings.numeric) {
                            tmp = random() % 5 + 1;
                            if (j < used_pref[i] - 1)
                                cost = (float)(settings.levels_to_use + extra_weight + (2.5 - tmp)) / (problem.num_last_dashed / 2);
                            else
                                cost = (float)(settings.levels_to_use + extra_weight + (tot - 2.5 * (used_pref[i] - 1))) / (problem.num_last_dashed / 2);
                        } else
                            cost = (float)(settings.levels_to_use + extra_weight + (random() % 5 + 1)) / (problem.num_last_dashed / 2);

                        fprintf(output, "(* %.1f (is-violated %s))", cost, pref_symb(i, j + 1));
                    }

                    tot += tmp;

                    first = FALSE;
                }
            }

            if (settings.numeric)
                fprintf(output, "\n\t\t     (* %.2f (total-time))", 1 / coeff);

            fprintf(output, "\n\t\t     (num-subs)))\n");
        }
    } else if (settings.simple_preference) {
        int tmp = 0.0, tot = 0.0;

        fprintf(output, STR_METRIC_FOR_PREF);

        first = TRUE;
        for (i = 0; i < MAX_PREFERENCES; i++) {
            if (used_pref[i] <= 0)
                continue;

            for (j = 0; j < used_pref[i]; j++) {
                if (!first)
                    fprintf(output, "\n\t\t     ");

                if (i > 0)
                    fprintf(output, "(* %d (is-violated %s))", i, pref_symb(i, j + 1));
                else {
                    float cost;

                    if (settings.numeric) {
                        tmp = random() % 5 + 1;
                        if (j < used_pref[i] - 1)
                            cost = (float)(settings.levels_to_use + extra_weight + (2.5 - tmp)) / (problem.num_last_dashed / 2);
                        else
                            cost = (float)(settings.levels_to_use + extra_weight + (tot - 2.5 * (used_pref[i] - 1))) / (problem.num_last_dashed / 2);
                    } else
                        cost = (float)(settings.levels_to_use + extra_weight + (random() % 5 + 1)) / (problem.num_last_dashed / 2);

                    fprintf(output, "(* %.1f (is-violated %s))", cost, pref_symb(i, j + 1));
                }

                tot += tmp;

                first = FALSE;
            }
        }

        fprintf(output, "))\n");
    }


    fprintf(output, "\n)");

    fprintf(output, "\n");

    if (settings.verbose) {
        un_s = 0;
        printf("\n\nUnavailable substances");
        for (i = 0; i < settings.num_of_dashed_substances; i++) {
            if (!GET_BIT(problem.dashed_available, i)) {
                un_s++;
                printf("\n%s", settings.dashed_substances[i]);
            }
        }

        printf("\n\nNumber of unapplied reactions: %d", un_r);
        printf("\nNumber of unavailable substances: %d", un_s);
    }
}


int main(int argc, char *argv[]) {
    uno = 1.0;
    cinque = 5.0;
    dieci = 10.0;

    memset(used_pref, 0, MAX_PREFERENCES * sizeof(Bool));
    extra_weight = 0;

    parse_command_line(argc, argv);
    parse_objects();
    parse_reactions();

    if (settings.verbose)
        print_all();

    build_problem();
    print_pddl_problem(settings.problem_file, FALSE);

    if (settings.dw_out)
        print_pddl_problem(settings.dw_problem_file, TRUE);

    printf("\n");

    return 0;
}
