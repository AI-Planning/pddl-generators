

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
 * File: inst_hard.c
 * Description: functions for multiplying hard operators.
 *
 *
 * Author: Joerg Hoffmann 2000
 *
 *********************************************************************/ 









#include "ff.h"

#include "output.h"
#include "memory.h"

#include "inst_pre.h"
#include "inst_hard.h" 











/* used in multiplying routines
 */
int linst_table[MAX_VARS];
int_pointer lini[MAX_PREDICATES];








void build_hard_action_templates( void )

{

  int i, j, size, adr;
  MixedOperator *o;

  /* remove unused params; empty types are already recognised during
   * domain translation; have to be handled after (or while)
   * unaries encoding (if done), though.
   */
  cleanup_hard_domain();

  if ( gcmd_line.display_info == 115 ) {
    printf("\n\ncleaned up hard domain representation is:\n\n");
    for ( i = 0; i < gnum_hard_operators; i++ ) {
      print_Operator( ghard_operators[i] );
    }
  }


  /* create local table of instantiated facts that occur in the
   * initial state. for fast finding out if fact is in ini or not.
   */
  for ( i = 0; i < gnum_predicates; i++ ) {
    size = 1;
    for ( j = 0; j < garity[i]; j++ ) {
      size *= gtype_size[gpredicates_args_type[i][j]];
    }
    lini[i] = ( int_pointer ) calloc( size, sizeof( int ) );
    for ( j = 0; j < size; j++ ) {
      lini[i][j] = 0;
    }
    for ( j = 0; j < gnum_initial_predicate[i]; j++ ) {
      adr = instantiated_fact_adress( &ginitial_predicate[i][j] );
      lini[i][adr]++;
    }
  }


  /* create mixed op for each param combination
   */
  multiply_hard_op_parameters();

  if ( gcmd_line.display_info == 116 ) {
    printf("\n\nmixed hard domain representation is:\n\n");
    for ( o = ghard_mixed_operators; o; o = o->next ) {
      print_MixedOperator( o );
    }
  }


  /* create pseudo op for each mixed op
   */
  multiply_hard_effect_parameters();
 
  if ( gcmd_line.display_info == 117 ) {
    printf("\n\npseudo hard domain representation is:\n\n");
    for ( i = 0; i < gnum_hard_templates; i++ ) {
      print_PseudoAction( ghard_templates[i] );
    }
  }
 

}












/****************
 * CLEANUP CODE *
 ****************/












void cleanup_hard_domain( void )

{

  int i, j, k, par;
  Operator *o;
  Effect *e;

  /* so far, only unused parameters removal
   */

  for ( i = 0; i < gnum_hard_operators; i++ ) {
    o = ghard_operators[i];

    j = 0;
    while ( j < o->num_vars ) {
      if ( var_used_in_wff( ENCODE_VAR( j ), o->preconds ) ) {
	j++;
	continue;
      }

      for ( e = o->effects; e; e = e->next ) {
	if ( var_used_in_wff( ENCODE_VAR( j ), e->conditions ) ) {
	  break;
	}
	if ( var_used_in_literals( ENCODE_VAR( j ), e->effects ) ) {
	  break;
	}
      }
      if ( e ) {
	j++;
	continue;
      }

      o->removed[j] = TRUE;
      j++;
    }

    for ( e = o->effects; e; e = e->next ) {
      j = 0;
      while ( j < e->num_vars ) {
	par = o->num_vars + j;
	if ( var_used_in_wff( ENCODE_VAR( par ), e->conditions ) ) {
	  j++;
	  continue;
	}
	if ( var_used_in_literals( ENCODE_VAR( par ), e->effects ) ) {
	  j++;
	  continue;
	}

	if ( e->var_names[j] ) {
	  free( e->var_names[j] );
	}
	for ( k = j; k < e->num_vars - 1; k++ ) {
	  e->var_names[k] = e->var_names[k+1];
	  e->var_names[k] = e->var_names[k+1];
	}
	e->num_vars--;
	decrement_inferior_vars( par, e->conditions );
	decrement_inferior_vars_in_literals( par, e->effects );
      }
    }
  }

}



Bool var_used_in_literals( int code_var, Literal *ef )

{

  Literal *l;
  int i;
  
  for ( l = ef; l; l = l->next ) {
    for ( i = 0; i < garity[l->fact.predicate]; i++ ) {
      if ( l->fact.args[i] == code_var ) {
	return TRUE;
      }
    }
  }

  return FALSE;

}



void decrement_inferior_vars_in_literals( int var, Literal *ef )

{

  Literal *l;
  int i;
  
  for ( l = ef; l; l = l->next ) {
    for ( i = 0; i < garity[l->fact.predicate]; i++ ) {
      if ( l->fact.args[i] >= 0 ) {
	continue;
      }
      if ( DECODE_VAR( l->fact.args[i] ) > var ) {
	l->fact.args[i]++;
      }
    }
  }

}














/******************************
 * CODE THAT BUILDS MIXED OPS *
 ******************************/

















void multiply_hard_op_parameters( void )

{

  int i;

  ghard_mixed_operators = NULL;

  for ( i = 0; i < MAX_VARS; i++ ) {
    linst_table[i] = -1;
  }

  for ( i = 0; i < gnum_hard_operators; i++ ) {
    create_hard_mixed_operators( ghard_operators[i], 0 );
  }

}



void create_hard_mixed_operators( Operator *o, int curr_var )

{

  int t, i, m, ccc;
  WffNode *tmp1, *w, *ww;
  MixedOperator *tmp2;

  if ( curr_var < o->num_vars ) {
    if ( o->removed[curr_var] ) {
      /* param doesn't matter -- select any appropriate type constant
       * at least one there; otherwise, op would not have been translated.
       */
      linst_table[curr_var] = gtype_consts[o->var_types[curr_var]][0];
      create_hard_mixed_operators( o, curr_var + 1 );
      linst_table[curr_var] = -1;
      return;
    }

    t = o->var_types[curr_var];
    for ( i = 0; i < gtype_size[t]; i++ ) {
      linst_table[curr_var] = gtype_consts[t][i];

      create_hard_mixed_operators( o, curr_var + 1 );

      linst_table[curr_var] = -1;
    }
    return;
  }

  tmp1 = instantiate_wff( o->preconds );

  if ( tmp1->connective == FAL ) {
    free_WffNode( tmp1 );
    return;
  }

  dnf( &tmp1 );
  cleanup_wff( &tmp1 );

  if ( tmp1->connective == FAL ) {
    free_WffNode( tmp1 );
    return;
  }

  /* only debugging, REMOVE LATER
   */
  if ( is_dnf( tmp1 ) == -1 ) {
    printf("\n\nILLEGAL DNF %s AFTER INSTANTIATION\n\n", o->name);
    print_Wff( tmp1, 0 );
    exit( 1 );
  }

  switch ( tmp1->connective ) {
  case OR:
    ccc = 0;
    for ( w = tmp1->sons; w; w = w->next ) {
      tmp2 = new_MixedOperator( o );
      sprintf(tmp2->name, "%s-%d", o->name, ccc);
      ccc++;
      for ( i = 0; i < o->num_vars; i++ ) {
	tmp2->inst_table[i] = linst_table[i];
      }
      if ( w->connective == AND ) {
	m = 0;
	for ( ww = w->sons; ww; ww = ww->next ) m++;
	tmp2->preconds = ( Fact * ) calloc( m, sizeof( Fact ) );
	tmp2->num_preconds = m;
	m = 0;
	for ( ww = w->sons; ww; ww = ww->next ) {
	  tmp2->preconds[m].predicate = ww->fact->predicate;
	  for ( i = 0; i < garity[ww->fact->predicate]; i++ ) {
	    tmp2->preconds[m].args[i] = ww->fact->args[i];
	  }
	  m++;
	}
      } else {
	tmp2->preconds = ( Fact * ) calloc( 1, sizeof( Fact ) );
	tmp2->num_preconds = 1;
	tmp2->preconds[0].predicate = w->fact->predicate;
	for ( i = 0; i < garity[w->fact->predicate]; i++ ) {
	  tmp2->preconds[0].args[i] = w->fact->args[i];
	}
      }
      tmp2->effects = instantiate_Effect( o->effects );
      tmp2->next = ghard_mixed_operators;
      ghard_mixed_operators = tmp2;
      gnum_hard_mixed_operators++;
    }
    break;
  case AND:
    tmp2 = new_MixedOperator( o );
    sprintf(tmp2->name, "%s", o->name);
    for ( i = 0; i < o->num_vars; i++ ) {
      tmp2->inst_table[i] = linst_table[i];
    }
    m = 0;
    for ( w = tmp1->sons; w; w = w->next ) m++;
    tmp2->preconds = ( Fact * ) calloc( m, sizeof( Fact ) );
    tmp2->num_preconds = m;
    m = 0;
    for ( w = tmp1->sons; w; w = w->next ) {
      tmp2->preconds[m].predicate = w->fact->predicate;
      for ( i = 0; i < garity[w->fact->predicate]; i++ ) {
	tmp2->preconds[m].args[i] = w->fact->args[i];
      }
      m++;
    }
    tmp2->effects = instantiate_Effect( o->effects );
    tmp2->next = ghard_mixed_operators;
    ghard_mixed_operators = tmp2;
    gnum_hard_mixed_operators++;
    break;
  case ATOM:
    tmp2 = new_MixedOperator( o );
    sprintf(tmp2->name, "%s", o->name);
    for ( i = 0; i < o->num_vars; i++ ) {
      tmp2->inst_table[i] = linst_table[i];
    }
    tmp2->preconds = ( Fact * ) calloc( 1, sizeof( Fact ) );
    tmp2->num_preconds = 1;
    tmp2->preconds[0].predicate = tmp1->fact->predicate;
    for ( i = 0; i < garity[tmp1->fact->predicate]; i++ ) {
      tmp2->preconds[0].args[i] = tmp1->fact->args[i];
    }
    tmp2->effects = instantiate_Effect( o->effects );
    tmp2->next = ghard_mixed_operators;
    ghard_mixed_operators = tmp2;
    gnum_hard_mixed_operators++;
    break;
  case TRU:
    tmp2 = new_MixedOperator( o );
    sprintf(tmp2->name, "%s", o->name);
    for ( i = 0; i < o->num_vars; i++ ) {
      tmp2->inst_table[i] = linst_table[i];
    }
    tmp2->effects = instantiate_Effect( o->effects );
    tmp2->next = ghard_mixed_operators;
    ghard_mixed_operators = tmp2;
    gnum_hard_mixed_operators++;
    break;
  default:
    printf("\n\nillegal connective %d in parsing DNF precond.\n\n",
	   tmp1->connective);
    exit( 1 );
  }

  free_WffNode( tmp1 );

}



Effect *instantiate_Effect( Effect *e )

{

  Effect *res = NULL, *tmp, *i;
  Literal *tt, *l;
  int j;

  for ( i = e; i; i = i->next ) {
    tmp = new_Effect();

    for ( j = 0; j < i->num_vars; j++ ) {
      tmp->var_types[j] = i->var_types[j];
    }
    tmp->num_vars = i->num_vars;

    tmp->conditions = instantiate_wff( i->conditions );

    if ( tmp->conditions->connective == FAL ) {
      free_partial_Effect( tmp );
      continue;
    }

    for ( l = i->effects; l; l = l->next ) {
      tt = new_Literal();
      tt->negated = l->negated;
      tt->fact.predicate = l->fact.predicate;
      for ( j = 0; j < garity[tt->fact.predicate]; j++ ) {
	tt->fact.args[j] = l->fact.args[j];
	if ( tt->fact.args[j] < 0 &&
	     linst_table[DECODE_VAR( tt->fact.args[j] )] != -1 ) {
	  tt->fact.args[j] = linst_table[DECODE_VAR( tt->fact.args[j] )];
	}
      }
      tt->next = tmp->effects;
      if ( tmp->effects ) {
	tmp->effects->prev = tt;
      }
      tmp->effects = tt;
    }

    tmp->next = res;
    if ( res ) {
      res->prev = tmp;
    }
    res = tmp;
  }

  return res;

}



WffNode *instantiate_wff( WffNode *w )

{

  WffNode *res = NULL, *tmp, *i;
  int j, c0, c1, m, h;
  Bool ok;

  switch ( w->connective ) {
  case AND:
    m = 0;
    i = w->sons;
    while ( i ) {
      tmp = instantiate_wff( i );
      if ( tmp->connective == FAL ) {
	free_WffNode( res );
	return tmp;
      }
      if ( tmp->connective == TRU ) {
	free( tmp );
	i = i->next;
	continue;
      }
      tmp->next = res;
      if ( res ) {
	res->prev = tmp;
      }
      res = tmp;
      i = i->next;
      m++;
    }
    if ( m == 0 ) {
      res = new_WffNode( TRU );
      break;
    }
    if ( m == 1 ) {
      break;
    }
    tmp = new_WffNode( AND );
    tmp->sons = res;
    res = tmp;
    break;
  case OR:
    m = 0;
    i = w->sons;
    while ( i ) {
      tmp = instantiate_wff( i );
      if ( tmp->connective == TRU ) {
	free_WffNode( res );
	return tmp;
      }
      if ( tmp->connective == FAL ) {
	free( tmp );
	i = i->next;
	continue;
      }
      tmp->next = res;
      if ( res ) {
	res->prev = tmp;
      }
      res = tmp;
      i = i->next;
      m++;
    }
    if ( m == 0 ) {
      res = new_WffNode( FAL );
      break;
    }
    if ( m == 1 ) {
      break;
    }
    tmp = new_WffNode( OR );
    tmp->sons = res;
    res = tmp;
    break;
  case NOT:
    /* must be non-equality
     */
    c0 = ( w->son->fact->args[0] < 0 ) ?
      linst_table[DECODE_VAR( w->son->fact->args[0] )] : w->son->fact->args[0];
    c1 = ( w->son->fact->args[1] < 0 ) ?
      linst_table[DECODE_VAR( w->son->fact->args[1] )] : w->son->fact->args[1];
    if ( c0 < 0 ||
	 c1 < 0 ) {
      /* ef param while checking ef conds in inst op
       */
      res = new_WffNode( ATOM );
      res->fact = new_Fact();
      res->fact->predicate = -2;
      res->fact->args[0] = ( c0 < 0 ) ? w->son->fact->args[0] : c0;
      res->fact->args[1] = ( c1 < 0 ) ? w->son->fact->args[1] : c1;
      break;
    }
    if ( c0 != c1 ) {
      res = new_WffNode( TRU );
    } else {
      res = new_WffNode( FAL );
    }
    break;
  case ATOM:
    res = new_WffNode( ATOM );
    res->fact = new_Fact();
    res->fact->predicate = w->fact->predicate;
    ok = TRUE;
    for ( j = 0; j < garity[res->fact->predicate]; j++ ) {
      h = ( w->fact->args[j] < 0 ) ?
	linst_table[DECODE_VAR( w->fact->args[j] )] : w->fact->args[j];
      if ( h < 0 ) {
	ok = FALSE;
	res->fact->args[j] = w->fact->args[j];
      } else {
	res->fact->args[j] = h;
      }
    }
    if ( !ok ) {/* contains ef params */
      break;
    }
    if ( !full_possibly_negative( res->fact ) ) {
      free( res->fact );
      res->fact = NULL;
      res->connective = TRU;
      break;
    }
    if ( !full_possibly_positive( res->fact ) ) {
      free( res->fact );
      res->fact = NULL;
      res->connective = FAL;
      break;
    }
    break;
  case TRU:
  case FAL:
    res = new_WffNode( w->connective );
    break;
  default:
    printf("\n\nillegal connective %d in instantiate formula\n\n",
	   w->connective);
    exit( 1 );
  }

  return res;

}



Bool full_possibly_positive( Fact *f )

{

  int adr;

  if ( gis_added[f->predicate] ) {
    return TRUE;
  }

  adr = instantiated_fact_adress( f );

  if ( lini[f->predicate][adr] > 0 ) {
    return TRUE;
  } else {
    return FALSE;
  }

}



Bool full_possibly_negative( Fact *f )

{

  int adr;

  if ( gis_deleted[f->predicate] ) {
    return TRUE;
  }

  adr = instantiated_fact_adress( f );

  if ( lini[f->predicate][adr] > 0 ) {
    return FALSE;
  } else {
    return TRUE;
  }

}



int instantiated_fact_adress( Fact *f )

{

  int r = 0, b = 1, i;

  for ( i = 0; i < garity[f->predicate]; i++ ) {
    r += b * gmember_nr[f->args[i]][gpredicates_args_type[f->predicate][i]];
    b *= gtype_size[gpredicates_args_type[f->predicate][i]];
  }

  return r;

}














/*********************************************************
 * CODE THAT MULTIPLIES EFFECT PARAMS --> PSEUDO ACTIONS *
 *********************************************************/















void multiply_hard_effect_parameters( void )

{

  MixedOperator *o;
  PseudoAction *tmp;
  int i;
  Effect *e;

  ghard_templates = ( PseudoAction_pointer * ) 
    calloc( gnum_hard_mixed_operators, sizeof ( PseudoAction_pointer ) );
  gnum_hard_templates = 0;

  for ( o = ghard_mixed_operators; o; o = o->next ) {
    tmp = new_PseudoAction( o );
    tmp->name = o->name;

    for ( i = 0; i < tmp->operator->num_vars; i++ ) {
      linst_table[i] = tmp->inst_table[i];
    }

    for ( e = o->effects; e; e = e->next ) {
      create_hard_pseudo_effects( tmp, e, 0 );
    }

    ghard_templates[gnum_hard_templates++] = tmp;
  }
}



void create_hard_pseudo_effects( PseudoAction *a, Effect *e, int curr_var )

{

  int par, t, i, m;
  WffNode *tmp1, *w, *ww;
  PseudoActionEffect *tmp2;

  if ( curr_var < e->num_vars ) {
    par = a->operator->num_vars + curr_var;

    t = e->var_types[curr_var];
    for ( i = 0; i < gtype_size[t]; i++ ) {
      linst_table[par] = gtype_consts[t][i];

      create_hard_pseudo_effects( a, e, curr_var + 1 );

      linst_table[par] = -1;
    }
    return;
  }

  tmp1 = instantiate_wff( e->conditions );

  if ( tmp1->connective == FAL ) {
    free_WffNode( tmp1 );
    return;
  }

  dnf( &tmp1 );
  cleanup_wff( &tmp1 );

  /* only debugging, REMOVE LATER
   */
  if ( is_dnf( tmp1 ) == -1 ) {
    printf("\n\nILLEGAL DNF %s AFTER INSTANTIATION\n\n", a->operator->name);
    print_Wff( tmp1, 0 );
    exit( 1 );
  }

  switch ( tmp1->connective ) {
  case OR:
    for ( w = tmp1->sons; w; w = w->next ) {
      tmp2 = new_PseudoActionEffect();
      if ( w->connective == AND ) {
	m = 0;
	for ( ww = w->sons; ww; ww = ww->next ) m++;
	tmp2->conditions = ( Fact * ) calloc( m, sizeof( Fact ) );
	tmp2->num_conditions = m;
	m = 0;
	for ( ww = w->sons; ww; ww = ww->next ) {
	  tmp2->conditions[m].predicate = ww->fact->predicate;
	  for ( i = 0; i < garity[ww->fact->predicate]; i++ ) {
	    tmp2->conditions[m].args[i] = ww->fact->args[i];
	  }
	  m++;
	}
      } else {
	tmp2->conditions = ( Fact * ) calloc( 1, sizeof( Fact ) );
	tmp2->num_conditions = 1;
	tmp2->conditions[0].predicate = w->fact->predicate;
	for ( i = 0; i < garity[w->fact->predicate]; i++ ) {
	  tmp2->conditions[0].args[i] = w->fact->args[i];
	}
      }
      make_instantiate_literals( tmp2, e->effects );
      tmp2->next = a->effects;
      a->effects = tmp2;
      a->num_effects++;
    }
    break;
  case AND:
    tmp2 = new_PseudoActionEffect();
    m = 0;
    for ( w = tmp1->sons; w; w = w->next ) m++;
    tmp2->conditions = ( Fact * ) calloc( m, sizeof( Fact ) );
    tmp2->num_conditions = m;
    m = 0;
    for ( w = tmp1->sons; w; w = w->next ) {
      tmp2->conditions[m].predicate = w->fact->predicate;
      for ( i = 0; i < garity[w->fact->predicate]; i++ ) {
	tmp2->conditions[m].args[i] = w->fact->args[i];
      }
      m++;
    }
    make_instantiate_literals( tmp2, e->effects );
    tmp2->next = a->effects;
    a->effects = tmp2;
    a->num_effects++;
    break;
  case ATOM:
    tmp2 = new_PseudoActionEffect();
    tmp2->conditions = ( Fact * ) calloc( 1, sizeof( Fact ) );
    tmp2->num_conditions = 1;
    tmp2->conditions[0].predicate = tmp1->fact->predicate;
    for ( i = 0; i < garity[tmp1->fact->predicate]; i++ ) {
      tmp2->conditions[0].args[i] = tmp1->fact->args[i];
    }
    make_instantiate_literals( tmp2, e->effects );
    tmp2->next = a->effects;
    a->effects = tmp2;
    a->num_effects++;
    break;
  case TRU:
    tmp2 = new_PseudoActionEffect();
    make_instantiate_literals( tmp2, e->effects );
    tmp2->next = a->effects;
    a->effects = tmp2;
    a->num_effects++;
    break;
  default:
    printf("\n\nillegal connective %d in parsing DNF precond.\n\n",
	   tmp1->connective);
    exit( 1 );
  }

  free_WffNode( tmp1 );

}
 


void make_instantiate_literals( PseudoActionEffect *e, Literal *ll )

{

  int ma = 0, md = 0, i;
  Literal *l;

  for ( l = ll; l; l = l->next ) {
    if ( l->negated ) {
      md++;
    } else {
      ma++;
    }
  }

  e->adds = ( Fact * ) calloc( ma, sizeof( Fact ) );
  e->dels = ( Fact * ) calloc( md, sizeof( Fact ) );

  for ( l = ll; l; l = l->next ) {
    if ( l->negated ) {
      e->dels[e->num_dels].predicate = l->fact.predicate;
      for ( i = 0; i < garity[l->fact.predicate]; i++ ) {
	e->dels[e->num_dels].args[i] = ( l->fact.args[i] < 0 ) ?
	  linst_table[DECODE_VAR( l->fact.args[i] )] : l->fact.args[i];
      }
      e->num_dels++;
    } else {
      e->adds[e->num_adds].predicate = l->fact.predicate;
      for ( i = 0; i < garity[l->fact.predicate]; i++ ) {
	e->adds[e->num_adds].args[i] = ( l->fact.args[i] < 0 ) ?
	  linst_table[DECODE_VAR( l->fact.args[i] )] : l->fact.args[i];
      }
      e->num_adds++;
    }
  }

}
