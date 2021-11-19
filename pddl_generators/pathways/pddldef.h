#ifndef __PDDLDEF__
#define __PDDLDEF__

#define LBL_OBJECTS       "\n(:objects"
#define LBL_INIT          "\n\n(:init"
#define LBL_GOAL          "\n\n(:goal"
#define LBL_METRIC        "\n\n(:metric"

#define PROBLEM_HEADER    "(define (problem %s)"
#define DOMAIN_REF        "\n(:domain Pathways-"

#define STR_INIT_POSS     "\n\t(possible %s)"
#define STR_INIT_PC       "\n\t(association-reaction %s %s %s)"
#define STR_INIT_DD       "\n\t(decomposition-reaction %s %s %s)"
#define STR_INIT_PCCAT    "\n\t(catalyzed-association-reaction %s %s %s)"
#define STR_INIT_PCAT     "\n\t(synthesis-reaction %s %s) "
#define STR_AVAIL         "\n\t(available %s)"
#define STR_PREF_AVAIL    "\n\t(preference %s (available %s))"
#define STR_PREF_NUMSUBS  "\n\t(preference %s (not (num-subs l%d)))"

#define STR_PREF_DJ_GOAL "\n\t(preference %s (or (available %s) (available %s)))"
#define STR_DJ_AVAIL      "\n\t(or (available %s)\n\t    (available %s))"
#define STR_DDJ_AVAIL     "\n\t(at start (or (available %s)\n\t\t      (available %s)))"
#define STR_LEVEL         "\n\t(sub-level %s l%d)"
#define STR_COUNT_SUBS    "\n\t(num-subs l%d)"
#define STR_NEXT          "\n\t(next l%d l%d)"
#define STR_SIMPLE        "\n\t(simple %s)"
#define STR_DASHED        "\n\t(complex %s)"

#define STR_PROD_PC       "\n\t(= (prod-by-association %s %s %s) %d)"
#define STR_NEED_PC       "\n\t(= (need-for-association %s %s %s) %d)"
#define DUR_PC            "\n\t(= (duration-association-reaction %s %s %s) %.1f)"

#define STR_PROD_DD       "\n\t(= (prod-by-dissociation %s %s %s) %d)"
#define STR_NEED_DD       "\n\t(= (need-for-dissociation %s %s %s) %d)"
#define DUR_DD            "\n\t(= (duration-dissociation-reaction %s %s %s) %.1f)"

#define STR_PROD_PCAT     "\n\t(= (prod-by-synthesis %s %s) %d)"
#define STR_NEED_PCAT     "\n\t(= (need-for-synthesis %s %s) %d)"
#define DUR_PCAT          "\n\t(= (duration-synthesis-reaction %s %s) %.1f)"

#define STR_PROD_PCCAT    "\n\t(= (prod-by-catalyzed-association %s %s %s) %d)"
#define STR_NEED_PCCAT    "\n\t(= (need-for-catalyzed-association %s %s %s) %d)"
#define DUR_PCCAT         "\n\t(= (duration-catalyzed-association-reaction %s %s %s) %.1f)"

#define STR_NUM_AVAIL     "\n\t(= (available %s) %d)"
#define STR_NUM_GOAL      "\n\t(>= (available %s) %d)"

#define STR_NUM_DJ_GOAL      "\n\t(>= (+ (available %s) (available %s)) %d)"
#define STR_PREF_NUM_DJ_GOAL "\n\t(preference %s (>= (+ (available %s) (available %s)) %d))"

#define STR_NUM_USED      "\n\t(= (used %s) %d)"
#define STR_NUM_COUNTSUBS "\n\t(= (num-subs) %d)"
#define STR_MAX_NUMSUBS   "\n\t(= (max-num-subs) %d)"

#define STR_TOTAL         "\n\t(= (total-used) %d)"
#define STR_TOTAL_USED    "\n\t(= (num-subs) 0)"
#define STR_METRIC_USED   "\n\n(:metric minimize (+ (* 25 (- (num-subs) %d)) (total-time)))"
#define STR_METRIC_FOR_PREF "\n\n(:metric minimize (+ "

#define STR_GOAL          "(goal%d)"

#define AND               "\n\t(and"

//PREFERENCES
#define CONSTR_LBL        "\n\n(:constraints (and"

#define STR_P_ALWAYS            "\n\t(preference %s (always (not (num-subs l%d))))"
#define STR_P_ALWAYS_NOT        "\n\t(preference %s (always (not (available %s))))"
#define STR_P_ALWAYS_NOT_NUM    "\n\t(preference %s (always (= (available %s) 0)))"

#define STR_P_SOMETIME          "\n\t(preference %s (sometime (available %s)))"
#define STR_P_SOMETIME_NUM      "\n\t(preference %s (sometime (> (available %s) 0)))"

#define STR_P_SOMETIME_BEFORE     "\n\t(preference %s (sometime-before (available %s) (available %s)))"
#define STR_P_SOMETIME_BEFORE_NUM "\n\t(preference %s (sometime-before (> (available %s) 0) (> (available %s) 0)))"

#define STR_P_SOMETIME_AFTER     "\n\t(preference %s (sometime-after (available %s) (available %s)))"
#define STR_P_SOMETIME_AFTER_NUM "\n\t(preference %s (sometime-after (> (available %s) 0) (> (available %s) 0)))"


#endif
