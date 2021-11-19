
/*
 * THIS SOURCE CODE IS SUPPLIED  ``AS IS'' WITHOUT WARRANTY OF ANY KIND, 
 * AND ITS AUTHOR AND THE JOURNAL OF ARTIFICIAL INTELLIGENCE RESEARCH 
 * (JAIR) AND JAIR'S PUBLISHERS AND DISTRIBUTORS, DISCLAIM ANY AND ALL 
 * WARRANTIES, INCLUDING BUT NOT LIMITED TO ANY IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND
 * ANY WARRANTIES OR NON INFRINGEMENT.  THE USER ASSUMES ALL LIABILITY AND
 * RESPONSIBILITY FOR USE OF THIS SOURCE CODE, AND NEITHER THE AUTHOR NOR
 * JAIR, NOR JAIR'S PUBLISHERS AND DISTRIBUTORS, WILL BE LIABLE FOR 
 * DAMAGES OF ANY KIND RESULTING FROM ITS USE.  Without limiting the 
 * generality of the foregoing, neither the author, nor JAIR, nor JAIR's
 * publishers and distributors, warrant that the Source Code will be 
 * error-free, will operate without interruption, or will meet the needs 
 * of the user.
 */



/*********************************************************************
 * File: ff.h
 * Description: Types and structures for the FastForward planner.
 *
 *        --------- ADL  VERSION  v 1.0 --------------
 *
 * Author: Joerg Hoffmann 2000
 * Contact: hoffmann@informatik.uni-freiburg.de
 *
 *********************************************************************/ 








#ifndef __FF_H
#define __FF_H






#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/times.h>









/*
 *  ------------------------------------ DEFINES ----------------------------
 */











/***********************
 * MEANINGLESS HELPERS *
 ***********************/




/* strcmp returns 0 if two strings are equal, which is not nice */
#define SAME 0









/****************
 * PARSING ETC. *
 ****************/









/* various defines used in parsing
 */
#define HIDDEN_STR "#"
#define AXIOM_STR "AXIOM"
#define NAME_STR "name\0"
#define VARIABLE_STR "variable\0"
#define STANDARD_TYPE "OBJECT\0"
#define EITHER_STR "EITHER"









/***************************
 * SOME ARBITRARY SETTINGS *
 ***************************/







/* maximal string length
 */
#define MAX_LENGTH 256 


/* marks border between connected items 
 */
#define CONNECTOR "~"


/* first size of goals_at array in 1P extraction
 */
#define RELAXED_STEPS_DEFAULT 25


/* size of hash table for repeated states checking
 * during EHC breadth first search
 */
#define EHC_HASH_SIZE 8192
#define EHC_HASH_BITS 8191


/* size of hash table for repeated states checking
 * in plan construction
 */
#define PLAN_HASH_SIZE 1024
#define PLAN_HASH_BITS 1023


/* size of hash table for repeated states checking
 * during BFS search
 */
#define BFS_HASH_SIZE 65536
#define BFS_HASH_BITS 65535


/* cut random values of facts off modulo this value,
 * to make state sums fit into a single integer
 */
#define BIG_INT 1500000







/************************
 * INSTANTIATION LIMITS *
 ************************/








#define MAX_CONSTANTS 2000
#define MAX_PREDICATES 50
#define MAX_TYPES 50
#define MAX_ARITY 5
#define MAX_VARS 15


#define MAX_TYPE 2000


#define MAX_OPERATORS 10000


/* in DNF: AND with OR - sons - collect 'hitting set':
 * one son of each OR node. 
 *
 * this here is initial max number of such son s that can be collected
 * (grows dynamically, if required)
 */
#define MAX_HITTING_SET_DEFAULT 1000


#define MAX_TYPE_INTERSECTIONS 10


#define MAX_RELEVANT_FACTS 100000






/******************************************
 * DOMAIN STRUCTURE AND SEARCHING LIMITS *
 ******************************************/







#define MAX_PLAN_LENGTH 2000







/****************
 * CODE DEFINES *
 ****************/









/* not a real 'code' define; used in relax and search to encode
 * infinite level number / plan length
 */
#ifndef INFINITY
#define INFINITY -1
#endif







/* define boolean types if not allready defined
 */
#ifndef Bool
typedef unsigned char Bool;
#ifndef TRUE /* we assume that FALSE is also not defined */
#define TRUE 1
#define FALSE 0
#endif /* TRUE */
#endif /* Bool */


/* code a param number into a negative number and vice versa
 */
#define ENCODE_VAR( val ) (val * (-1)) - 1
#define DECODE_VAR( val ) (val + 1) * (-1)

#define GET_CONSTANT( val, pointer ) ( val >= 0 ) ? val : pointer->inst_table[DECODE_VAR( val )]


/* Check allocated memory
 */
#define CHECK_PTR(p) if (NULL == (p)) { \
  fprintf(stdout, "\n\aNO MEMORY in file %s:%d\n\n", __FILE__, __LINE__); \
  exit(1);}


/* add elapsed time from main local time vars to specified val
 */
#define TIME( val ) val += ( float ) ( ( end.tms_utime - start.tms_utime + \
					 end.tms_stime - start.tms_stime  ) / 100.0 )




#define MEMORY_CHECK(n) { gmemory += n; if ( (gmemory >> 20) > 10000 ) { printf("\n\npre-process out of memory (> 1 GByte)\n\n"); exit( 1 ); }}









/*
 *  ------------------------------ DATA STRUCTURES ----------------------------
 */











/*******************
 * GENERAL HELPERS *
 *******************/








/* all command switches
 */
struct _command_line {

  char path[MAX_LENGTH];
  char ops_file_name[MAX_LENGTH];
  char fct_file_name[MAX_LENGTH];
  int display_info;
  int debug;

  Bool strips;
  Bool bernhard;

};


typedef char *Token;












/***********
 * PARSING *
 ***********/










/* A list of strings
 */
typedef struct _TokenList {

  char *item;
  struct _TokenList *next;

} TokenList;



/* list of string lists
 */
typedef struct _FactList {

  TokenList *item;
  struct _FactList *next;

} FactList;



/* structure to store  typed-list-of <name>/<variable>,
 * as they are declared in PDDL files
 */
typedef struct _TypedList {

  char *name;

  /* each item in this list is the name of a type which
   * our type is the union of (EITHER - types ...)
   *
   * usually, this will default to a single-item TokenList.
   */
  TokenList *type;
  /* after first sweep, this will contain the number in type table
   */
  int n;

  struct _TypedList *next;

} TypedList;



/* only needed to parse in the predicates and their arg
 * definitions
 */
typedef struct _TypedListList {

  char *predicate;

  TypedList *args;

  struct _TypedListList *next;

} TypedListList;



/* This type indicates whether a node in the pddl tree stands for
 * an atomic expression, a junctor or a quantor. 
 */
typedef enum _Connective{TRU,
			   FAL,
			   ATOM, 
			   NOT, 
			   AND, 
			   OR, 
			   ALL, 
			   EX, 
			   WHEN} Connective;



/*
 * This is a node in the tree to parse PDDL files
 */
typedef struct _PlNode {

  /* type of the node
   */
  Connective connective;

  /* only for parsing: the var args in quantifiers
   */
  TypedList *parse_vars;

  /* AND, OR, NOT, WHEN => NULL
   * ALL, EX            => the quantified variable with its type
   * ATOM               => the atom as predicate->param1->param2->...
   */
  TokenList *atom;

  /* (a) for AND, OR this is the list of sons(a AND b AND c...),
   * (b) for the rest this is the son, e.g. a subtree that is negated
   * (c) for WHEN, the first son is the condition and the next son
   * is the effect
   */
  struct _PlNode *sons;

  /* if you have a list of sons, they are connected by next
   */
  struct _PlNode *next;

} PlNode;


/*
 * This resembles an uninstantiated PDDL operator
 */
typedef struct _PlOperator {

  char *name;

  /* only important for PDDL where :VARS may be added to the param list
   * which must be hidden when writing the plan to an output file
   */
  int number_of_real_params; 

  /* the params, as they are declared in domain file
   */
  TypedList *parse_params;

  /* params is a list of variable/type pairs, such that:
   * factlist->item = [variable] -> [type]
   */
  FactList *params;
  PlNode *preconds;
  PlNode *effects;

  struct _PlOperator *next;

} PlOperator;















/***************** 
 * INSTANTIATION *
 *****************/









/* helpers
 */

typedef int TypeArray[MAX_TYPE_INTERSECTIONS];

typedef int *int_pointer;




/* first step structures: parsing & preprocessing
 */

typedef struct _Fact {

  int predicate, args[MAX_ARITY];

} Fact;



typedef struct _Facts {

  Fact *fact;

  struct _Facts *next;

} Facts;



typedef struct _WffNode {

  Connective connective;

  /* in ALL/EX s
   */
  int var, var_type;
  char *var_name;

  /* in AND/OR s
   */
  struct _WffNode *sons;
  /* sons are doubly connected linear list
   */
  struct _WffNode *next;
  struct _WffNode *prev;

  /* in ATOMs
   */
  Fact *fact;
  /* after translation: mark NOT-p s for efficiency
   */
  int NOT_p;

  /* in ALL/EX/NOT
   */
  struct _WffNode *son;

  /* for expansion speedup
   */
  Bool visited;

  /* no WHEN s here... use Pl Connectives anyway for simplicity
   */

} WffNode, *WffNode_pointer;



typedef struct _Literal {

  Bool negated;

  Fact fact;

  struct _Literal *next;
  struct _Literal *prev;

} Literal;



typedef struct _Effect {

  int num_vars, var_types[MAX_VARS];
  char *var_names[MAX_VARS];

  WffNode *conditions;

  Literal *effects;

  struct _Effect *next;
  struct _Effect *prev;

} Effect;



typedef struct _Operator {

  char *name, *var_names[MAX_VARS];
  int number_of_real_params; 

  int num_vars, var_types[MAX_VARS];
  Bool removed[MAX_VARS];
 
  WffNode *preconds;

  Effect *effects;

  Bool hard;

} Operator, *Operator_pointer;




/* second step: structures that keep already normalized
 * operators
 */




typedef struct _NormEffect {

  int num_vars, var_types[MAX_VARS];
  int inst_table[MAX_VARS];

  Fact *conditions;
  int num_conditions;

  Fact *adds;
  int num_adds;
  Fact *dels;
  int num_dels;

  struct _NormEffect *prev;
  struct _NormEffect *next;

} NormEffect;



typedef struct _NormOperator {
  
  Operator *operator;

  char name[MAX_LENGTH];
  int num_vars, var_types[MAX_VARS];
  int inst_table[MAX_VARS];
  int removed_vars[MAX_VARS], num_removed_vars, type_removed_vars[MAX_VARS];

  Fact *preconds;
  int num_preconds;

  NormEffect *effects;

  Bool out;

} NormOperator, *NormOperator_pointer;
  


/* minimal info for a fully instantiated easy operator;
 * yields one action when expanded
 */
typedef struct _EasyTemplate {

  NormOperator *op;
  int inst_table[MAX_VARS];

  struct _EasyTemplate *prev;
  struct _EasyTemplate *next;

} EasyTemplate;






/* structures for hard ops
 */





/* intermediate step: structure for keeping hard ops
 * with normalized precondition, but arbitrary
 * effect conditions
 */
typedef struct _MixedOperator {
  
  Operator *operator;

  char name[MAX_LENGTH];
  int inst_table[MAX_VARS];

  Fact *preconds;
  int num_preconds;

  Effect *effects;

  struct _MixedOperator *next;

} MixedOperator;





/* last hard step: everything is action - like, except that
 * facts are not yet integer coded
 */  

typedef struct _PseudoActionEffect {

  Fact *conditions;
  int num_conditions;

  Fact *adds;
  int num_adds;
  Fact *dels;
  int num_dels;

  struct _PseudoActionEffect *next;

} PseudoActionEffect;



typedef struct _PseudoAction {

  Operator *operator;
  
  char *name;
  int inst_table[MAX_VARS];

  Fact *preconds;
  int num_preconds;

  PseudoActionEffect *effects;
  int num_effects;

} PseudoAction, *PseudoAction_pointer;




/* final domain representation structure
 */




typedef struct _ActionEffect {

  int *conditions;
  int num_conditions;

  int *adds;
  int num_adds;
  int *dels;
  int num_dels;

} ActionEffect;



typedef struct _Action {

  NormOperator *norm_operator;
  PseudoAction *pseudo_action;

  char *name;
  int num_name_vars;
  int name_inst_table[MAX_VARS];

  int inst_table[MAX_VARS];

  int *preconds;
  int num_preconds;

  ActionEffect *effects;
  int num_effects;

  struct _Action *next;

} Action;











/*****************************************************
 * BASIC OP AND FT STRUCTURES FOR CONNECTIVITY GRAPH *
 *****************************************************/











typedef struct _OpConn {

  /* to get name
   */
  Action *action;

  /* effects
   */
  int *E;
  int num_E;

  int *P;
  int num_P;


  /* member for applicable actions extraction
   */
  Bool is_in_A;

  /* members for 1Ph - H(S) extraction
   */
  int is_used;
  Bool is_in_H;

} OpConn;



typedef struct _EfConn {

  int op;

  /* op preconds + conds
   */
  int *PC;
  int num_PC;

  int *A;
  int num_A;

  int *D;
  int num_D;

  /* implied effects
   */
  int *I;
  int num_I;

  Bool removed;

  /* members for relaxed fixpoint computation
   */
  int level;
  Bool in_E;
  int num_active_PCs;
  Bool ch;

  /* in search: which ef is ``in plan''
   */
  Bool in_plan;

} EfConn;



typedef struct _FtConn {

  int negated_fact_to;

  /* effects it is union conds, pres element of
   */
  int *PC;
  int num_PC;

  /* efs that add or del it
   */
  int *A;
  int num_A;

  int *D;
  int num_D;

  /* members for orderings preprocessing
   */
  int *False;
  int num_False;

  /* members for relaxed fixpoint computation
   */
  int level;
  Bool in_F;

  /* members for 1Ph extraction
   */
  int is_goal;
  int is_true;
  Bool ch;

  /* search
   */
  int rand;/* for hashing */
  Bool is_global_goal;/* fast goal add finding */

} FtConn;












/****************************
 * STRUCTURES FOR SEARCHING *
 ****************************/









typedef struct _State {
  
  int *F;
  int num_F;

  int max_F;

} State, *State_pointer;



typedef struct _EhcNode {
  
  State S;

  int op;
  int depth;

  struct _EhcNode *father;
  struct _EhcNode *next;

  /* for Goal Added Deletion Heuristic:
   * number of new goal that came in into S;
   *
   * if no such goal --> == -1
   */
  int new_goal;

} EhcNode;



typedef struct _EhcHashEntry {

  int sum;

  EhcNode *ehc_node;

  struct _EhcHashEntry *next;

} EhcHashEntry, *EhcHashEntry_pointer;



typedef struct _PlanHashEntry {

  int sum;
  State S;

  /* step is number of op that is EXECUTED in S;
   * -1 means that this state is no longer contained in plan
   */
  int step;
  struct _PlanHashEntry *next_step;

  struct _PlanHashEntry *next;

} PlanHashEntry, *PlanHashEntry_pointer;



typedef struct _BfsNode {
  
  State S;

  int op;
  int h;

  struct _BfsNode *father;

  struct _BfsNode *next;
  struct _BfsNode *prev;

} BfsNode;



typedef struct _BfsHashEntry {

  int sum;

  BfsNode *bfs_node;

  struct _BfsHashEntry *next;

} BfsHashEntry, *BfsHashEntry_pointer;













/*
 *  -------------------------------- MAIN FN HEADERS ----------------------------
 */








void print_official_result( void );/* AIPS 2002 output routine */
void print_official_op_name( int index );





void output_planner_info( void );
void ff_usage( void );
Bool process_command_line( int argc, char *argv[] );









/*
 *  ----------------------------- GLOBAL VARIABLES ----------------------------
 */












/*******************
 * GENERAL HELPERS *
 *******************/










/* used to time the different stages of the planner
 */
extern float gtempl_time, greach_time, grelev_time, gconn_time;
extern float gsearch_time;

/* the command line inputs
 */
extern struct _command_line gcmd_line;

/* number of states that got heuristically evaluated
 */
extern int gevaluated_states;

/* maximal depth of breadth first search
 */
extern int gmax_search_depth;









/***********
 * PARSING *
 ***********/











/* used for pddl parsing, flex only allows global variables
 */
extern int gbracket_count;
extern char *gproblem_name;

/* The current input line number
 */
extern int lineno;

/* The current input filename
 */
extern char *gact_filename;

/* The pddl domain name
 */
extern char *gdomain_name;

/* loaded, uninstantiated operators
 */
extern PlOperator *gloaded_ops;

/* stores initials as fact_list 
 */
extern PlNode *gorig_initial_facts;

/* not yet preprocessed goal facts
 */
extern PlNode *gorig_goal_facts;

/* the types, as defined in the domain file
 */
extern TypedList *gparse_types;

/* the constants, as defined in domain file
 */
extern TypedList *gparse_constants;

/* the predicates and their arg types, as defined in the domain file
 */
extern TypedListList *gparse_predicates;

/* the objects, declared in the problem file
 */
extern TypedList *gparse_objects;



/* connection to instantiation ( except ops, goal, initial )
 */

/* all typed objects 
 */
extern FactList *gorig_constant_list;

/* the predicates and their types
 */
extern FactList *gpredicates_and_types;














/*****************
 * INSTANTIATING *
 *****************/










/* global arrays of constant names,
 *               type names (with their constants),
 *               predicate names,
 *               predicate aritys,
 *               defined types of predicate args
 */
extern Token gconstants[MAX_CONSTANTS];
extern int gnum_constants;
extern Token gtype_names[MAX_TYPES];
extern int gtype_consts[MAX_TYPES][MAX_TYPE];
extern Bool gis_member[MAX_CONSTANTS][MAX_TYPES];
extern int gmember_nr[MAX_CONSTANTS][MAX_TYPES];/* nr of object within a type */
extern int gtype_size[MAX_TYPES];
extern int gnum_types;
extern Token gpredicates[MAX_PREDICATES];
extern int garity[MAX_PREDICATES];
extern int gpredicates_args_type[MAX_PREDICATES][MAX_ARITY];
extern int gnum_predicates;
extern int gnegated_predicate_to[MAX_PREDICATES];




/* the domain in first step integer representation
 */
extern Operator_pointer goperators[MAX_OPERATORS];
extern int gnum_operators;
extern Fact *gfull_initial;
extern int gnum_full_initial;
extern WffNode *ggoal;



/* stores inertia - information: is any occurence of the predicate
 * added / deleted in the uninstantiated ops ?
 */
extern Bool gis_added[MAX_PREDICATES];
extern Bool gis_deleted[MAX_PREDICATES];



/* splitted initial state:
 * initial non static facts,
 * initial static facts, divided into predicates
 * (will be two dimensional array, allocated directly before need)
 */
extern Facts *ginitial;
extern int gnum_initial;
extern Fact **ginitial_predicate;
extern int *gnum_initial_predicate;



/* the type numbers corresponding to any unary inertia
 */
extern int gtype_to_predicate[MAX_PREDICATES];
extern int gpredicate_to_type[MAX_TYPES];

/* (ordered) numbers of types that new type is intersection of
 */
extern TypeArray gintersected_types[MAX_TYPES];
extern int gnum_intersected_types[MAX_TYPES];



/* splitted domain: hard n easy ops
 */
extern Operator_pointer *ghard_operators;
extern int gnum_hard_operators;
extern NormOperator_pointer *geasy_operators;
extern int gnum_easy_operators;



/* so called Templates for easy ops: possible inertia constrained
 * instantiation constants
 */
extern EasyTemplate *geasy_templates;
extern int gnum_easy_templates;



/* first step for hard ops: create mixed operators, with conjunctive
 * precondition and arbitrary effects
 */
extern MixedOperator *ghard_mixed_operators;
extern int gnum_hard_mixed_operators;



/* hard ''templates'' : pseudo actions
 */
extern PseudoAction_pointer *ghard_templates;
extern int gnum_hard_templates;



/* store the final "relevant facts"
 */
extern Fact grelevant_facts[MAX_RELEVANT_FACTS];
extern int gnum_relevant_facts;
extern int gnum_pp_facts;



/* the final actions and problem representation
 */
extern Action *gactions;
extern int gnum_actions;
extern State ginitial_state;
extern State ggoal_state;








/**********************
 * CONNECTIVITY GRAPH *
 **********************/





/* one ops (actions) array ...
 */
extern OpConn *gop_conn;
extern int gnum_op_conn;



/* one effects array ...
 */
extern EfConn *gef_conn;
extern int gnum_ef_conn;



/* one facts array.
 */
extern FtConn *gft_conn;
extern int gnum_ft_conn;










/*******************
 * SEARCHING NEEDS *
 *******************/







/* the goal state, divided into subsets
 */
extern State *ggoal_agenda;
extern int gnum_goal_agenda;



/* applicable actions
 */
extern int *gA;
extern int gnum_A;



/* communication from extract 1.P. to search engine:
 * 1P action choice
 */
extern int *gH;
extern int gnum_H;



/* the effects that are considered true in relaxed plan
 */
extern int *gin_plan_E;
extern int gnum_in_plan_E;



/* always stores (current) serial plan
 */
extern int gplan_ops[MAX_PLAN_LENGTH];
extern int gnum_plan_ops;



/* stores the states that the current plan goes through
 */
extern State gplan_states[MAX_PLAN_LENGTH + 1];




/* ultra clean: collect the requirements.
 */
extern Bool gtyping;
extern Bool gnegative_preconditions;
extern Bool gdisjunctive_preconditions;
extern Bool gequality;
extern Bool gexistential_preconditions;
extern Bool guniversal_preconditions;
extern Bool gquantified_preconditions;
extern Bool gconditional_effects;
extern Bool gadl;



/* don't let this run across a memory limit... avoid workstation killing
 */
extern long gmemory;



#endif __FF_H
