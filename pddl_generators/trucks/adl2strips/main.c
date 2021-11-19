

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
 * File: main.c
 * Description: The main routine for the FastForward Planner.
 *
 * Author: Joerg Hoffmann 2000
 * 
 *********************************************************************/ 








#include "ff.h"

#include "memory.h"
#include "output.h"

#include "parse.h"

#include "inst_pre.h"
#include "inst_easy.h"
#include "inst_hard.h"
#include "inst_final.h"














/*
 *  ----------------------------- GLOBAL VARIABLES ----------------------------
 */












/*******************
 * GENERAL HELPERS *
 *******************/





/* don't let this run across a memory limit... avoid workstation killing
 */
long gmemory = 0;



/* ultra clean: collect the requirements.
 */
Bool gtyping = FALSE;
Bool gnegative_preconditions = FALSE;
Bool gdisjunctive_preconditions = FALSE;
Bool gequality = FALSE;
Bool gexistential_preconditions = FALSE;
Bool guniversal_preconditions = FALSE;
Bool gquantified_preconditions = FALSE;
Bool gconditional_effects = FALSE;
Bool gadl = FALSE;






/* used to time the different stages of the planner
 */
float gtempl_time = 0, greach_time = 0, grelev_time = 0, gconn_time = 0;
float gsearch_time = 0;


/* the command line inputs
 */
struct _command_line gcmd_line;

/* number of states that got heuristically evaluated
 */
int gevaluated_states = 0;

/* maximal depth of breadth first search
 */
int gmax_search_depth = 0;





/***********
 * PARSING *
 ***********/







/* used for pddl parsing, flex only allows global variables
 */
int gbracket_count;
char *gproblem_name;

/* The current input line number
 */
int lineno = 1;

/* The current input filename
 */
char *gact_filename;

/* The pddl domain name
 */
char *gdomain_name = NULL;

/* loaded, uninstantiated operators
 */
PlOperator *gloaded_ops = NULL;

/* stores initials as fact_list 
 */
PlNode *gorig_initial_facts = NULL;

/* not yet preprocessed goal facts
 */
PlNode *gorig_goal_facts = NULL;

/* axioms as in UCPOP before being changed to ops
 */
PlOperator *gloaded_axioms = NULL;

/* the types, as defined in the domain file
 */
TypedList *gparse_types = NULL;

/* the constants, as defined in domain file
 */
TypedList *gparse_constants = NULL;

/* the predicates and their arg types, as defined in the domain file
 */
TypedListList *gparse_predicates = NULL;

/* the objects, declared in the problem file
 */
TypedList *gparse_objects = NULL;


/* connection to instantiation ( except ops, goal, initial )
 */

/* all typed objects 
 */
FactList *gorig_constant_list = NULL;

/* the predicates and their types
 */
FactList *gpredicates_and_types = NULL;












/*****************
 * INSTANTIATING *
 *****************/









/* global arrays of constant names,
 *               type names (with their constants),
 *               predicate names,
 *               predicate aritys,
 *               defined types of predicate args
 */
Token gconstants[MAX_CONSTANTS];
int gnum_constants = 0;
Token gtype_names[MAX_TYPES];
int gtype_consts[MAX_TYPES][MAX_TYPE];
Bool gis_member[MAX_CONSTANTS][MAX_TYPES];
int gmember_nr[MAX_CONSTANTS][MAX_TYPES];/* nr of object within a type */
int gtype_size[MAX_TYPES];
int gnum_types = 0;
Token gpredicates[MAX_PREDICATES];
int garity[MAX_PREDICATES];
int gpredicates_args_type[MAX_PREDICATES][MAX_ARITY];
int gnum_predicates = 0;
int gnegated_predicate_to[MAX_PREDICATES];





/* the domain in integer (Fact) representation
 */
Operator_pointer goperators[MAX_OPERATORS];
int gnum_operators = 0;
Fact *gfull_initial;
int gnum_full_initial = 0;
WffNode *ggoal = NULL;




/* stores inertia - information: is any occurence of the predicate
 * added / deleted in the uninstantiated ops ?
 */
Bool gis_added[MAX_PREDICATES];
Bool gis_deleted[MAX_PREDICATES];



/* splitted initial state:
 * initial non static facts,
 * initial static facts, divided into predicates
 * (will be two dimensional array, allocated directly before need)
 */
Facts *ginitial = NULL;
int gnum_initial = 0;
Fact **ginitial_predicate;
int *gnum_initial_predicate;



/* the type numbers corresponding to any unary inertia
 */
int gtype_to_predicate[MAX_PREDICATES];
int gpredicate_to_type[MAX_TYPES];

/* (ordered) numbers of types that new type is intersection of
 */
TypeArray gintersected_types[MAX_TYPES];
int gnum_intersected_types[MAX_TYPES];



/* splitted domain: hard n easy ops
 */
Operator_pointer *ghard_operators;
int gnum_hard_operators;
NormOperator_pointer *geasy_operators;
int gnum_easy_operators;



/* so called Templates for easy ops: possible inertia constrained
 * instantiation constants
 */
EasyTemplate *geasy_templates;
int gnum_easy_templates;



/* first step for hard ops: create mixed operators, with conjunctive
 * precondition and arbitrary effects
 */
MixedOperator *ghard_mixed_operators;
int gnum_hard_mixed_operators;



/* hard ''templates'' : pseudo actions
 */
PseudoAction_pointer *ghard_templates;
int gnum_hard_templates;



/* store the final "relevant facts"
 */
Fact grelevant_facts[MAX_RELEVANT_FACTS];
int gnum_relevant_facts = 0;
int gnum_pp_facts = 0;



/* the final actions and problem representation
 */
Action *gactions;
int gnum_actions;
State ginitial_state;
State ggoal_state;









/**********************
 * CONNECTIVITY GRAPH *
 **********************/







/* one ops (actions) array ...
 */
OpConn *gop_conn;
int gnum_op_conn;



/* one effects array ...
 */
EfConn *gef_conn;
int gnum_ef_conn;



/* one facts array.
 */
FtConn *gft_conn;
int gnum_ft_conn;









/*******************
 * SEARCHING NEEDS *
 *******************/








/* the goal state, divided into subsets
 */
State *ggoal_agenda;
int gnum_goal_agenda;



/* byproduct of fixpoint: applicable actions
 */
int *gA;
int gnum_A;



/* communication from extract 1.P. to search engines:
 * 1P action choice
 */
int *gH;
int gnum_H;



/* the effects that are considered true in relaxed plan
 */
int *gin_plan_E;
int gnum_in_plan_E;



/* always stores (current) serial plan
 */
int gplan_ops[MAX_PLAN_LENGTH];
int gnum_plan_ops = 0;



/* stores the states that the current plan goes through
 * ( for knowing where new agenda entry starts from )
 */
State gplan_states[MAX_PLAN_LENGTH + 1];








/*
 *  ----------------------------- HEADERS FOR PARSING ----------------------------
 * ( fns defined in the scan-* files )
 */







void get_fct_file_name( char *filename );
void load_ops_file( char *filename );
void load_fct_file( char *filename );


void output_grounded_SIMPLEADL_task( void );
void output_grounded_STRIPS_task( void );
void fprint_Fact( FILE *out, Fact *f );
void fprint_ft_name( FILE *out, int index );
void fprint_op_name( FILE *out, int index );
void fprint_bernhards_encoding( FILE *ops, int op, Bool conds );
void fprint_all_possibilities( FILE *ops, int op );
void recursive_fprint_all_possibilities( FILE *ops, int op,
					 int *ncEs, int nnc,
					 int *cEs, int nc,
					 int curr_ce );
void figure_out_negated_facts( void );






/*
 *  ----------------------------- MAIN ROUTINE ----------------------------
 */




struct tms lstart, lend;







int main( int argc, char *argv[] )

{

  /* resulting name for ops file
   */
  char ops_file[MAX_LENGTH] = "";
  /* same for fct file 
   */
  char fct_file[MAX_LENGTH] = "";
  
  struct tms start, end;

  times ( &lstart );


  gcmd_line.display_info = 0;

  /* command line treatment
   */
  if ( argc == 1 || ( argc == 2 && *++argv[0] == '?' ) ) {
    ff_usage();
    exit( 1 );
  }
  if ( !process_command_line( argc, argv ) ) {
    ff_usage();
    exit( 1 );
  }


  /* make file names
   */
  /* one input name missing
   */
  if ( !gcmd_line.ops_file_name || 
       !gcmd_line.fct_file_name ) {
    ff_usage();      
    exit( 1 );
  }
  /* add path info, complete file names will be stored in
   * ops_file and fct_file 
   */
  sprintf(ops_file, "%s%s", gcmd_line.path, gcmd_line.ops_file_name);
  sprintf(fct_file, "%s%s", gcmd_line.path, gcmd_line.fct_file_name);


  /* parse the input files
   */

  /* start parse & instantiation timing
   */
  times( &start );
  /* domain file (ops)
   */
  if ( gcmd_line.display_info >= 1 ) {
      printf("\nff: parsing domain file");
  } 
  /* it is important for the pddl language to define the domain before 
   * reading the problem 
   */
  load_ops_file( ops_file );
  /* problem file (facts)
   */  
  if ( gcmd_line.display_info >= 1 ) {
    printf(" ... done.\nff: parsing problem file"); 
  }
  load_fct_file( fct_file );
  if ( gcmd_line.display_info >= 1 ) {
    printf(" ... done.\n\n");
  }
    
  if ( gexistential_preconditions &&
       guniversal_preconditions ) {
    gquantified_preconditions = TRUE;
  }
  if ( gtyping &&
       gnegative_preconditions &&
       gdisjunctive_preconditions &&
       gequality &&
       gquantified_preconditions &&
       gconditional_effects ) {
    gadl = TRUE;
  }


  /* This is needed to get all types.
   */
  build_orig_constant_list();

  /* last step of parsing: see if it's an ADL domain!
   */
  if ( !make_adl_domain() ) {
    printf("\nff: this is not an ADL problem!");
    printf("\n    can't be handled by this version.\n\n");
    exit( 1 );
  }

  if ( 0 ) {
    printf("\nafter make adl"); fflush(stdout);
  }

  /* now instantiate operators;
   */


  /**************************
   * first do PREPROCESSING * 
   **************************/


  /* start by collecting all strings and thereby encoding 
   * the domain in integers.
   */
  encode_domain_in_integers();

  if ( 0 ) {
    printf("\nafter encode"); fflush(stdout);
  }

  /* inertia preprocessing, first step:
   *   - collect inertia information
   *   - split initial state into
   *        _ arrays for individual predicates
   *        - arrays for all static relations
   *        - array containing non - static relations
   */
  do_inertia_preprocessing_step_1();


  if ( 0 ) {
    printf("\nafter inertia1"); fflush(stdout);
  }

  /* normalize all PL1 formulae in domain description:
   * (goal, preconds and effect conditions)
   *   - simplify formula
   *   - expand quantifiers
   *   - NOTs down
   */
  normalize_all_wffs();

  if ( 0 ) {
    printf("\nafter normalize"); fflush(stdout);
  }

  /* translate negative preconds: introduce symmetric new predicate
   * NOT-p(..) (e.g., not-in(?ob) in briefcaseworld)
   */
  translate_negative_preconds();

  if ( 0 ) {
    printf("\nafter negations"); fflush(stdout);
  }

  /* split domain in easy (disjunction of conjunctive preconds)
   * and hard (non DNF preconds) part, to apply 
   * different instantiation algorithms
   */
  split_domain();

  if ( 0 ) {
    printf("\nafter split"); fflush(stdout);
  }


  /***********************************************
   * PREPROCESSING FINISHED                      *
   *                                             *
   * NOW MULTIPLY PARAMETERS IN EFFECTIVE MANNER *
   ***********************************************/

  build_easy_action_templates();
  if ( 0 ) {
    printf("\nafter easy templates"); fflush(stdout);
  }
  build_hard_action_templates();
  if ( 0 ) {
    printf("\nafter hard templates"); fflush(stdout);
  }

  times( &end );
  TIME( gtempl_time );

  times( &start );

  /* perform reachability analysis in terms of relaxed 
   * fixpoint
   */
  perform_reachability_analysis();

  times( &end );
  TIME( greach_time );

  times( &start );

  /* collect the relevant facts and build final domain
   * and problem representations.
   */
  collect_relevant_facts();

  times( &end );
  TIME( grelev_time );

  times( &start );

  /* now build globally accessable connectivity graph
   */
  build_connectivity_graph();

  times( &end );
  TIME( gconn_time );
  

  if ( gcmd_line.strips ) {
      /*printf("\n\nputting out grounded STRIPS task, %d facts\ninto ``domain-strips.pddl'' and ``problem-strips.pddl''", gnum_ft_conn);
    if ( gcmd_line.bernhard ) {
      printf("\nencoding of conditional effects:\naccording to Bernhard Nebel, polynomial, increase in plan length");
    } else {
      printf("\nencoding of conditional effects:\naccording to Gazen/Knoblock, exponential, NO increase in plan length");
      }*/
    fflush( stdout );
    output_grounded_STRIPS_task();
  } else {
      /*printf("\n\nputting out grounded SIMPLE-ADL task\ninto ``domain-strips.pddl'' and ``problem-strips.pddl''\n\n");*/
    fflush( stdout );
    output_grounded_SIMPLEADL_task();
  }


/*  printf("\n\n");*/
  exit( 0 );

}



void output_grounded_SIMPLEADL_task( void )

{

  FILE *fts, *ops;
  int i, j, k, ef;

  if ( (fts = fopen( "problem-strips.pddl", "w")) == NULL ) {
    printf("\n\nCannot open file problem-strips.pddl.\n\n");
    exit( 1 );
  }
  fprintf(fts, "(define (problem grounded-SIMPLE-ADL-%s)\n", gproblem_name);
  fprintf(fts, "(:domain grounded-SIMPLE-ADL-%s)\n", gdomain_name);
  if ( ginitial_state.num_F > 0 ) {
    fprintf(fts, "(:init\n");
    for ( i = 0; i < ginitial_state.num_F; i++ ) {
      fprintf(fts, "(");
      fprint_ft_name(fts, ginitial_state.F[i]);
      fprintf(fts, ")\n");
    }
    fprintf(fts, ")\n");
  }
  fprintf(fts, "(:goal\n");
  fprintf(fts, "(and\n");
  for ( i = 0; i < ggoal_state.num_F; i++ ) {
    fprintf(fts, "(");
    fprint_ft_name(fts, ggoal_state.F[i]);
    fprintf(fts, ")\n");
  }
  fprintf(fts, ")\n");
  fprintf(fts, ")\n");
  fprintf(fts, ")\n");
  fclose( fts);

  if ( (ops = fopen( "domain-strips.pddl", "w")) == NULL ) {
    printf("\n\nCannot open file domain-strips.pddl.\n\n");
    exit( 1 );
  }
  fprintf(ops, "(define (domain grounded-SIMPLE-ADL-%s)\n", gdomain_name);
  fprintf(ops, "(:requirements\n");
  fprintf(ops, ":strips\n");
  if ( gconditional_effects ) {
    fprintf(ops, ":conditional-effects\n");
  }
  fprintf(ops, ")\n");
  fprintf(ops, "(:predicates\n");
  for ( i = 0; i < gnum_ft_conn; i++ ) {
    fprintf(ops, "(");
    fprint_ft_name(ops, i);
    fprintf(ops, ")\n");
  }
  fprintf(ops, ")\n");
  for ( i = 0; i < gnum_op_conn; i++ ) {
    fprintf(ops, "(:action ");
    fprint_op_name( ops, i);
    fprintf(ops, "\n");
    fprintf(ops, ":parameters ()\n");
    if ( gop_conn[i].num_P > 0 ) {
      fprintf(ops, ":precondition\n");
      fprintf(ops, "(and\n");
      for ( j = 0; j < gop_conn[i].num_P; j++ ) {
	fprintf(ops, "(");
	fprint_ft_name(ops, gop_conn[i].P[j]);
	fprintf(ops, ")\n");
      }
      fprintf(ops,")\n");
    }
    fprintf(ops, ":effect\n");
    fprintf(ops, "(and\n");
    for ( j = 0; j < gop_conn[i].num_E; j++ ) {
      ef = gop_conn[i].E[j];
      if ( gef_conn[ef].num_PC > 0 ) {
	fprintf(ops, "(when\n");
	fprintf(ops, "(and\n");
	for ( k = 0; k < gef_conn[ef].num_PC; k++ ) {
	  fprintf(ops, "(");
	  fprint_ft_name(ops, gef_conn[ef].PC[k]);
	  fprintf(ops, ")\n");
	}
	fprintf(ops, ")\n");
	fprintf(ops, "(and\n");
      }
      for ( k = 0; k < gef_conn[ef].num_A; k++ ) {
	fprintf(ops, "(");
	fprint_ft_name(ops, gef_conn[ef].A[k]);
	fprintf(ops, ")\n");
      }
      for ( k = 0; k < gef_conn[ef].num_D; k++ ) {
	fprintf(ops, "(not (");
	fprint_ft_name(ops, gef_conn[ef].D[k]);
	fprintf(ops, "))\n");
      }
      if ( gef_conn[ef].num_PC > 0 ) {
	fprintf(ops, ")\n");
	fprintf(ops, ")\n");
      }
    }
    fprintf(ops, ")\n");
    fprintf(ops, ")\n");
  }
  fprintf(ops, ")\n");
  fclose( ops);

}



void output_grounded_STRIPS_task( void )

{

  FILE *fts, *ops;
  int i, j, ef, num = 0, numcs, maxcs;

  figure_out_negated_facts();

  if ( (ops = fopen( "domain-strips.pddl", "w")) == NULL ) {
    printf("\n\nCannot open file domain-strips.pddl.\n\n");
    exit( 1 );
  }
  fprintf(ops, "(define (domain grounded-STRIPS-%s)\n", gdomain_name);
  fprintf(ops, "(:requirements\n");
  fprintf(ops, ":strips\n");
  fprintf(ops, ")\n");
  fprintf(ops, "(:predicates\n");
  for ( i = 0; i < gnum_ft_conn; i++ ) {
    fprintf(ops, "(");
    fprint_ft_name(ops, i);
    fprintf(ops, ")\n");
  }
  if ( gcmd_line.bernhard ) {
    /* insert the artificial facts to control cond eff application
     * phases: one do-op per op with cond effs, one do-normal
     * if there is at least one such op.
     * all normal ops require do-normal, all artificial ops require do-op
     * for the respective op.
     */
    maxcs = -1;
    for ( i = 0; i < gnum_op_conn; i++ ) {
      numcs = 0;
      for ( j = 0; j < gop_conn[i].num_E; j++ ) {
	ef = gop_conn[i].E[j];
	if ( gef_conn[ef].num_PC > 0 ) {
	  numcs++;
	}
      }
      if ( numcs == 0 ) {
	continue;
      }
      num++;
      fprintf(ops, "(do-");
      fprint_op_name(ops, i);
      fprintf(ops, "-condeffs)\n");
      if ( numcs > maxcs ) {
	maxcs = numcs;
      }
    }
    if ( num > 0 ) {
      fprintf(ops, "(do-normal)\n");
    }
    if ( maxcs > 0 ) {
      for ( i = 0; i < maxcs; i++ ) {
	fprintf(ops, "(done-%d)\n", i);
      }
    }
  }
  fprintf(ops, ")\n");

  for ( i = 0; i < gnum_op_conn; i++ ) {
    if ( !gcmd_line.bernhard ) {
      fprint_all_possibilities( ops, i );
    } else {
      fprint_bernhards_encoding( ops, i, (num>0) );
    }
  }
  fprintf(ops, ")\n");
  fclose( ops);

  if ( (fts = fopen( "problem-strips.pddl", "w")) == NULL ) {
    printf("\n\nCannot open file problem-strips.pddl.\n\n");
    exit( 1 );
  }
  fprintf(fts, "(define (problem grounded-STRIPS-%s)\n", gproblem_name);
  fprintf(fts, "(:domain grounded-STRIPS-%s)\n", gdomain_name);
  if ( ginitial_state.num_F > 0 ) {
    fprintf(fts, "(:init\n");
    if ( num > 0 ) {/* can only happen if Bernhard */
      fprintf(fts, "(do-normal)\n");
    }
    for ( i = 0; i < ginitial_state.num_F; i++ ) {
      fprintf(fts, "(");
      fprint_ft_name(fts, ginitial_state.F[i]);
      fprintf(fts, ")\n");
    }
    fprintf(fts, ")\n");
  }
  fprintf(fts, "(:goal\n");
  fprintf(fts, "(and\n");
  if ( num > 0 ) {/* can only happen if Bernhard */
    fprintf(ops, "(do-normal)\n");
  }
  for ( i = 0; i < ggoal_state.num_F; i++ ) {
    fprintf(fts, "(");
    fprint_ft_name(fts, ggoal_state.F[i]);
    fprintf(fts, ")\n");
  }
  fprintf(fts, ")\n");
  fprintf(fts, ")\n");
  fprintf(fts, ")\n");
  fclose( fts);

}



void fprint_bernhards_encoding( FILE *ops, int op, Bool conds )

{

  int i, j, ef, ft, ft_;
  int *cEs, *ncEs, nc, nnc;
  int *add, *del, na, nd, ccc;

  add = ( int * ) calloc( 1000, sizeof( int ) );
  del = ( int * ) calloc( 1000, sizeof( int ) );

  /* remember the cond and uncond efs.
   */
  cEs = ( int * ) calloc( gop_conn[op].num_E, sizeof( int ) );
  ncEs = ( int * ) calloc( gop_conn[op].num_E, sizeof( int ) );
  nc = 0;
  nnc = 0;
  for ( i = 0; i < gop_conn[op].num_E; i++ ) {
    ef = gop_conn[op].E[i];
    if ( gef_conn[ef].num_PC > 0 ) {
      cEs[nc++] = ef;
    } else {
      ncEs[nnc++] = ef;
    }
  }

  /* first, the normal action. all uncond effs.
   */
  fprintf(ops, "(:action ");
  fprint_op_name( ops, op );
  fprintf(ops, "\n");
  fprintf(ops, ":parameters ()\n");

  fprintf(ops, ":precondition\n");
  fprintf(ops, "(and\n");
  if ( conds ) {
    fprintf(ops, "(do-normal)\n");
  }
  for ( i = 0; i < gop_conn[op].num_P; i++ ) {
    fprintf(ops, "(");
    fprint_ft_name(ops, gop_conn[op].P[i]);
    fprintf(ops, ")\n");
  }
  fprintf(ops,")\n");

  fprintf(ops, ":effect\n");
  fprintf(ops, "(and\n");
  if ( nc > 0 ) {
    fprintf(ops, "(not (do-normal))\n");
    fprintf(ops, "(do-");
    fprint_op_name(ops, op);
    fprintf(ops, "-condeffs)\n");
  }
  na = 0;
  for ( ef = 0; ef < nnc; ef++ ) {
    for ( i = 0; i < gef_conn[ncEs[ef]].num_A; i++ ) {
      for ( j = 0; j < na; j++ ) {
	if ( add[j] == gef_conn[ncEs[ef]].A[i] ) break;
      }
      if ( j < na ) continue;
      add[na++] = gef_conn[ncEs[ef]].A[i];
    }
  }
  for ( i = 0; i < na; i++ ) {
    fprintf(ops, "(");
    fprint_ft_name(ops, add[i]);
    fprintf(ops, ")\n");
  }
  nd = 0;
  for ( ef = 0; ef < nnc; ef++ ) {
    for ( i = 0; i < gef_conn[ncEs[ef]].num_D; i++ ) {
      for ( j = 0; j < nd; j++ ) {
	if ( del[j] == gef_conn[ncEs[ef]].D[i] ) break;
      }
      if ( j < nd ) continue;
      del[nd++] = gef_conn[ncEs[ef]].D[i];
    }
  }
  for ( i = 0; i < nd; i++ ) {
    fprintf(ops, "(not (");
    fprint_ft_name(ops, del[i]);
    fprintf(ops, "))\n");
  }
  fprintf(ops, ")\n");
  fprintf(ops, ")\n");

  if ( nc == 0 ) {
    /* no cond effs. done. this was the action itself.
     */
    return;
  }

  /* now, some actions per unconditional effect.
   */
  for ( i = 0; i < nc; i++ ) {
    ef = cEs[i];
    /* one "yes" action, using condition.
     */
    fprintf(ops, "(:action ");
    fprint_op_name( ops, op );
    fprintf(ops, "-condeff%d-yes\n", i);
    fprintf(ops, ":parameters ()\n");
    fprintf(ops, ":precondition\n");
    fprintf(ops, "(and\n");
    fprintf(ops, "(do-");
    fprint_op_name(ops, op);
    fprintf(ops, "-condeffs)\n");
    for ( j = 0; j < gef_conn[ef].num_PC; j++ ) {
      fprintf(ops, "(");
      fprint_ft_name(ops, gef_conn[ef].PC[j]);
      fprintf(ops, ")\n");
    }
    fprintf(ops,")\n");
    fprintf(ops, ":effect\n");
    fprintf(ops, "(and\n");
    fprintf(ops, "(done-%d)\n", i);
    for ( j = 0; j < gef_conn[ef].num_A; j++ ) {
      fprintf(ops, "(");
      fprint_ft_name(ops, gef_conn[ef].A[j]);
      fprintf(ops, ")\n");
    }
    for ( j = 0; j < gef_conn[ef].num_D; j++ ) {
      fprintf(ops, "(not (");
      fprint_ft_name(ops, gef_conn[ef].D[j]);
      fprintf(ops, "))\n");
    }
    fprintf(ops, ")\n");
    fprintf(ops, ")\n");
    /* one "no" action per inverted condition fact.
     */
    ccc = 0;
    for ( j = 0; j < gef_conn[ef].num_PC; j++ ) {
      ft = gef_conn[ef].PC[j];
      fprintf(ops, "(:action ");
      fprint_op_name( ops, op );
      fprintf(ops, "-condeff%d-no-%d\n", i, ccc++);
      fprintf(ops, ":parameters ()\n");
      fprintf(ops, ":precondition\n");
      fprintf(ops, "(and\n");
      fprintf(ops, "(do-");
      fprint_op_name(ops, op);
      fprintf(ops, "-condeffs)\n");
      /* inversion of ft into precondition!
       */
      ft_ = gft_conn[ft].negated_fact_to;
      if ( ft_ == -1 ) {
	printf("\n\nno negation to effcond %d present??\n\n", ft);
	exit( 1 );
      }
      fprintf(ops, "(");
      fprint_ft_name(ops, ft_);
      fprintf(ops, ")\n");
      fprintf(ops, ")\n");
      fprintf(ops, ":effect\n");
      fprintf(ops, "(and\n");
      fprintf(ops, "(done-%d)\n", i);
      fprintf(ops, ")\n");
      fprintf(ops, ")\n");
    }
  }
  /* finally, one effcond phase termination action
   */
  fprintf(ops, "(:action ");
  fprint_op_name( ops, op );
  fprintf(ops, "-endof-condeffs\n");
  fprintf(ops, ":parameters ()\n");
  fprintf(ops, ":precondition\n");
  fprintf(ops, "(and\n");
  fprintf(ops, "(do-");
  fprint_op_name(ops, op);
  fprintf(ops, "-condeffs)\n");
  for ( i = 0; i < nc; i++ ) {
    fprintf(ops, "(done-%d)\n", i);
  }
  fprintf(ops, ")\n");
  fprintf(ops, ":effect\n");
  fprintf(ops, "(and\n");
  fprintf(ops, "(do-normal)\n");
  fprintf(ops, "(not (do-");
  fprint_op_name(ops, op);
  fprintf(ops, "-condeffs))\n");
  for ( i = 0; i < nc; i++ ) {
    fprintf(ops, "(not (done-%d))\n", i);
  }
  fprintf(ops, ")\n");
  fprintf(ops, ")\n");

}





int *linout;
int *lselectedpreventcond;
int lpcount;

void fprint_all_possibilities( FILE *ops, int op )

{

  int *cEs, *ncEs, nc, nnc;
  int i, ef;

  cEs = ( int * ) calloc( gop_conn[op].num_E, sizeof( int ) );
  ncEs = ( int * ) calloc( gop_conn[op].num_E, sizeof( int ) );
  nc = 0;
  nnc = 0;

  for ( i = 0; i < gop_conn[op].num_E; i++ ) {
    ef = gop_conn[op].E[i];
    if ( gef_conn[ef].num_PC > 0 ) {
      cEs[nc++] = ef;
    } else {
      ncEs[nnc++] = ef;
    }
  }

  linout = ( int * ) calloc( nc, sizeof( int ) );
  lselectedpreventcond = ( int * ) calloc( nc, sizeof( int ) );
  lpcount = 0;
  recursive_fprint_all_possibilities( ops, op,
				      ncEs, nnc,
				      cEs, nc,
				      0 );
  free( linout );
  free( cEs );
  free( ncEs );

}



void recursive_fprint_all_possibilities( FILE *ops, int op,
					 int *ncEs, int nnc,
					 int *cEs, int nc,
					 int curr_ce )

{

  int i, j, ef, ft;

  int *pre, *add, *del, np, na, nd;


  if ( curr_ce == nc ) {
    /* print op corresponding to curr linout values;
     * first compute contents
     */
    pre = ( int * ) calloc( 1000, sizeof( int ) );
    add = ( int * ) calloc( 1000, sizeof( int ) );
    del = ( int * ) calloc( 1000, sizeof( int ) );

    np = 0;
    for ( i = 0; i < gop_conn[op].num_P; i++ ) {
      pre[np++] = gop_conn[op].P[i];
    }
    for ( ef = 0; ef < nc; ef++ ) {
      if ( linout[ef] == 1 ) {
	for ( i = 0; i < gef_conn[cEs[ef]].num_PC; i++ ) {
	  for ( j = 0; j < np; j++ ) {
	    if ( pre[j] == gef_conn[cEs[ef]].PC[i] ) break;
	  }
	  if ( j < np ) continue;
	  pre[np++] = gef_conn[cEs[ef]].PC[i];
	}
      } else {
	/* include negation of selected prevent cond.
	 */
	ft = gft_conn[lselectedpreventcond[ef]].negated_fact_to;
	if ( ft == -1 ) {
	  printf("\n\nno negation to effcond %d present??\n\n",
		 lselectedpreventcond[ef]);
	  exit( 1 );
	}
	for ( j = 0; j < np; j++ ) {
	  if ( pre[j] == ft ) break;
	}
	if ( j < np ) continue;
	pre[np++] = ft;
      }
    }

    na = 0;
    for ( ef = 0; ef < nnc; ef++ ) {
      for ( i = 0; i < gef_conn[ncEs[ef]].num_A; i++ ) {
	for ( j = 0; j < na; j++ ) {
	  if ( add[j] == gef_conn[ncEs[ef]].A[i] ) break;
	}
	if ( j < na ) continue;
	add[na++] = gef_conn[ncEs[ef]].A[i];
      }
    }
    for ( ef = 0; ef < nc; ef++ ) {
      if ( linout[ef] == 0 ) continue;
      for ( i = 0; i < gef_conn[cEs[ef]].num_A; i++ ) {
	for ( j = 0; j < na; j++ ) {
	  if ( add[j] == gef_conn[cEs[ef]].A[i] ) break;
	}
	if ( j < na ) continue;
	add[na++] = gef_conn[cEs[ef]].A[i];
      }
    }

    nd = 0;
    for ( ef = 0; ef < nnc; ef++ ) {
      for ( i = 0; i < gef_conn[ncEs[ef]].num_D; i++ ) {
	for ( j = 0; j < nd; j++ ) {
	  if ( del[j] == gef_conn[ncEs[ef]].D[i] ) break;
	}
	if ( j < nd ) continue;
	del[nd++] = gef_conn[ncEs[ef]].D[i];
      }
    }
    for ( ef = 0; ef < nc; ef++ ) {
      if ( linout[ef] == 0 ) continue;
      for ( i = 0; i < gef_conn[cEs[ef]].num_D; i++ ) {
	for ( j = 0; j < nd; j++ ) {
	  if ( del[j] == gef_conn[cEs[ef]].D[i] ) break;
	}
	if ( j < nd ) continue;
	del[nd++] = gef_conn[cEs[ef]].D[i];
      }
    }

    /* now print pre, add, del
     */

    fprintf(ops, "(:action ");
    fprint_op_name( ops, op);
    fprintf(ops, "-%d\n", lpcount);
    lpcount++;
    fprintf(ops, ":parameters ()\n");
    if ( np > 0 ) {
      fprintf(ops, ":precondition\n");
      fprintf(ops, "(and\n");
      for ( j = 0; j < np; j++ ) {
	fprintf(ops, "(");
	fprint_ft_name(ops, pre[j]);
	fprintf(ops, ")\n");
      }
      fprintf(ops,")\n");
    }
    fprintf(ops, ":effect\n");
    fprintf(ops, "(and\n");
    for ( j = 0; j < na; j++ ) {
      fprintf(ops, "(");
      fprint_ft_name(ops, add[j]);
      fprintf(ops, ")\n");
    }
    for ( j = 0; j < nd; j++ ) {
      fprintf(ops, "(not (");
      fprint_ft_name(ops, del[j]);
      fprintf(ops, "))\n");
    }
    fprintf(ops, ")\n");
    fprintf(ops, ")\n");

    free( pre );
    free( add );
    free( del );

    return;
  }

  linout[curr_ce] = 0;
  /* ef is out; invoke recursion for each possible prevent condition
   */
  for ( i = 0; i < gef_conn[cEs[curr_ce]].num_PC; i++ ) {
    lselectedpreventcond[curr_ce] = gef_conn[cEs[curr_ce]].PC[i];
    recursive_fprint_all_possibilities( ops, op,
					ncEs, nnc,
					cEs, nc,
					curr_ce + 1 );
  }

  linout[curr_ce] = 1;
  /* ef is in; only single recursion necessary. (all Cs will be included)
   */
  recursive_fprint_all_possibilities( ops, op,
				      ncEs, nnc,
				      cEs, nc,
				      curr_ce + 1 );

}



void fprint_Fact( FILE *out, Fact *f )

{

  int j;

  if ( f->predicate == -3 ) {
    fprintf(out, "GOAL-REACHED");
    return;
  }

  if ( f->predicate == -1 ||
       f->predicate == -2 ) {
    printf("\n\nEQ not implemented here.\n\n");
    exit( 1 );
  }
    
  fprintf(out, "%s-", gpredicates[f->predicate]);
  for ( j=0; j<garity[f->predicate]; j++ ) {
    if ( f->args[j] >= 0 ) {
      fprintf(out, "%s", gconstants[(f->args)[j]]);
    } else {
      fprintf(out, "x%d", DECODE_VAR( f->args[j] ));
    }
    if ( j < garity[f->predicate] - 1 ) {
      fprintf(out, "-");
    }
  }

}



void fprint_ft_name( FILE *out, int index )

{

  fprint_Fact( out, &(grelevant_facts[index]) );

}



void fprint_op_name( FILE *out, int index )

{

  int i;
  Action *a = gop_conn[index].action;

  if ( !a->norm_operator &&
       !a->pseudo_action ) {
    fprintf(out, "REACH-GOAL-%d", index);
  } else {
    fprintf(out, "%s", a->name ); 
    for ( i = 0; i < a->num_name_vars; i++ ) {
      fprintf(out, "-%s", gconstants[a->name_inst_table[i]]);
    }
  }

}



void figure_out_negated_facts( void )

{

  int i, j, k, negp;
  Fact *f, *ff;

  for ( i = 0; i < gnum_ft_conn; i++ ) {
    gft_conn[i].negated_fact_to = -1;
  }

  for ( i = 0; i < gnum_ft_conn; i++ ) {
    if ( gft_conn[i].negated_fact_to != -1 ) {
      continue;
    }
    f = &(grelevant_facts[i]);
    negp = gnegated_predicate_to[f->predicate];
    if ( negp == -1 ) {
      continue;
    }
    for ( j = 0; j < gnum_ft_conn; j++ ) {
      if ( gft_conn[j].negated_fact_to != -1 ) {
	continue;
      }
      ff = &(grelevant_facts[j]);
      if ( negp != ff->predicate ) {
	continue;
      }
      for ( k = 0; k < garity[f->predicate]; k++ ) {
	if ( f->args[k] != ff->args[k] ) {
	  break;
	}
      }
      if ( k < garity[f->predicate] ) {
	continue;
      }
      /* now ff is the inverse of f.
       */
      gft_conn[i].negated_fact_to = j;
      gft_conn[j].negated_fact_to = i;
    }
  }

  if ( 0 ) {
    for ( i = 0; i < gnum_ft_conn; i++ ) {
      if ( gft_conn[i].negated_fact_to == -1 ) continue;
      printf("\n");
      print_ft_name( i );
      printf(" is negation of ");
      print_ft_name( gft_conn[i].negated_fact_to );
    }
  }

}
















/*
 *  ----------------------------- HELPING FUNCTIONS ----------------------------
 */












void output_planner_info( void )

{

  printf( "\n\ntime spent: %7.2f seconds instantiating %d easy, %d hard action templates", 
	  gtempl_time, gnum_easy_templates, gnum_hard_mixed_operators );
  printf( "\n            %7.2f seconds reachability analysis, yielding %d facts and %d actions", 
	  greach_time, gnum_pp_facts, gnum_actions );
  printf( "\n            %7.2f seconds creating final representation with %d relevant facts", 
	  grelev_time, gnum_relevant_facts );
  printf( "\n            %7.2f seconds building connectivity graph",
	  gconn_time );
  printf( "\n            %7.2f seconds searching, evaluating %d states, to a max depth of %d", 
	  gsearch_time, gevaluated_states, gmax_search_depth );
  printf( "\n            %7.2f seconds total time", 
	  gtempl_time + greach_time + grelev_time + gconn_time + gsearch_time );

  printf("\n\n");

  exit( 0 );

}


FILE *out;

void print_official_result( void )

{

  int i;
  char name[MAX_LENGTH];

  sprintf( name, "%s.soln", gcmd_line.fct_file_name );

  if ( (out = fopen( name, "w")) == NULL ) {
    printf("\n\nCan't open official output file!\n\n");
    return;
  }

  times( &lend );
  fprintf(out, "Time %d\n", 
	 (int) ((lend.tms_utime - lstart.tms_utime + lend.tms_stime - lstart.tms_stime) * 10.0));

  for ( i = 0; i < gnum_plan_ops; i++ ) {
    print_official_op_name( gplan_ops[i] );
    fprintf(out, "\n");
  }

  fclose( out );

}



void print_official_op_name( int index )

{

  int i;
  Action *a = gop_conn[index].action;

  if ( a->norm_operator ||
       a->pseudo_action ) {
    fprintf(out, "(%s", a->name ); 
    for ( i = 0; i < a->num_name_vars; i++ ) {
      fprintf(out, " %s", gconstants[a->name_inst_table[i]]);
    }
    fprintf(out, ")");
  }

}



void ff_usage( void )

{

  printf("\nusage:\n");

  printf("\nOPTIONS   DESCRIPTIONS\n\n");
  printf("-p <str>    path for operator and fact file\n");
  printf("-o <str>    operator file name\n");
  printf("-f <str>    fact file name\n\n");

  printf("-A          output SIMPLE-ADL task instead of STRIPS\n");
  printf("-B          use Bernhard's encoding for compiling SIMPLE-ADL cond effs\n\n");



/*   printf("    109     reachability analysis results\n"); */
/*   printf("    110     final domain representation\n"); */
/*   printf("    111     connectivity graph\n"); */
/*   printf("    112     False sets of goals <GAM>\n"); */
/*   printf("    113     detected ordering constraints leq_h <GAM>\n"); */
/*   printf("    114     the Goal Agenda <GAM>\n"); */
/*   printf("    115     fixpoint result on each evaluated state <1Ph>\n"); */
/*   printf("    116     1P extracted on each evaluated state <1Ph>\n"); */
/*   printf("    117     H set collected for each evaluated state <1Ph>\n"); */
  
}



Bool process_command_line( int argc, char *argv[] )

{

  char option;

  gcmd_line.display_info = 0;
  gcmd_line.debug = 0;

  gcmd_line.strips = TRUE;
  gcmd_line.bernhard = FALSE;
  
  memset(gcmd_line.ops_file_name, 0, MAX_LENGTH);
  memset(gcmd_line.fct_file_name, 0, MAX_LENGTH);
  memset(gcmd_line.path, 0, MAX_LENGTH);

  while ( --argc && ++argv ) {
    if ( *argv[0] != '-' || strlen(*argv) != 2 ) {
      return FALSE;
    }
    option = *++argv[0];
    switch ( option ) {
    case 'A':
      gcmd_line.strips = FALSE;
      break;
    case 'B':
      gcmd_line.bernhard = TRUE;
      break;
    default:
      if ( --argc && ++argv ) {
	switch ( option ) {
	case 'p':
	  strncpy( gcmd_line.path, *argv, MAX_LENGTH );
	  break;
	case 'o':
	  strncpy( gcmd_line.ops_file_name, *argv, MAX_LENGTH );
	  break;
	case 'f':
	  strncpy( gcmd_line.fct_file_name, *argv, MAX_LENGTH );
	  break;
	case 'i':
	  sscanf( *argv, "%d", &gcmd_line.display_info );
	  break;
	case 'd':
	  sscanf( *argv, "%d", &gcmd_line.debug );
	  break;
	default:
	  printf( "\nff: unknown option: %c entered\n\n", option );
	  return FALSE;
	}
      } else {
	return FALSE;
      }
    }
  }

  return TRUE;

}

