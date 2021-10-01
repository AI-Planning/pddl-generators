


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
 * File: inst_final.c
 * Description: final domain representation functions
 *
 *
 * Author: Joerg Hoffmann 2000
 *
 *********************************************************************/ 









#include "ff.h"

#include "output.h"
#include "memory.h"

#include "inst_pre.h"
#include "inst_final.h"














/********************************
 * POSSIBLY TRUE FACTS ANALYSIS *
 ********************************/








/* local globals for this part
 */

int_pointer lpos[MAX_PREDICATES];
int_pointer lneg[MAX_PREDICATES];
int_pointer luse[MAX_PREDICATES];
int_pointer lindex[MAX_PREDICATES];

int lp;
int largs[MAX_VARS];







void perform_reachability_analysis( void )

{

  int size, i, j, k, adr, num, pargtype;
  Bool fixpoint;
  Facts *f;
  NormOperator *no;
  EasyTemplate *t1, *t2;
  NormEffect *ne;
  Action *tmp, *a;
  Bool *had_hard_template;
  PseudoAction *pa;
  PseudoActionEffect *pae;

  gactions = NULL;
  gnum_actions = 0;

  for ( i = 0; i < gnum_predicates; i++ ) {
    size =  1;
    for ( j = 0; j < garity[i]; j++ ) {
      pargtype = gpredicates_args_type[i][j];
      size *= gtype_size[pargtype];
    }

    lpos[i] = ( int_pointer ) calloc( size, sizeof( int ) );
    lneg[i] = ( int_pointer ) calloc( size, sizeof( int ) );
    luse[i] = ( int_pointer ) calloc( size, sizeof( int ) );
    lindex[i] = ( int_pointer ) calloc( size, sizeof( int ) );

    for ( j = 0; j < size; j++ ) {
      lpos[i][j] = 0;
      lneg[i][j] = 1;/* all facts but initials are poss. negative */
      luse[i][j] = 0;
      lindex[i][j] = -1;
    }
  }

  had_hard_template = ( Bool * ) calloc( gnum_hard_templates, sizeof( Bool ) );
  for ( i = 0; i < gnum_hard_templates; i++ ) {
    had_hard_template[i] = FALSE;
  }

  /* mark initial facts as possibly positive, not poss. negative
   */
  for ( i = 0; i < gnum_predicates; i++ ) {
    lp = i;
    for ( j = 0; j < gnum_initial_predicate[i]; j++ ) {
      for ( k = 0; k < garity[i]; k++ ) {
	largs[k] = ginitial_predicate[i][j].args[k];
      }
      adr = fact_adress();
      lpos[lp][adr] = 1;
      lneg[lp][adr] = 0;
    }
  }

  /* compute fixpoint
   */
  fixpoint = FALSE;
  while ( !fixpoint ) {
    fixpoint = TRUE;

    /* assign next layer of easy templates to possibly positive fixpoint
     */
    t1 = geasy_templates;
    while ( t1 ) {
      no = t1->op;
      for ( i = 0; i < no->num_preconds; i++ ) {
	lp = no->preconds[i].predicate;
	for ( j = 0; j < garity[lp]; j++ ) {
	  largs[j] = ( no->preconds[i].args[j] >= 0 ) ?
	    no->preconds[i].args[j] : t1->inst_table[DECODE_VAR( no->preconds[i].args[j] )];
	}
	if ( !lpos[lp][fact_adress()] ) {
	  break;
	}
      }

      if ( i < no->num_preconds ) {
	t1 = t1->next;
	continue;
      }

      num = 0;
      for ( ne = no->effects; ne; ne = ne->next ) {
	num++;
	/* currently, simply ignore effect conditions and assume
	 * they will all be made true eventually.
	 */
	for ( i = 0; i < ne->num_adds; i++ ) {
	  lp = ne->adds[i].predicate;
	  for ( j = 0; j < garity[lp]; j++ ) {
	    largs[j] = ( ne->adds[i].args[j] >= 0 ) ?
	      ne->adds[i].args[j] : t1->inst_table[DECODE_VAR( ne->adds[i].args[j] )];
	  }
	  adr = fact_adress();
	  if ( !lpos[lp][adr] ) {
	    /* new relevant fact! (added non initial)
	     */
	    lpos[lp][adr] = 1;
	    lneg[lp][adr] = 1;
	    luse[lp][adr] = 1;
	    if ( gnum_relevant_facts == MAX_RELEVANT_FACTS ) {
	      printf("\ntoo many relevant facts! increase MAX_RELEVANT_FACTS (currently %d)\n\n",
		     MAX_RELEVANT_FACTS);
	      exit( 1 );
	    }
	    grelevant_facts[gnum_relevant_facts].predicate = lp;
	    for ( j = 0; j < garity[lp]; j++ ) {
	      grelevant_facts[gnum_relevant_facts].args[j] = largs[j];
	    }
	    lindex[lp][adr] = gnum_relevant_facts;
	    gnum_relevant_facts++;
	    fixpoint = FALSE;
	  }
	}
      }

      tmp = new_Action();
      tmp->norm_operator = no;
      for ( i = 0; i < no->num_vars; i++ ) {
	tmp->inst_table[i] = t1->inst_table[i];
      }
      tmp->name = no->name;
      tmp->num_name_vars = no->operator->number_of_real_params;
      make_name_inst_table_from_NormOperator( tmp, no, t1 );
      tmp->next = gactions;
      tmp->num_effects = num;
      gactions = tmp;
      gnum_actions++;

      t2 = t1->next;
      if ( t1->next ) {
	t1->next->prev = t1->prev;
      }
      if ( t1->prev ) {
	t1->prev->next = t1->next;
      } else {
	geasy_templates = t1->next;
      }
      free_single_EasyTemplate( t1 );
      t1 = t2;
    }

    /* now assign all hard templates that have not been transformed
     * to actions yet.
     */
    for ( i = 0; i < gnum_hard_templates; i++ ) {
      if ( had_hard_template[i] ) {
	continue;
      }
      pa = ghard_templates[i];

      for ( j = 0; j < pa->num_preconds; j++ ) {
	lp = pa->preconds[j].predicate;
	for ( k = 0; k < garity[lp]; k++ ) {
	  largs[k] = pa->preconds[j].args[k];
	}
	if ( !lpos[lp][fact_adress()] ) {
	  break;
	}
      }

      if ( j < pa->num_preconds ) {
	continue;
      }

      for ( pae = pa->effects; pae; pae = pae->next ) {
	/* currently, simply ignore effect conditions and assume
	 * they will all be made true eventually.
	 */
	for ( j = 0; j < pae->num_adds; j++ ) {
	  lp = pae->adds[j].predicate;
	  for ( k = 0; k < garity[lp]; k++ ) {
	    largs[k] = pae->adds[j].args[k];
	  }
	  adr = fact_adress();
	  if ( !lpos[lp][adr] ) {
	    /* new relevant fact! (added non initial)
	     */
	    lpos[lp][adr] = 1;
	    lneg[lp][adr] = 1;
	    luse[lp][adr] = 1;
	    if ( gnum_relevant_facts == MAX_RELEVANT_FACTS ) {
	      printf("\ntoo many relevant facts! increase MAX_RELEVANT_FACTS (currently %d)\n\n",
		     MAX_RELEVANT_FACTS);
	      exit( 1 );
	    }
	    grelevant_facts[gnum_relevant_facts].predicate = lp;
	    for ( k = 0; k < garity[lp]; k++ ) {
	      grelevant_facts[gnum_relevant_facts].args[k] = largs[k];
	    }
	    lindex[lp][adr] = gnum_relevant_facts;
	    gnum_relevant_facts++;
	    fixpoint = FALSE;
	  }
	}
      }

      tmp = new_Action();
      tmp->pseudo_action = pa;
      for ( j = 0; j < pa->operator->num_vars; j++ ) {
	tmp->inst_table[j] = pa->inst_table[j];
      }
      tmp->name = pa->name;
      tmp->num_name_vars = pa->operator->number_of_real_params;
      make_name_inst_table_from_PseudoAction( tmp, pa );
      tmp->next = gactions;
      tmp->num_effects = pa->num_effects;
      gactions = tmp;
      gnum_actions++;

      had_hard_template[i] = TRUE;
    }
  }

  free( had_hard_template );

  gnum_pp_facts = gnum_initial + gnum_relevant_facts;

  if ( gcmd_line.display_info == 118 ) {
    printf("\nreachability analysys came up with:");

    printf("\n\npossibly positive facts:");
    for ( f = ginitial; f; f = f->next ) {
      printf("\n");
      print_Fact( f->fact );
    }
    for ( i = 0; i < gnum_relevant_facts; i++ ) {
      printf("\n");
      print_Fact( &(grelevant_facts[i]) );
    }

    printf("\n\nthis yields these %d action templates:", gnum_actions);
    for ( i = 0; i < gnum_operators; i++ ) {
      printf("\n\noperator %s:", goperators[i]->name);
      for ( a = gactions; a; a = a->next ) {
	if ( ( a->norm_operator && 
	       a->norm_operator->operator !=  goperators[i] ) ||
	     ( a->pseudo_action &&
	       a->pseudo_action->operator !=  goperators[i] ) ) {
	  continue;
	}
	printf("\ntemplate: ");
	for ( j = 0; j < goperators[i]->number_of_real_params; j++ ) {
	  printf("%s", gconstants[a->name_inst_table[j]]);
	  if ( j < goperators[i]->num_vars-1 ) {
	    printf(" ");
	  }
	}
      }
    }
    printf("\n\n");
  }

}



/* bit complicated to avoid memory explosion when high arity predicates take
 * num_obs ^ arity space. take space for individual arg types only; 
 * must consider pred args in smallest - to - largest - type order to make
 * mapping injective.
 */
int fact_adress( void )

{

  int r = 0, b = 1, i, j, min, minj;
  Bool done[MAX_ARITY];

  for ( i = 0; i < garity[lp]; i++ ) {
    done[i] = FALSE;
  }

  for ( i = 0; i < garity[lp]; i++ ) {
    min = -1;
    minj = -1;
    for ( j = 0; j < garity[lp]; j++ ) {
      if ( !done[j] ) {
	if ( min == -1 ||
	     gtype_size[gpredicates_args_type[lp][j]] < min ) {
	  min = gtype_size[gpredicates_args_type[lp][j]];
	  minj = j;
	}
      }
    }
    if ( minj == -1 || min == -1 ) {
      printf("\n\nmin or minj not made in fact adress?\n\n");
      exit( 1 );
    }
    /* now minj is remaining arg with lowest type size min
     */
    /* need number **within type** here! */
    r += b * gmember_nr[largs[minj]][gpredicates_args_type[lp][minj]];
    b *= min;
    done[minj] = TRUE;
  }

  return r;

}



void make_name_inst_table_from_NormOperator( Action *a, NormOperator *o, EasyTemplate *t )

{

  int i, r = 0, m = 0;

  for ( i = 0; i < o->operator->number_of_real_params; i++ ) {
    if ( o->num_removed_vars > r &&
	 o->removed_vars[r] == i ) {
      /* this var has been removed in NormOp;
       * insert type constraint constant
       *
       * at least one there, as empty typed pars ops are removed
       */
      a->name_inst_table[i] = gtype_consts[o->type_removed_vars[r]][0];
      r++;
    } else {
      /* this par corresponds to par m  in NormOp
       */
      a->name_inst_table[i] = t->inst_table[m];
      m++;
    }
  }

}



void make_name_inst_table_from_PseudoAction( Action *a, PseudoAction *pa )

{

  int i;

  for ( i = 0; i < pa->operator->number_of_real_params; i++ ) {
    a->name_inst_table[i] = pa->inst_table[i];
  }

}


















/***********************************************************
 * RELEVANCE ANALYSIS AND FINAL DOMAIN AND PROBLEM CLEANUP *
 ***********************************************************/









/* counts effects for later allocation
 */
int lnum_effects;









void collect_relevant_facts( void )

{

  Action *a;
  NormOperator *no;
  NormEffect *ne;
  int i, j, adr;
  PseudoAction *pa;
  PseudoActionEffect *pae;

  /* mark all deleted facts; such facts, that are also pos, are relevant.
   */
  for ( a = gactions; a; a = a->next ) {
    if ( a->norm_operator ) {
      no = a->norm_operator;

      for ( ne = no->effects; ne; ne = ne->next ) {
	for ( i = 0; i < ne->num_dels; i++ ) {
	  lp = ne->dels[i].predicate;
	  for ( j = 0; j < garity[lp]; j++ ) {
	    largs[j] = ( ne->dels[i].args[j] >= 0 ) ?
	      ne->dels[i].args[j] : a->inst_table[DECODE_VAR( ne->dels[i].args[j] )];
	  }
	  adr = fact_adress();

	  lneg[lp][adr] = 1;
	  if ( lpos[lp][adr] &&
	       !luse[lp][adr] ) {
	    luse[lp][adr] = 1;
	    lindex[lp][adr] = gnum_relevant_facts;
	    if ( gnum_relevant_facts == MAX_RELEVANT_FACTS ) {
	      printf("\nincrease MAX_RELEVANT_FACTS! (current value: %d)\n\n",
		     MAX_RELEVANT_FACTS);
	      exit( 1 );
	    }
	    grelevant_facts[gnum_relevant_facts].predicate = lp;
	    for ( j = 0; j < garity[lp]; j++ ) {
	      grelevant_facts[gnum_relevant_facts].args[j] = largs[j];
	    }
	    lindex[lp][adr] = gnum_relevant_facts;
	    gnum_relevant_facts++;
	  }
	}
      }
    } else {
      pa = a->pseudo_action;

      for ( pae = pa->effects; pae; pae = pae->next ) {
	for ( i = 0; i < pae->num_dels; i++ ) {
	  lp = pae->dels[i].predicate;
	  for ( j = 0; j < garity[lp]; j++ ) {
	    largs[j] = pae->dels[i].args[j];
	  }
	  adr = fact_adress();

	  lneg[lp][adr] = 1;
	  if ( lpos[lp][adr] &&
	       !luse[lp][adr] ) {
	    luse[lp][adr] = 1;
	    lindex[lp][adr] = gnum_relevant_facts;
	    if ( gnum_relevant_facts == MAX_RELEVANT_FACTS ) {
	      printf("\nincrease MAX_RELEVANT_FACTS! (current value: %d)\n\n",
		     MAX_RELEVANT_FACTS);
	      exit( 1 );
	    }
	    grelevant_facts[gnum_relevant_facts].predicate = lp;
	    for ( j = 0; j < garity[lp]; j++ ) {
	      grelevant_facts[gnum_relevant_facts].args[j] = largs[j];
	    }
	    lindex[lp][adr] = gnum_relevant_facts;
	    gnum_relevant_facts++;
	  }
	}
      }
    }
  }

  if ( gcmd_line.display_info == 119 ) {
    printf("\n\nfacts selected as relevant:\n\n");
    for ( i = 0; i < gnum_relevant_facts; i++ ) {
      printf("\n%d: ", i);
      print_Fact( &(grelevant_facts[i]) );
    }
  }

  lnum_effects = 0;

  /* first make place for initial and goal states.
   * (one artificial fact might still be added here)
   */
  make_state( &ggoal_state, gnum_relevant_facts + 1 );
  ggoal_state.max_F = gnum_relevant_facts + 1;
  make_state( &ginitial_state, gnum_relevant_facts + 1 );
  ginitial_state.max_F = gnum_relevant_facts + 1;

  create_final_goal_state();
  create_final_initial_state();
  create_final_actions();

  if ( gcmd_line.display_info == 120 ) {
    printf("\n\nfinal domain representation is:\n\n");  
    for ( i = 0; i < gnum_operators; i++ ) {
      printf("\n\n------------------operator %s-----------\n\n", goperators[i]->name);
      for ( a = gactions; a; a = a->next ) {
	if ( ( !a->norm_operator &&
	       !a->pseudo_action ) ||
	     ( a->norm_operator && 
	       a->norm_operator->operator != goperators[i] ) ||
	     ( a->pseudo_action &&
	       a->pseudo_action->operator != goperators[i] ) ) {
	  continue;
	}
	print_Action( a );
      }
    }
    printf("\n\n--------------------GOAL REACHED ops-----------\n\n");
    for ( a = gactions; a; a = a->next ) {
      if ( !a->norm_operator &&
	   !a->pseudo_action ) {
	print_Action( a );
      }
    }
   
    printf("\n\nfinal initial state is:\n\n");
    for ( i = 0; i < ginitial_state.num_F; i++ ) {
      print_ft_name( ginitial_state.F[i] );
      printf("\n");
    }
    printf("\n\nfinal goal state is:\n\n");
    for ( i = 0; i < ggoal_state.num_F; i++ ) {
      print_ft_name( ggoal_state.F[i] );
      printf("\n");
    }
  }

}



void create_final_goal_state( void )

{

  WffNode *w, *ww;
  int m, i, adr;
  Action *tmp;

  set_relevants_in_wff( &ggoal );
  cleanup_wff( &ggoal );
  if ( ggoal->connective == TRU ) {
    printf("\nff: goal can be simplified to TRUE. The empty plan solves it\n\n");
    exit( 1 );
  }
  if ( ggoal->connective == FAL ) {
    printf("\nff: goal can be simplified to FALSE. No plan will solve it\n\n");
    exit( 1 );
  }

  switch ( ggoal->connective ) {
  case OR:
    if ( gnum_relevant_facts == MAX_RELEVANT_FACTS ) {
      printf("\nincrease MAX_RELEVANT_FACTS! (current value: %d)\n\n",
	     MAX_RELEVANT_FACTS);
      exit( 1 );
    }
    grelevant_facts[gnum_relevant_facts].predicate = -3;
    gnum_relevant_facts++;
    for ( w = ggoal->sons; w; w = w->next ) {
      tmp = new_Action();
      if ( w->connective == AND ) {
	m = 0;
	for ( ww = w->sons; ww; ww = ww->next ) m++;
	tmp->preconds = ( int * ) calloc( m, sizeof( int ) );
	tmp->num_preconds = 0;
	for ( ww = w->sons; ww; ww = ww->next ) {
	  lp = ww->fact->predicate;
	  for ( i = 0; i < garity[lp]; i++ ) {
	    largs[i] = ww->fact->args[i];
	  }
	  adr = fact_adress();
	  tmp->preconds[tmp->num_preconds++] = lindex[lp][adr];
	}
      } else {
	tmp->preconds = ( int * ) calloc( 1, sizeof( int ) );
	tmp->num_preconds = 1;
	lp = w->fact->predicate;
	for ( i = 0; i < garity[lp]; i++ ) {
	  largs[i] = w->fact->args[i];
	}
	adr = fact_adress();
	tmp->preconds[0] = lindex[lp][adr];
      }
      tmp->effects = ( ActionEffect * ) calloc( 1, sizeof( ActionEffect ) );
      tmp->num_effects = 1;
      tmp->effects[0].conditions = NULL;
      tmp->effects[0].num_conditions = 0;
      tmp->effects[0].dels = NULL;
      tmp->effects[0].num_dels = 0;
      tmp->effects[0].adds = ( int * ) calloc( 1, sizeof( int ) );
      tmp->effects[0].adds[0] = gnum_relevant_facts - 1;
      tmp->effects[0].num_adds = 1;
      tmp->next = gactions;
      gactions = tmp;
      gnum_actions++;
      lnum_effects++;
    }
    ggoal_state.F[0] = gnum_relevant_facts - 1;
    ggoal_state.num_F = 1;
    break;
  case AND:
    for ( w = ggoal->sons; w; w = w->next ) {
      lp = w->fact->predicate;
      for ( i = 0; i < garity[lp]; i++ ) {
	largs[i] = w->fact->args[i];
      }
      adr = fact_adress();
      ggoal_state.F[ggoal_state.num_F++] = lindex[lp][adr];
    }
    break;
  case ATOM:
    ggoal_state.num_F = 1;
    lp = ggoal->fact->predicate;
    for ( i = 0; i < garity[lp]; i++ ) {
      largs[i] = ggoal->fact->args[i];
    }
    adr = fact_adress();
    ggoal_state.F[0] = lindex[lp][adr];
    break;
  default:
    printf("\n\nwon't get here: non ATOM,AND,OR in fully simplified goal\n\n");
    exit( 1 );
  }

}



void set_relevants_in_wff( WffNode **w )

{

  WffNode *i;
  int j, adr;

  switch ( (*w)->connective ) {
  case AND:
  case OR:
    for ( i = (*w)->sons; i; i = i->next ) {
      set_relevants_in_wff( &i );
    }
    break;
  case ATOM:
    /* no equalities, as fully instantiated
     */
    lp = (*w)->fact->predicate;
    for ( j = 0; j < garity[lp]; j++ ) {
      largs[j] = (*w)->fact->args[j];
    }
    adr = fact_adress();

    if ( !lneg[lp][adr] ) {
      (*w)->connective = TRU;
      free( (*w)->fact );
      (*w)->fact = NULL;
      break;
    }
    if ( !lpos[lp][adr] ) {
      (*w)->connective = FAL;
      free( (*w)->fact );
      (*w)->fact = NULL;
      break;
    }
    break;
  default:
    printf("\n\nwon't get here: non NOT,OR,AND in goal set relevants\n\n");
    exit( 1 );
  }

}



void create_final_initial_state( void )

{

  Facts *f;
  int i, adr;

  for ( f = ginitial; f; f = f->next ) {
    lp = f->fact->predicate;
    for ( i = 0; i < garity[lp]; i++ ) {
      largs[i] = f->fact->args[i];
    }
    adr = fact_adress();

    if ( !lneg[lp][adr] ) {/* non deleted ini */
      continue;
    }

    ginitial_state.F[ginitial_state.num_F++] = lindex[lp][adr];
  }
 
}



void create_final_actions( void )

{

  Action *a, *p, *t;
  NormOperator *no;
  NormEffect *ne;
  int i, j, adr;
  PseudoAction *pa;
  PseudoActionEffect *pae;

  a = gactions; p = NULL;
  while ( a ) {
    if ( a->norm_operator ) {
      /* action comes from an easy template NormOp
       */
      no = a->norm_operator;

      if ( no->num_preconds > 0 ) {
	a->preconds = ( int * ) calloc( no->num_preconds, sizeof( int ) );
      }
      a->num_preconds = 0;
      for ( i = 0; i < no->num_preconds; i++ ) {
	lp = no->preconds[i].predicate;
	for ( j = 0; j < garity[lp]; j++ ) {
	  largs[j] = ( no->preconds[i].args[j] >= 0 ) ?
	    no->preconds[i].args[j] : a->inst_table[DECODE_VAR( no->preconds[i].args[j] )];
	}
	adr = fact_adress();
	
	/* preconds are lpos in all cases due to reachability analysis
	 */
	if ( !lneg[lp][adr] ) {
	  continue;
	}
	
	a->preconds[a->num_preconds++] = lindex[lp][adr];
      }

      if ( a->num_effects > 0 ) {
	a->effects = ( ActionEffect * ) calloc( a->num_effects, sizeof( ActionEffect ) );
      }
      a->num_effects = 0;
      for ( ne = no->effects; ne; ne = ne->next ) {
	if ( ne->num_conditions > 0 ) {
	  a->effects[a->num_effects].conditions =
	    ( int * ) calloc( ne->num_conditions, sizeof( int ) );
	}
	a->effects[a->num_effects].num_conditions = 0;

	for ( i = 0; i < ne->num_conditions; i++ ) {
	  lp = ne->conditions[i].predicate;
	  for ( j = 0; j < garity[lp]; j++ ) {
	    largs[j] = ( ne->conditions[i].args[j] >= 0 ) ?
	      ne->conditions[i].args[j] : a->inst_table[DECODE_VAR( ne->conditions[i].args[j] )];
	  }
	  adr = fact_adress();
	  if ( !lpos[lp][adr] ) {/* condition not reachable: skip effect */
	    break;
	  }
	  if ( !lneg[lp][adr] ) {/* condition always true: skip it */
	    continue;
	  }
	  a->effects[a->num_effects].conditions[a->effects[a->num_effects].num_conditions++] =
	    lindex[lp][adr];
	}

	if ( i < ne->num_conditions ) {/* found unreachable condition: free condition space */
	  free( a->effects[a->num_effects].conditions );
	  continue;
	}

	/* now create the add and del effects.
	 */
	if ( ne->num_adds > 0 ) {
	  a->effects[a->num_effects].adds = ( int * ) calloc( ne->num_adds, sizeof( int ) );
	}
	a->effects[a->num_effects].num_adds = 0;
	for ( i = 0; i < ne->num_adds; i++ ) {
	  lp = ne->adds[i].predicate;
	  for ( j = 0; j < garity[lp]; j++ ) {
	    largs[j] = ( ne->adds[i].args[j] >= 0 ) ?
	      ne->adds[i].args[j] : a->inst_table[DECODE_VAR( ne->adds[i].args[j] )];
	  }
	  adr = fact_adress();

	  if ( !lneg[lp][adr] ) {/* effect always true: skip it */
	    continue;
	  }
	  
	  a->effects[a->num_effects].adds[a->effects[a->num_effects].num_adds++] = lindex[lp][adr];
	}

	if ( ne->num_dels > 0 ) {
	  a->effects[a->num_effects].dels = ( int * ) calloc( ne->num_dels, sizeof( int ) );
	}
	a->effects[a->num_effects].num_dels = 0;
	for ( i = 0; i < ne->num_dels; i++ ) {
	  lp = ne->dels[i].predicate;
	  for ( j = 0; j < garity[lp]; j++ ) {
	    largs[j] = ( ne->dels[i].args[j] >= 0 ) ?
	      ne->dels[i].args[j] : a->inst_table[DECODE_VAR( ne->dels[i].args[j] )];
	  }
	  adr = fact_adress();

	  if ( !lpos[lp][adr] ) {/* effect always false: skip it */
	    continue;
	  }

	  a->effects[a->num_effects].dels[a->effects[a->num_effects].num_dels++] = lindex[lp][adr];
	}
	if ( i < ne->num_dels ) break;

	/* this effect is OK. go to next one in NormOp.
	 */
	a->num_effects++;
	lnum_effects++;
      }
      if ( ne ) {
	/* we get here if one effect was faulty
	 */
	if ( p ) {
	  p->next = a->next;
	  t = a;
	  a = a->next;
	  free_single_Action( t );
	} else {
	  gactions = a->next;
	  t = a;
	  a = a->next;
	  free_single_Action( t );
	}
      } else {
	p = a;
	a = a->next;
      }
      continue;
    }
    if ( a->pseudo_action ) {
      /* action is result of a PseudoAction
       */
      pa = a->pseudo_action;

      if ( pa->num_preconds > 0 ) {
	a->preconds = ( int * ) calloc( pa->num_preconds, sizeof( int ) );
      }
      a->num_preconds = 0;
      for ( i = 0; i < pa->num_preconds; i++ ) {
	lp = pa->preconds[i].predicate;
	for ( j = 0; j < garity[lp]; j++ ) {
	  largs[j] = pa->preconds[i].args[j];
	}
	adr = fact_adress();
	
	/* preconds are lpos in all cases due to reachability analysis
	 */
	if ( !lneg[lp][adr] ) {
	  continue;
	}
	
	a->preconds[a->num_preconds++] = lindex[lp][adr];
      }

      if ( a->num_effects > 0 ) {
	a->effects = ( ActionEffect * ) calloc( a->num_effects, sizeof( ActionEffect ) );
      }
      a->num_effects = 0;
      for ( pae = pa->effects; pae; pae = pae->next ) {
	if ( pae->num_conditions > 0 ) {
	  a->effects[a->num_effects].conditions =
	    ( int * ) calloc( pae->num_conditions, sizeof( int ) );
	}
	a->effects[a->num_effects].num_conditions = 0;

	for ( i = 0; i < pae->num_conditions; i++ ) {
	  lp = pae->conditions[i].predicate;
	  for ( j = 0; j < garity[lp]; j++ ) {
	    largs[j] = pae->conditions[i].args[j];
	  }
	  adr = fact_adress();

	  if ( !lpos[lp][adr] ) {/* condition not reachable: skip effect */
	    break;
	  }
	  if ( !lneg[lp][adr] ) {/* condition always true: skip it */
	    continue;
	  }
	  
	  a->effects[a->num_effects].conditions[a->effects[a->num_effects].num_conditions++] =
	    lindex[lp][adr];
	}

	if ( i < pae->num_conditions ) {/* found unreachable condition: free condition space */
	  free( a->effects[a->num_effects].conditions );
	  continue;
	}

	/* now create the add and del effects.
	 */
	if ( pae->num_adds > 0 ) {
	  a->effects[a->num_effects].adds = ( int * ) calloc( pae->num_adds, sizeof( int ) );
	}
	a->effects[a->num_effects].num_adds = 0;
	for ( i = 0; i < pae->num_adds; i++ ) {
	  lp = pae->adds[i].predicate;
	  for ( j = 0; j < garity[lp]; j++ ) {
	    largs[j] = pae->adds[i].args[j];
	  }
	  adr = fact_adress();

	  if ( !lneg[lp][adr] ) {/* effect always true: skip it */
	    continue;
	  }
	  
	  a->effects[a->num_effects].adds[a->effects[a->num_effects].num_adds++] = lindex[lp][adr];
	}

	if ( pae->num_dels > 0 ) {
	  a->effects[a->num_effects].dels = ( int * ) calloc( pae->num_dels, sizeof( int ) );
	}
	a->effects[a->num_effects].num_dels = 0;
	for ( i = 0; i < pae->num_dels; i++ ) {
	  lp = pae->dels[i].predicate;
	  for ( j = 0; j < garity[lp]; j++ ) {
	    largs[j] = pae->dels[i].args[j];
	  }
	  adr = fact_adress();

	  if ( !lpos[lp][adr] ) {/* effect always false: skip it */
	    continue;
	  }
	  
	  a->effects[a->num_effects].dels[a->effects[a->num_effects].num_dels++] = lindex[lp][adr];
	}
	if ( i < pae->num_dels ) break;

	/* this effect is OK. go to next one in PseudoAction.
	 */
	a->num_effects++;
	lnum_effects++;
      }
      if ( pae ) {
	/* we get here if one effect was faulty
	 */
	if ( p ) {
	  p->next = a->next;
	  t = a;
	  a = a->next;
	  free_single_Action( t );
	} else {
	  gactions = a->next;
	  t = a;
	  a = a->next;
	  free_single_Action( t );
	}
      } else {
	p = a;
	a = a->next;
      }
      continue;
    }/* end of if clause for PseudoAction */
    /* if action was neither normop, nor pseudo action determined,
     * then it is an artificial action due to disjunctive goal
     * conditions.
     *
     * these are already in final form.
     */
    p = a;
    a = a->next;
  }/* endfor all actions ! */

}















/**************************************************
 * CONNECTIVITY GRAPH. ULTRA CLEAN REPRESENTATION *
 **************************************************/













void build_connectivity_graph( void )

{

  int i, j, k, l, n_op, n_ef, na, nd, ef, ef_, m, l_;
  Action *a;
  int *same_effects, sn;
  Bool *had_effects;
  ActionEffect *e, *e_, *e__;

  struct timeb tp;

  ftime( &tp );
  srandom( tp.millitm );

  gnum_ft_conn = gnum_relevant_facts;
  gnum_op_conn = gnum_actions;
  gft_conn = ( FtConn * ) calloc( gnum_ft_conn, sizeof( FtConn ) );
  gop_conn = ( OpConn * ) calloc( gnum_op_conn, sizeof( OpConn ) );
  gef_conn = ( EfConn * ) calloc( lnum_effects, sizeof( EfConn ) );
  gnum_ef_conn = 0;

  same_effects = ( int * ) calloc( lnum_effects, sizeof( int ) );
  had_effects = ( Bool * ) calloc( lnum_effects, sizeof( Bool ) );

  for ( i = 0; i < gnum_ft_conn; i++ ) {
    gft_conn[i].num_PC = 0;
    gft_conn[i].num_A = 0;
    gft_conn[i].num_D = 0;

    gft_conn[i].rand = random() % BIG_INT;
  }

  for ( i = 0; i < gnum_op_conn; i++ ) {
    gop_conn[i].num_E = 0;
    gop_conn[i].num_P = 0;
  }

  for ( i = 0; i < lnum_effects; i++ ) {
    gef_conn[i].num_PC = 0;
    gef_conn[i].num_A = 0;
    gef_conn[i].num_D = 0;
    gef_conn[i].num_I = 0;

    gef_conn[i].removed = FALSE;
  }


  n_op = 0;
  n_ef = 0;
  for ( a = gactions; a; a = a->next ) {

    gop_conn[n_op].action = a;

    gop_conn[n_op].P = ( int * ) calloc( a->num_preconds, sizeof( int ) );
    for ( j = 0; j < a->num_preconds; j++ ) {
      for ( k = 0; k < gop_conn[n_op].num_P; k++ ) {
	if ( gop_conn[n_op].P[k] == a->preconds[j] ) break;
      }
      if ( k < gop_conn[n_op].num_P ) continue;
      gop_conn[n_op].P[gop_conn[n_op].num_P++] = a->preconds[j];
    }

    gop_conn[n_op].E = ( int * ) calloc( a->num_effects, sizeof( int ) );
    for ( i = 0; i < a->num_effects; i++ ) {
      had_effects[i] = FALSE;
    }
    for ( i = 0; i < a->num_effects; i++ ) {
      if ( had_effects[i] ) {
	continue;
      }
      had_effects[i] = TRUE;
      e = &(a->effects[i]);
      gop_conn[n_op].E[gop_conn[n_op].num_E++] = n_ef;
      gef_conn[n_ef].op = n_op;

      gef_conn[n_ef].PC = ( int * ) 
	calloc( e->num_conditions + a->num_preconds, sizeof( int ) );
      for ( j = 0; j < e->num_conditions; j++ ) {
	for ( k = 0; k < gef_conn[n_ef].num_PC; k++ ) {
	  if ( gef_conn[n_ef].PC[k] == e->conditions[j] ) break;
	}
	if ( k < gef_conn[n_ef].num_PC ) continue;
	gef_conn[n_ef].PC[gef_conn[n_ef].num_PC++] = e->conditions[j];
      }

      sn = 0;
      for ( j = i + 1; j < a->num_effects; j++ ) {
	if ( had_effects[j] ) {
	  continue;
	}
	e_ = &(a->effects[j]);
	/* check conditions
	 */
	for ( k = 0; k < e_->num_conditions; k++ ) {
	  for ( l = 0; l < e->num_conditions; l++ ) {
	    if ( e_->conditions[k] == e->conditions[l] ) {
	      break;
	    }
	  }
	  if ( l == e->num_conditions ) {
	    break;
	  }
	}
	if ( k < e_->num_conditions ) {
	  continue;
	}
	if ( e->num_conditions == e_->num_conditions ) {
	  same_effects[sn++] = j;
	}
      }

      na = e->num_adds;
      nd = e->num_dels;
      for ( j = 0; j < sn; j++ ) {
	na += a->effects[same_effects[j]].num_adds;
	nd += a->effects[same_effects[j]].num_dels;
      }
      gef_conn[n_ef].A = ( int * ) calloc( na, sizeof( int ) );
      gef_conn[n_ef].D = ( int * ) calloc( nd, sizeof( int ) );
      for ( j = 0; j < e->num_adds; j++ ) {
	for ( k = 0; k < gef_conn[n_ef].num_A; k++ ) {
	  if ( gef_conn[n_ef].A[k] == e->adds[j] ) break;
	}
	if ( k < gef_conn[n_ef].num_A ) continue;
	/* exclude already true adds
	 */
	for ( k = 0; k < gef_conn[n_ef].num_PC; k++ ) {
	  if ( gef_conn[n_ef].PC[k] == e->adds[j] ) break;
	}
	if ( k < gef_conn[n_ef].num_PC ) continue;
	gef_conn[n_ef].A[gef_conn[n_ef].num_A++] = e->adds[j];
      }
      for ( j = 0; j < e->num_dels; j++ ) {
	for ( k = 0; k < gef_conn[n_ef].num_D; k++ ) {
	  if ( gef_conn[n_ef].D[k] == e->dels[j] ) break;
	}
	if ( k < gef_conn[n_ef].num_D ) continue;
	/* exclude re-added dels; check against *all*
	 * adds to be integrated.
	 */
	for ( k = 0; k < e->num_adds; k++ ) {
	  if ( e->adds[k] == e->dels[j] ) break;
	}
	if ( k < e->num_adds ) continue;
	for ( l = 0; l < sn; l++ ) {
	  e_ = &(a->effects[same_effects[l]]);
	  for ( k = 0; k < e_->num_adds; k++ ) {
	    if ( e_->adds[k] == e->dels[j] ) break;
	  }
	  if ( k < e_->num_adds ) break;
	}
	if ( l < sn ) continue;
	gef_conn[n_ef].D[gef_conn[n_ef].num_D++] = e->dels[j];
      }
      for ( j = 0; j < sn; j++ ) {
	e_ = &(a->effects[same_effects[j]]);
	for ( l = 0; l < e_->num_adds; l++ ) {
	  for ( k = 0; k < gef_conn[n_ef].num_A; k++ ) {
	    if ( gef_conn[n_ef].A[k] == e_->adds[l] ) break;
	  }
	  if ( k < gef_conn[n_ef].num_A ) continue;
	  for ( k = 0; k < gef_conn[n_ef].num_PC; k++ ) {
	    if ( gef_conn[n_ef].PC[k] == e_->adds[l] ) break;
	  }
	  if ( k < gef_conn[n_ef].num_PC ) continue;
	  gef_conn[n_ef].A[gef_conn[n_ef].num_A++] = e_->adds[l];
	}
	for ( l = 0; l < e_->num_dels; l++ ) {
	  for ( k = 0; k < gef_conn[n_ef].num_D; k++ ) {
	    if ( gef_conn[n_ef].D[k] == e_->dels[l] ) break;
	  }
	  if ( k < gef_conn[n_ef].num_D ) continue;
	  /* exclude re-added dels; check against *all*
	   * adds to be integrated.
	   */
	  for ( k = 0; k < e->num_adds; k++ ) {
	    if ( e->adds[k] == e_->dels[l] ) break;
	  }
	  if ( k < e->num_adds ) continue;
	  for ( l_ = 0; l_ < sn; l_++ ) {
	    e__ = &(a->effects[same_effects[l_]]);
	    for ( k = 0; k < e__->num_adds; k++ ) {
	      if ( e__->adds[k] == e_->dels[l] ) break;
	    }
	    if ( k < e__->num_adds ) break;
	  }
	  if ( l_ < sn ) continue;
	  gef_conn[n_ef].D[gef_conn[n_ef].num_D++] = e_->dels[l];
	}
      }
      for ( j = 0; j < sn; j++ ) {
	had_effects[same_effects[j]] = TRUE;
      }
      
      n_ef++;
      gnum_ef_conn++;
    }/* ende all a->effects */


    if ( gop_conn[n_op].num_E >= 1 ) {
      /* CHECK EMPTY EFFECTS!
       *
       * two step process --- first, remove all effects that are entirely empty.
       *                      second, check if all remaining effects are illegal
       *                      or only delete:
       *                      in that case, the op will never do any good so we 
       *                      remove all its effects.
       */
      i = 0;
      while ( i < gop_conn[n_op].num_E ) {
	if ( gef_conn[gop_conn[n_op].E[i]].num_A != 0 ||
	     gef_conn[gop_conn[n_op].E[i]].num_D != 0 ) {
	  i++;
	  continue;
	}
	/* we keep it in the gef_conn (seems easier), 
	 * but mark it as removed, which will exclude it from everything.
	 */
	gef_conn[gop_conn[n_op].E[i]].removed = TRUE;
	for ( j = i; j < gop_conn[n_op].num_E - 1; j++ ) {
	  gop_conn[n_op].E[j] = gop_conn[n_op].E[j+1];
	}
	gop_conn[n_op].num_E--;
      }

      m = 0;
      for ( i = 0; i < gop_conn[n_op].num_E; i++ ) {
	if ( gef_conn[gop_conn[n_op].E[i]].num_A == 0 ) {
	  m++;
	}
      }
      if ( m == gop_conn[n_op].num_E ) {
	/* all remaining effects solely-deleters.
	 */
	for ( i = 0; i < gop_conn[n_op].num_E; i++ ) {
	  gef_conn[gop_conn[n_op].E[i]].removed = TRUE;
	}
	gop_conn[n_op].num_E = 0;
      }
    }

    /* setup implied effects info
     */
    if ( gop_conn[n_op].num_E > 1 ) {
      for ( i = 0; i < gop_conn[n_op].num_E; i++ ) {
	ef = gop_conn[n_op].E[i];
	gef_conn[ef].I = ( int * ) calloc( gop_conn[n_op].num_E, sizeof( int ) );
	gef_conn[ef].num_I = 0;
      }    
      for ( i = 0; i < gop_conn[n_op].num_E - 1; i++ ) {
	ef = gop_conn[n_op].E[i];
	for ( j = i+1; j < gop_conn[n_op].num_E; j++ ) {
	  ef_ = gop_conn[n_op].E[j];
	  /* ef ==> ef_ ? */
	  for ( k = 0; k < gef_conn[ef_].num_PC; k++ ) {
	    for ( l = 0; l < gef_conn[ef].num_PC; l++ ) {
	      if ( gef_conn[ef].PC[l] == gef_conn[ef_].PC[k] ) break;
	    }
	    if ( l == gef_conn[ef].num_PC ) break;
	  }
	  if ( k == gef_conn[ef_].num_PC ) {
	    gef_conn[ef].I[gef_conn[ef].num_I++] = ef_;
	  }
	  /* j ==> i ? */
	  for ( k = 0; k < gef_conn[ef].num_PC; k++ ) {
	    for ( l = 0; l < gef_conn[ef_].num_PC; l++ ) {
	      if ( gef_conn[ef_].PC[l] == gef_conn[ef].PC[k] ) break;
	    }
	    if ( l == gef_conn[ef_].num_PC ) break;
	  }
	  if ( k == gef_conn[ef].num_PC ) {
	    gef_conn[ef_].I[gef_conn[ef_].num_I++] = ef;
	  }
	}
      }
    }

    /* first sweep: only count the space we need for the fact arrays !
     */
    if ( gop_conn[n_op].num_E > 0 ) {
      for ( i = 0; i < gop_conn[n_op].num_E; i++ ) {
	ef = gop_conn[n_op].E[i];
	for ( j = 0; j < gef_conn[ef].num_PC; j++ ) {
	  gft_conn[gef_conn[ef].PC[j]].num_PC++;
	}
 	for ( j = 0; j < gef_conn[ef].num_A; j++ ) {
	  gft_conn[gef_conn[ef].A[j]].num_A++;
	}
	for ( j = 0; j < gef_conn[ef].num_D; j++ ) {
	  gft_conn[gef_conn[ef].D[j]].num_D++;
	}
      }
    }

    n_op++;
  }

  for ( i = 0; i < gnum_ft_conn; i++ ) {
    if ( gft_conn[i].num_PC > 0 ) {
      gft_conn[i].PC = ( int * ) calloc( gft_conn[i].num_PC, sizeof( int ) );
    }
    gft_conn[i].num_PC = 0;
    if ( gft_conn[i].num_A > 0 ) {
      gft_conn[i].A = ( int * ) calloc( gft_conn[i].num_A, sizeof( int ) );
    }
    gft_conn[i].num_A = 0;
    if ( gft_conn[i].num_D > 0 ) {
      gft_conn[i].D = ( int * ) calloc( gft_conn[i].num_D, sizeof( int ) );
    }
    gft_conn[i].num_D = 0;

    gft_conn[i].is_global_goal = FALSE;
  }
  for ( i = 0; i < ggoal_state.num_F; i++ ) {
    gft_conn[ggoal_state.F[i]].is_global_goal = TRUE;
  }

  for ( i = 0; i < gnum_ef_conn; i++ ) {
    if ( gef_conn[i].removed ) continue;
    for ( j = 0; j < gef_conn[i].num_PC; j++ ) {
      gft_conn[gef_conn[i].PC[j]].PC[gft_conn[gef_conn[i].PC[j]].num_PC++] = i;
    }
    for ( j = 0; j < gef_conn[i].num_A; j++ ) {
      gft_conn[gef_conn[i].A[j]].A[gft_conn[gef_conn[i].A[j]].num_A++] = i;
    }
    for ( j = 0; j < gef_conn[i].num_D; j++ ) {
      gft_conn[gef_conn[i].D[j]].D[gft_conn[gef_conn[i].D[j]].num_D++] = i;
    }
  }

  free( same_effects );
  free( had_effects );

  if ( gcmd_line.display_info == 121 ) {
    printf("\n\ncreated connectivity graph as follows:");

    printf("\n\n------------------OP ARRAY:-----------------------");
    for ( i = 0; i < gnum_op_conn; i++ ) {
      printf("\n\nOP: ");
      print_op_name( i );
      printf("\n----------EFFS:");
      for ( j = 0; j < gop_conn[i].num_E; j++ ) {
	printf("\neffect %d", gop_conn[i].E[j]);
      }
    }
    
    printf("\n\n-------------------EFFECT ARRAY:----------------------");
    for ( i = 0; i < gnum_ef_conn; i++ ) {
      printf("\n\neffect %d of op %d: ", i, gef_conn[i].op);
      print_op_name( gef_conn[i].op );
      if ( gef_conn[i].removed ) {
	printf(" --- REMOVED ");
	continue;
      }
      printf("\n----------PCS:");
      for ( j = 0; j < gef_conn[i].num_PC; j++ ) {
	printf("\n");
	print_ft_name( gef_conn[i].PC[j] );
      }
      printf("\n----------ADDS:");
      for ( j = 0; j < gef_conn[i].num_A; j++ ) {
	printf("\n");
	print_ft_name( gef_conn[i].A[j] );
      }
      printf("\n----------DELS:");
      for ( j = 0; j < gef_conn[i].num_D; j++ ) {
	printf("\n");
	print_ft_name( gef_conn[i].D[j] );
      }
      printf("\n----------IMPLIEDS:");
      for ( j = 0; j < gef_conn[i].num_I; j++ ) {
	printf("\nimplied effect %d of op %d: ", 
	       gef_conn[i].I[j], gef_conn[gef_conn[i].I[j]].op);
	print_op_name( gef_conn[gef_conn[i].I[j]].op );
      }
    }
    
    printf("\n\n----------------------FT ARRAY:-----------------------------");
    for ( i = 0; i < gnum_ft_conn; i++ ) {
      printf("\n\nFT: ");
      print_ft_name( i );
      printf(" rand: %d", gft_conn[i].rand);
      printf("\n----------PRE COND OF:");
      for ( j = 0; j < gft_conn[i].num_PC; j++ ) {
	printf("\neffect %d", gft_conn[i].PC[j]);
      }
      printf("\n----------ADD BY:");
      for ( j = 0; j < gft_conn[i].num_A; j++ ) {
	printf("\neffect %d", gft_conn[i].A[j]);
      }
      printf("\n----------DEL BY:");
      for ( j = 0; j < gft_conn[i].num_D; j++ ) {
	printf("\neffect %d", gft_conn[i].D[j]);
      }
    }
  }
    
}






