

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
 * File: inst_easy.c
 * Description: functions for multiplying easy operators.
 *
 *
 * Author: Joerg Hoffmann 2000
 *
 *********************************************************************/ 









#include "ff.h"

#include "output.h"
#include "memory.h"

#include "inst_pre.h"
#include "inst_easy.h" 








void build_easy_action_templates( void )

{

  int i, j;
  NormOperator *o;
  EasyTemplate *t;

  cleanup_easy_domain();

  if ( gcmd_line.display_info == 110 ) {
    printf("\n\ncleaned up easy operators are:\n");
    for ( i = 0; i < gnum_easy_operators; i++ ) {
      print_NormOperator( geasy_operators[i] );
    }
  }

  encode_easy_unaries_as_types();

  if ( gcmd_line.display_info == 111 ) {
    printf("\n\nunaries encoded easy operators are:\n");
    for ( i = 0; i < gnum_easy_operators; i++ ) {
      print_NormOperator( geasy_operators[i] );
    }
  }

  multiply_easy_effect_parameters();

  if ( gcmd_line.display_info == 112 ) {
    printf("\n\neffects multiplied easy operators are:\n");
    for ( i = 0; i < gnum_easy_operators; i++ ) {
      print_NormOperator( geasy_operators[i] );
    }
  }

  multiply_easy_op_parameters();

  if ( gcmd_line.display_info == 113 ) {
    printf("\n\ninertia free easy operators are:");
    for ( i = 0; i < gnum_easy_operators; i++ ) {
      print_NormOperator( geasy_operators[i] );
    }
    printf("\n\n");
  }

  if ( gcmd_line.display_info == 114 ) {
    printf("\n\neasy operator templates are:\n");
    
    for ( i = 0; i < gnum_easy_operators; i++ ) {
      o = geasy_operators[i];

      printf("\n\n-----------operator %s:-----------", o->operator->name);
      for ( t = geasy_templates; t; t = t->next ) {
	if ( t->op != o ) {
	  continue;
	}
	printf("\ninst: ");
	for ( j = 0; j < o->num_vars; j++ ) {
	  if ( t->inst_table[j] < 0 ) {
	    printf("\nuninstantiated param in template! debug me, please\n\n");
	    exit( 1 );
	  }
	  printf("x%d = %s", j, gconstants[t->inst_table[j]]);
	  if ( j < o->num_vars - 1 ) {
	    printf(", ");
	  }
	}
      }
    }
  }

}











/*********************************
 * EASY DOMAIN CLEANUP FUNCTIONs *
 *********************************/











void cleanup_easy_domain( void )

{

  int i, i1, i2, i3, i4, a;
  NormOperator *o;
  NormEffect *e;

  /* unused params can result from unary encoding;
   * that's why we have an extra function here
   */
  remove_unused_easy_parameters();


  /* most likely ( for sure ? ) we do not need this function call here,
   * as empty types are recognised in translation already.
   *
   * however, who knows .. ? doesn't need any real computation time anyway.
   *
   * function DOES make sense after unaries encoding, as artificial types
   * might well be empty.
   */
  handle_empty_easy_parameters();


  /* remove identical preconds and effects;
   * VERY unlikely that such will get down to here, after all
   * the formula preprocessing, but possible (?) in principle.
   * takes no computation time.
   *
   * also, remove effect conditions that are contained in the 
   * preconditions.
   */
  for ( i = 0; i < gnum_easy_operators; i++ ) {
    o = geasy_operators[i];

    i1 = 0;
    while ( i1 < o->num_preconds-1 ) {
      i2 = i1+1;
      while ( i2 < o->num_preconds ) {
	if ( identical_fact( &(o->preconds[i1]), &(o->preconds[i2]) ) ) {
	  for ( i3 = i2; i3 < o->num_preconds-1; i3++ ) {
	    o->preconds[i3].predicate = o->preconds[i3+1].predicate;
	    for ( i4 = 0; i4 < garity[o->preconds[i3].predicate]; i4++ ) {
	      o->preconds[i3].args[i4] = o->preconds[i3+1].args[i4];
	    }
	  }
	  o->num_preconds--;
	} else {
	  i2++;
	}
      }
      i1++;
    }

    for ( e = o->effects; e; e = e->next ) {

      i1 = 0;
      while ( i1 < e->num_conditions-1 ) {
	i2 = i1+1;
	while ( i2 < e->num_conditions ) {
	  if ( identical_fact( &(e->conditions[i1]), &(e->conditions[i2]) ) ) {
	    for ( i3 = i2; i3 < e->num_conditions-1; i3++ ) {
	      e->conditions[i3].predicate = e->conditions[i3+1].predicate;
	      /* here, we can still have equalities. nowhere else.
	       */
	      a = ( e->conditions[i3].predicate < 0 ) ? 
		2 : garity[e->conditions[i3].predicate];
	      for ( i4 = 0; i4 < a; i4++ ) {
		e->conditions[i3].args[i4] = e->conditions[i3+1].args[i4];
	      }
	    }
	    e->num_conditions--;
	  } else {
	    i2++;
	  }
	}
	i1++;
      }

      i1 = 0;
      while ( i1 < e->num_conditions ) {
	for ( i2 = 0; i2 < o->num_preconds; i2++ ) {
	  if ( identical_fact( &(e->conditions[i1]), &(o->preconds[i2]) ) ) {
	    break;
	  }
	}
	if ( i2 == o->num_preconds ) {
	  i1++;
	  continue;
	}
	for ( i2 = i1; i2 < e->num_conditions-1; i2++ ) {
	  e->conditions[i2].predicate = e->conditions[i2+1].predicate;
	  for ( i3 = 0; i3 < garity[e->conditions[i2].predicate]; i3++ ) {
	    e->conditions[i2].args[i3] = e->conditions[i2+1].args[i3];
	  }
	}
	e->num_conditions--;
      }  

      i1 = 0;
      while ( i1 < e->num_adds-1 ) {
	i2 = i1+1;
	while ( i2 < e->num_adds ) {
	  if ( identical_fact( &(e->adds[i1]), &(e->adds[i2]) ) ) {
	    for ( i3 = i2; i3 < e->num_adds-1; i3++ ) {
	      e->adds[i3].predicate = e->adds[i3+1].predicate;
	      for ( i4 = 0; i4 < garity[e->adds[i3].predicate]; i4++ ) {
		e->adds[i3].args[i4] = e->adds[i3+1].args[i4];
	      }
	    }
	    e->num_adds--;
	  } else {
	    i2++;
	  }
	}
	i1++;
      }

      i1 = 0;
      while ( i1 < e->num_dels-1 ) {
	i2 = i1+1;
	while ( i2 < e->num_dels ) {
	  if ( identical_fact( &(e->dels[i1]), &(e->dels[i2]) ) ) {
	    for ( i3 = i2; i3 < e->num_dels-1; i3++ ) {
	      e->dels[i3].predicate = e->dels[i3+1].predicate;
	      for ( i4 = 0; i4 < garity[e->dels[i3].predicate]; i4++ ) {
		e->dels[i3].args[i4] = e->dels[i3+1].args[i4];
	      }
	    }
	    e->num_dels--;
	  } else {
	    i2++;
	  }
	}
	i1++;
      }
    }
  }

}



Bool identical_fact( Fact *f1, Fact *f2 )

{

  int i, a;

  if ( f1->predicate != f2->predicate ) {
    return FALSE;
  }

  a = ( f1->predicate < 0 ) ? 2 : garity[f1->predicate];

  for ( i = 0; i < a; i++ ) {
    if ( f1->args[i] != f2->args[i] ) {
      return FALSE;
    }
  }

  return TRUE;

} 



void remove_unused_easy_parameters( void )

{

  int i, i1, i2, i3, a;
  NormEffect *e;
  Bool used[MAX_VARS];
  NormOperator *o;

  for ( i = 0; i < gnum_easy_operators; i++ ) {
    o = geasy_operators[i];
    for ( i1 = 0; i1 < MAX_VARS; i1++ ) {
      used[i1] = FALSE;
    }
    
    for ( i1 = 0; i1 < o->num_preconds; i1++ ) {
      for ( i2 = 0; i2 < garity[o->preconds[i1].predicate]; i2++ ) {
	if ( o->preconds[i1].args[i2] < 0 ) {
	  used[DECODE_VAR( o->preconds[i1].args[i2] )] = TRUE;
	}
      }
    }

    for ( e = o->effects; e; e = e->next ) {
      for ( i1 = 0; i1 < e->num_conditions; i1++ ) {
	a = garity[e->conditions[i1].predicate];
	for ( i2 = 0; i2 < a; i2++ ) {
	  if ( e->conditions[i1].args[i2] < 0 ) {
	    used[DECODE_VAR( e->conditions[i1].args[i2])] = TRUE;
	  }
	}
      }
      for ( i1 = 0; i1 < e->num_adds; i1++ ) {
	for ( i2 = 0; i2 < garity[e->adds[i1].predicate]; i2++ ) {
	  if ( e->adds[i1].args[i2] < 0 ) {
	    used[DECODE_VAR( e->adds[i1].args[i2])] = TRUE;
	  }
	}
      }
      for ( i1 = 0; i1 < e->num_dels; i1++ ) {
	for ( i2 = 0; i2 < garity[e->dels[i1].predicate]; i2++ ) {
	  if ( e->dels[i1].args[i2] < 0 ) {
	    used[DECODE_VAR( e->dels[i1].args[i2])] = TRUE;
	  }
	}
      }
      remove_unused_easy_effect_parameters( o, e );
    }

    i1 = 0;
    i3 = 0;
    while ( i1 < o->num_vars ) {
      if ( used[i1] ) {
	i1++;
      } else {
	o->type_removed_vars[o->num_removed_vars] = o->var_types[i1];
	for ( i2 = i1; i2 < o->num_vars-1; i2++ ) {
	  o->var_types[i2] = o->var_types[i2+1];
	  used[i2] = used[i2+1];
	}
	decrement_var_entries( o, i1 );
	o->num_vars--;
	o->removed_vars[o->num_removed_vars++] = i3;
      }
      i3++;
    }
  }

}



void decrement_var_entries( NormOperator *o, int start )

{

  int st = ENCODE_VAR( start ), i, j, a;
  NormEffect *e;

  for ( i = 0; i < o->num_preconds; i++ ) {
    for ( j = 0; j < garity[o->preconds[i].predicate]; j++ ) {
      if ( o->preconds[i].args[j] < st ) {
	o->preconds[i].args[j]++;
      }
    }
  }

  for ( e = o->effects; e; e = e->next ) {
    for ( i = 0; i < e->num_conditions; i++ ) {
      a = garity[e->conditions[i].predicate];
      for ( j = 0; j < a; j++ ) {
	if ( e->conditions[i].args[j] < st ) {
	  e->conditions[i].args[j]++;
	}
      }
    }
    for ( i = 0; i < e->num_adds; i++ ) {
      for ( j = 0; j < garity[e->adds[i].predicate]; j++ ) {
	if ( e->adds[i].args[j] < st ) {
	  e->adds[i].args[j]++;
	}
      }
    }
    for ( i = 0; i < e->num_dels; i++ ) {
      for ( j = 0; j < garity[e->dels[i].predicate]; j++ ) {
	if ( e->dels[i].args[j] < st ) {
	  e->dels[i].args[j]++;
	}
      }
    }
  }

}



void remove_unused_easy_effect_parameters( NormOperator *o, NormEffect *e )

{

  Bool used[MAX_VARS];
  int i1, i2, i, j, v, a;

  for ( i1 = 0; i1 < MAX_VARS; i1++ ) {
    used[i1] = FALSE;
  }  
  for ( i1 = 0; i1 < e->num_conditions; i1++ ) {
    a = garity[e->conditions[i1].predicate];
    for ( i2 = 0; i2 < a; i2++ ) {
      if ( e->conditions[i1].args[i2] < 0 ) {
	used[DECODE_VAR( e->conditions[i1].args[i2])] = TRUE;
      }
    }
  }
  for ( i1 = 0; i1 < e->num_adds; i1++ ) {
    for ( i2 = 0; i2 < garity[e->adds[i1].predicate]; i2++ ) {
      if ( e->adds[i1].args[i2] < 0 ) {
	used[DECODE_VAR( e->adds[i1].args[i2])] = TRUE;
      }
    }
  }
  for ( i1 = 0; i1 < e->num_dels; i1++ ) {
    for ( i2 = 0; i2 < garity[e->dels[i1].predicate]; i2++ ) {
      if ( e->dels[i1].args[i2] < 0 ) {
	used[DECODE_VAR( e->dels[i1].args[i2])] = TRUE;
      }
    }
  }
 
  i1 = 0;
  while ( i1 < e->num_vars ) {
    v = o->num_vars+i1;
    if ( used[v] ) {
      i1++;
    } else {
      for ( i2 = i1; i2 < e->num_vars-1; i2++ ) {
	e->var_types[i2] = e->var_types[i2+1];
	used[o->num_vars+i2] = used[o->num_vars+i2+1];
      }
      /* now decrement var entrys higher than o->num_vars+i1
       */
      for ( i = 0; i < e->num_conditions; i++ ) {
	a = garity[e->conditions[i].predicate];
	for ( j = 0; j < a; j++ ) {
	  if ( e->conditions[i].args[j] < ENCODE_VAR( v ) ) {
	    e->conditions[i].args[j]++;
	  }
	}
      }
      for ( i = 0; i < e->num_adds; i++ ) {
	for ( j = 0; j < garity[e->adds[i].predicate]; j++ ) {
	  if ( e->adds[i].args[j] < ENCODE_VAR( v ) ) {
	    e->adds[i].args[j]++;
	  }
	}
      }
      for ( i = 0; i < e->num_dels; i++ ) {
	for ( j = 0; j < garity[e->dels[i].predicate]; j++ ) {
	  if ( e->dels[i].args[j] < ENCODE_VAR( v ) ) {
	    e->dels[i].args[j]++;
	  }
	}
      }
      e->num_vars--;
    }
  }

}



/* this one needs ONLY be used after unaries encoding, as all empty types
 * are already recognised during translation, except the artificial ones,
 * of course.
 */
void handle_empty_easy_parameters( void )

{

  int i, j, k;
  NormOperator *o;
  NormEffect *e, *tmp;

  i = 0;
  while ( i < gnum_easy_operators ) {
    o = geasy_operators[i];

    for ( j = 0; j < o->num_vars; j++ ) {
      if ( gtype_size[o->var_types[j]] == 0 ) {
	break;
      }
    }
    if ( j < o->num_vars ) {
      free_NormOperator( o );
      for ( k = i; k < gnum_easy_operators - 1; k++ ) {
	geasy_operators[k] = geasy_operators[k+1];
      }
      gnum_easy_operators--;
    } else {
      i++;
    }
  }

  for ( i = 0; i < gnum_easy_operators; i++ ) {
    o = geasy_operators[i];
    
    e = o->effects;
    while ( e ) {
      for ( j = 0; j < e->num_vars; j++ ) {
	if ( gtype_size[e->var_types[j]] == 0 ) {
	  break;
	}
      }
      if ( j < e->num_vars ) {
	if ( e->prev ) {
	  e->prev->next = e->next;
	} else {
	  o->effects = e->next;
	}
	if ( e->next ) {
	  e->next->prev = e->prev;
	}
	tmp = e->next;
	free_single_NormEffect( e );
	e = tmp;
      } else {
	e = e->next;
      }
    }
  }

}










/****************************
 * UNARY INERTIA INTO TYPES *
 ****************************/












void encode_easy_unaries_as_types( void )

{

  NormOperator *o;
  int i1, i, j, k, l, new_T, p, a;
  TypeArray T;
  int num_T;
  NormEffect *e;
  int intersected_type, var;

  for ( i1 = 0; i1 < gnum_easy_operators; i1++ ) {
    o = geasy_operators[i1];

    for ( i = 0; i < o->num_vars; i++ ) {

      T[0] = o->var_types[i];
      num_T = 1;

      j = 0;
      while ( j < o->num_preconds ) {
	p = o->preconds[j].predicate;
	if ( ( (new_T = gtype_to_predicate[p]) != -1 ) &&
	     ( o->preconds[j].args[0] == ENCODE_VAR( i ) ) ) {
	  if ( num_T == MAX_TYPE_INTERSECTIONS ) {
	    printf("\nincrease MAX_TYPE_INTERSECTIONS (currently %d)\n\n",
		   MAX_TYPE_INTERSECTIONS);
	    exit( 1 );
	  }
	  /* insert new type number into ordered array T;
	   * ---- all type numbers in T are different:
	   *      new nr. is of inferred type - can't be type declared for param
	   *      precondition facts occur at most once - doubles are removed
	   *                                              during cleanup
	   */
	  for ( k = 0; k < num_T; k++ ) {
	    if ( new_T < T[k] ) {
	      break;
	    }
	  }
  	  for ( l = num_T; l > k; l-- ) {
	    T[l] = T[l-1];
	  }
	  T[k] = new_T;
	  num_T++;
	  /* now remove superfluous precondition
	   */
	  for ( k = j; k < o->num_preconds-1; k++ ) {
	    o->preconds[k].predicate = o->preconds[k+1].predicate;
	    for ( l = 0; l < garity[o->preconds[k].predicate]; l++ ) {
	      o->preconds[k].args[l] = o->preconds[k+1].args[l];
	    }
	  }
	  o->num_preconds--;
	} else {
	  j++;
	}
      }

      /* if we did not hit any unary inertia concerning this parameter
       * in the preconds, skip parameter and go to next one
       */
      if ( num_T == 1 ) {
	continue;
      }

      /* now we have the ordered array of types to intersect for param i 
       * of op o in array T of size num_T;
       * if there already is this intersected type, set type of this
       * param to its number, otherwise create the new intersected type.
       */
      if ( (intersected_type = find_intersected_type( T, num_T )) != -1 ) {
	/* type already there
	 */
	o->var_types[i] = intersected_type;
	continue;
      }

      /* create new type
       */
      o->var_types[i] = create_intersected_type( T, num_T );
    }

    for ( e = o->effects; e; e = e->next ) {
      for ( i = 0; i < e->num_vars; i++ ) {
	T[0] = e->var_types[i];
	var = o->num_vars + i;
	num_T = 1;
	j = 0;
	while ( j < e->num_conditions ) {
	  p = e->conditions[j].predicate;
	  if ( p < 0 ) {
	    j++;
	    continue;
	  }
	  if ( ( (new_T = gtype_to_predicate[p]) != -1 ) &&
	       ( e->conditions[j].args[0] == ENCODE_VAR( var ) ) ) {
	    if ( num_T == MAX_TYPE_INTERSECTIONS ) {
	      printf("\nincrease MAX_TYPE_INTERSECTIONS (currently %d)\n\n",
		     MAX_TYPE_INTERSECTIONS);
	      exit( 1 );
	    }
	    for ( k = 0; k < num_T; k++ ) {
	      if ( new_T < T[k] ) {
		break;
	      }
	    }
	    for ( l = num_T; l > k; l-- ) {
	      T[l] = T[l-1];
	    }
	    T[k] = new_T;
	    num_T++;
	    for ( k = j; k < e->num_conditions-1; k++ ) {
	      e->conditions[k].predicate = e->conditions[k+1].predicate;
	      a = ( e->conditions[k].predicate < 0 ) ?
		2 : garity[e->conditions[k].predicate];
	      for ( l = 0; l < a; l++ ) {
		e->conditions[k].args[l] = e->conditions[k+1].args[l];
	      }
	    }
	    e->num_conditions--;
	  } else {
	    j++;
	  }
	}
	if ( num_T == 1 ) {
	  continue;
	}
	if ( (intersected_type = find_intersected_type( T, num_T )) != -1 ) {
	  e->var_types[i] = intersected_type;
	  continue;
	}
	e->var_types[i] = create_intersected_type( T, num_T );
      }
    }
  }

  remove_unused_easy_parameters();
  handle_empty_easy_parameters();

}



int create_intersected_type( TypeArray T, int num_T )

{

  int i, j, k, intersected_type;

  if ( gnum_types == MAX_TYPES ) {
    printf("\ntoo many (inferred and intersected) types! increase MAX_TYPES (currently %d)\n\n",
	   MAX_TYPES);
    exit( 1 );
  } 
  gtype_names[gnum_types] = NULL;
  gtype_size[gnum_types] = 0;
  for ( i = 0; i < MAX_CONSTANTS; i++ ) {
    gis_member[i][gnum_types] = FALSE;
  }
  for ( i = 0; i < num_T; i++ ) {
    gintersected_types[gnum_types][i] = T[i];
  }
  gnum_intersected_types[gnum_types] = num_T;
  intersected_type = gnum_types;
  gnum_types++;

  for ( j = 0; j < gtype_size[T[0]]; j++ ) {
    for ( k = 1; k < num_T; k++ ) {
      if ( !gis_member[gtype_consts[T[0]][j]][T[k]] ) {
	break;
      }
    }
    if ( k < num_T ) {
      continue;
    }
    /* add constant to new type
     */
    if ( gtype_size[intersected_type] == MAX_TYPE ) {
      printf("\ntoo many consts in intersected type! increase MAX_TYPE (currently %d)\n\n",
	     MAX_TYPE);
      exit( 1 );
    }
    gtype_consts[intersected_type][gtype_size[intersected_type]++] = gtype_consts[T[0]][j];
    gis_member[gtype_consts[T[0]][j]][intersected_type] = TRUE;
  }
  
  /* now verify if the intersected type equals one of the types that we intersected.
   * this is the case, iff one of the types in T has the same size as intersected_type
   */
  for ( j = 0; j < num_T; j++ ) {
    if ( gtype_size[intersected_type] != gtype_size[T[j]] ) {
      continue;
    }
    /* type T[j] contains exactly the constants that we need!
     *
     * remove intersected type from table!
     */
    gtype_size[intersected_type] = 0;
    for ( k = 0; k < MAX_CONSTANTS; k++ ) {
      gis_member[k][intersected_type] = FALSE;
    }
    gnum_intersected_types[intersected_type] = -1;
    gnum_types--;
    intersected_type = T[j];
    break;
  }

  return intersected_type;

}



int find_intersected_type( TypeArray T, int num_T )

{

  int i, j;

  for ( i = 0; i < gnum_types; i++ ) {
    if ( gnum_intersected_types[i] == -1 ) {
      continue;
    }

    if ( gnum_intersected_types[i] != num_T ) {
      continue;
    }

    for ( j = 0; j < num_T; j++ ) {
      if ( T[j] != gintersected_types[i][j] ) {
	break;
      }
    }
    if ( j < num_T ) {
      continue;
    }

    return i;
  }

  return -1;

}
  













/******************************
 * MULTIPLY EFFECT PARAMETERS *
 ******************************/












/* local globals for multiplying
 */

int linertia_conds[MAX_VARS];
int lnum_inertia_conds;

int lmultiply_parameters[MAX_VARS];
int lnum_multiply_parameters;

NormOperator *lo;
NormEffect *le;

NormEffect *lres;






void multiply_easy_effect_parameters( void )

{

  int i, j, k, l, p, par;
  NormEffect *e;

  for ( i = 0; i < gnum_easy_operators; i++ ) {
    lo = geasy_operators[i];

    lres = NULL;
    for ( e = lo->effects; e; e = e->next ) {
      le = e;

      lnum_inertia_conds = 0;
      for ( j = 0; j < e->num_conditions; j++ ) {
	for ( k = 0; k < garity[e->conditions[j].predicate]; k++ ) {
	  if ( e->conditions[j].args[k] < 0 &&
	       DECODE_VAR( e->conditions[j].args[k] ) < lo->num_vars ) {
	    break;
	  }
	}
	if ( k < garity[e->conditions[j].predicate] ) {
	  /* only consider inertia constraining effect parameters
	   */
	  continue;
	}
	if ( !gis_added[e->conditions[j].predicate] &&
	     !gis_deleted[e->conditions[j].predicate] ) {
	  linertia_conds[lnum_inertia_conds++] = j;
	}
      }

      lnum_multiply_parameters = 0;
      for ( j = 0; j < e->num_vars; j++ ) {
	par = lo->num_vars + j;
	for ( k = 0; k < lnum_inertia_conds; k++ ) {
	  p = e->conditions[linertia_conds[k]].predicate;
	  for ( l = 0; l < garity[p]; l++ ) {
	    if ( e->conditions[linertia_conds[k]].args[l] ==
		 ENCODE_VAR( par ) ) {
	      break;
	    }
	  }
	  if ( l < garity[p] ) {
	    break;
	  }
	}
	if ( k < lnum_inertia_conds ) {
	  continue;
	}
	lmultiply_parameters[lnum_multiply_parameters++] = j;
      }

      unify_easy_inertia_conditions( 0 );
    }
    free_NormEffect( lo->effects );
    lo->effects = lres;
  }

}



void unify_easy_inertia_conditions( int curr_inertia )

{

  int p, i, j, af, hh;
  int args[MAX_VARS];
  int affected_params[MAX_VARS];
  int num_affected_params = 0;

  if ( curr_inertia == lnum_inertia_conds ) {
    multiply_easy_non_constrained_effect_parameters( 0 );
    return;
  }

  p = le->conditions[linertia_conds[curr_inertia]].predicate;
  for ( i = 0; i < garity[p]; i++ ) {
    args[i] = le->conditions[linertia_conds[curr_inertia]].args[i];
    if ( args[i] < 0 ) {
      hh = DECODE_VAR( args[i] );
      hh -= lo->num_vars;
      if ( le->inst_table[hh] != -1 ) {
	args[i] = le->inst_table[hh];
      } else {
	affected_params[num_affected_params++] = hh;
      }
    }
  }

  for ( i = 0; i < gnum_initial_predicate[p]; i++ ) {
    af = 0;
    for ( j = 0; j < garity[p]; j++ ) {
      if ( args[j] >= 0 ) {
	if ( args[j] != ginitial_predicate[p][i].args[j] ) {
	  break;
	} else {
	  continue;
	}
      }
      le->inst_table[affected_params[af++]] = ginitial_predicate[p][i].args[j];
    }
    if ( j < garity[p] ) {
      continue;
    }

    unify_easy_inertia_conditions( curr_inertia + 1 );
  }

  for ( i = 0; i < num_affected_params; i++ ) {
    le->inst_table[affected_params[i]] = -1;
  }

}



void multiply_easy_non_constrained_effect_parameters( int curr_parameter )

{

  int t, n, i, j, k, p, par;
  NormEffect *tmp;
  Bool rem;

  if ( curr_parameter == lnum_multiply_parameters ) {
    /* create new effect, adjusting conds to inst, and
     * partially instantiating effects;
     *
     * add result to  lres
     */
    tmp = new_NormEffect2( le );
    /* instantiate param occurences
     */
    for ( i = 0; i < le->num_vars; i++ ) {
      par = lo->num_vars + i;
      for ( j = 0; j < tmp->num_conditions; j++ ) {
	for ( k = 0; k < garity[tmp->conditions[j].predicate]; k++ ) {
	  if ( tmp->conditions[j].args[k] == ENCODE_VAR( par ) ) {
	    tmp->conditions[j].args[k] = le->inst_table[i];
	  }
	}
      }
      for ( j = 0; j < tmp->num_adds; j++ ) {
	for ( k = 0; k < garity[tmp->adds[j].predicate]; k++ ) {
	  if ( tmp->adds[j].args[k] == ENCODE_VAR( par ) ) {
	    tmp->adds[j].args[k] = le->inst_table[i];
	  }
	}
      }
      for ( j = 0; j < tmp->num_dels; j++ ) {
	for ( k = 0; k < garity[tmp->dels[j].predicate]; k++ ) {
	  if ( tmp->dels[j].args[k] == ENCODE_VAR( par ) ) {
	    tmp->dels[j].args[k] = le->inst_table[i];
	  }
	}
      }
    }
    /* adjust conditions
     */
    i = 0;
    while ( i < tmp->num_conditions ) {
      rem = FALSE;
      p = tmp->conditions[i].predicate;
      if ( !gis_added[p] &&
	   !gis_deleted[p] ) {
	for ( j = 0; j < garity[p]; j++ ) {
	  if ( tmp->conditions[i].args[j] < 0 &&
	       DECODE_VAR( tmp->conditions[i].args[j] < lo->num_vars ) ) {
	    break;
	  }
	}
	if ( j == garity[p] ) {
	  /* inertia that constrain only effect params have been unified,
	   * are therefore TRUE
	   */
	  rem = TRUE;
	}
      }
      if ( rem ) {
	for ( j = i; j < tmp->num_conditions - 1; j++ ) {
	  tmp->conditions[j].predicate = tmp->conditions[j+1].predicate;
	  for ( k = 0; k < garity[tmp->conditions[j+1].predicate]; k++ ) {
	    tmp->conditions[j].args[k] = tmp->conditions[j+1].args[k];
	  }
	}
	tmp->num_conditions--;
      } else {
	i++;
      }
    }
    /* add result to lres
     */
    if ( lres ) {
      lres->prev = tmp;
    }
    tmp->next = lres;
    lres = tmp;
    return;
  }

  t = le->var_types[lmultiply_parameters[curr_parameter]];
  n = gtype_size[t];

  for ( i = 0; i < n; i++ ) {
    le->inst_table[lmultiply_parameters[curr_parameter]] = gtype_consts[t][i];
    multiply_easy_non_constrained_effect_parameters( curr_parameter + 1 );
  }

  le->inst_table[lmultiply_parameters[curr_parameter]] = -1;

}



















/**************************
 * MULTIPLY OP PARAMETERS *
 **************************/


















void multiply_easy_op_parameters( void )

{

  int i, j, k, l, p;
  NormOperator *o;

  geasy_templates = NULL;
  gnum_easy_templates = 0;

  for ( i = 0; i < gnum_easy_operators; i++ ) {
    lo = geasy_operators[i];

    lnum_inertia_conds = 0;
    for ( j = 0; j < lo->num_preconds; j++ ) {
      if ( !gis_added[lo->preconds[j].predicate] &&
	   !gis_deleted[lo->preconds[j].predicate] ) {
	linertia_conds[lnum_inertia_conds++] = j;
      }
    }
      
    lnum_multiply_parameters = 0;
    for ( j = 0; j < lo->num_vars; j++ ) {
      for ( k = 0; k < lnum_inertia_conds; k++ ) {
	p = lo->preconds[linertia_conds[k]].predicate;
	for ( l = 0; l < garity[p]; l++ ) {
	  if ( lo->preconds[linertia_conds[k]].args[l] ==
	       ENCODE_VAR( j ) ) {
	    break;
	  }
	}
	if ( l < garity[p] ) {
	  break;
	}
      }
      if ( k < lnum_inertia_conds ) {
	continue;
      }
      lmultiply_parameters[lnum_multiply_parameters++] = j;
    }

    unify_easy_inertia_preconds( 0 );
  }

  /* now remove inertia preconditions from operator schemata
   */
  for ( i = 0; i < gnum_easy_operators; i++ ) {
    o = geasy_operators[i];

    j = 0;
    while ( j < o->num_preconds ) {
      if ( !gis_added[o->preconds[j].predicate] &&
	   !gis_deleted[o->preconds[j].predicate] ) {
	for ( k = j; k < o->num_preconds - 1; k++ ) { 
 	  o->preconds[k].predicate = o->preconds[k+1].predicate;
	  for ( l = 0; l < garity[o->preconds[k].predicate]; l++ ) {
	    o->preconds[k].args[l] = o->preconds[k+1].args[l];
	  }
	}
	o->num_preconds--;
      } else {
	j++;
      }
    }
  }   

}



void unify_easy_inertia_preconds( int curr_inertia )

{

  int p, i, j, af, hh;
  int args[MAX_VARS];
  int affected_params[MAX_VARS];
  int num_affected_params = 0;

  if ( curr_inertia == lnum_inertia_conds ) {
    multiply_easy_non_constrained_op_parameters( 0 );
    return;
  }

  p = lo->preconds[linertia_conds[curr_inertia]].predicate;
  for ( i = 0; i < garity[p]; i++ ) {
    args[i] = lo->preconds[linertia_conds[curr_inertia]].args[i];
    if ( args[i] < 0 ) {
      hh = DECODE_VAR( args[i] );
      if ( lo->inst_table[hh] != -1 ) {
	args[i] = lo->inst_table[hh];
      } else {
	affected_params[num_affected_params++] = hh;
      }
    }
  }

  for ( i = 0; i < gnum_initial_predicate[p]; i++ ) {
    af = 0;
    for ( j = 0; j < garity[p]; j++ ) {
      if ( args[j] >= 0 ) {
	if ( args[j] != ginitial_predicate[p][i].args[j] ) {
	  break;
	} else {
	  continue;
	}
      }
      /* check whether that constant has the correct type for that
       * parameter
       */
      if ( !gis_member[ginitial_predicate[p][i].args[j]][lo->var_types[affected_params[af]]] ) {
	break;
      }
      /* legal constant; set op parameter instantiation to it
       */
      lo->inst_table[affected_params[af++]] = ginitial_predicate[p][i].args[j];
    }
    if ( j < garity[p] ) {
      continue;
    }

    unify_easy_inertia_preconds( curr_inertia + 1 );
  }

  for ( i = 0; i < num_affected_params; i++ ) {
    lo->inst_table[affected_params[i]] = -1;
  }

}



void multiply_easy_non_constrained_op_parameters( int curr_parameter )

{

  EasyTemplate *tmp;
  int i, j, t, n;

  if ( curr_parameter == lnum_multiply_parameters ) {
    tmp = new_EasyTemplate( lo );
    for ( i = 0; i < lo->num_vars; i++ ) {
      tmp->inst_table[i] = lo->inst_table[i];
    }
    tmp->next = geasy_templates;
    if ( geasy_templates ) {
      geasy_templates->prev = tmp;
    }
    geasy_templates = tmp;
    gnum_easy_templates++;
    return;
  }

  if ( curr_parameter == lnum_multiply_parameters - 1 ) {
    t = lo->var_types[lmultiply_parameters[curr_parameter]];
    n = gtype_size[t];
    for ( i = 0; i < n; i++ ) {
      lo->inst_table[lmultiply_parameters[curr_parameter]] = gtype_consts[t][i];

      tmp = new_EasyTemplate( lo );
      for ( j = 0; j < lo->num_vars; j++ ) {
	tmp->inst_table[j] = lo->inst_table[j];
      }
      tmp->next = geasy_templates;
      if ( geasy_templates ) {
	geasy_templates->prev = tmp;
      }
      geasy_templates = tmp;
      gnum_easy_templates++;
    }

    lo->inst_table[lmultiply_parameters[curr_parameter]] = -1;

    return;
  }

  t = lo->var_types[lmultiply_parameters[curr_parameter]];
  n = gtype_size[t];
  for ( i = 0; i < n; i++ ) {
    lo->inst_table[lmultiply_parameters[curr_parameter]] = gtype_consts[t][i];

    multiply_easy_non_constrained_op_parameters( curr_parameter + 1 );
  }

  lo->inst_table[lmultiply_parameters[curr_parameter]] = -1;

}
