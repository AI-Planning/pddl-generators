

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
 * File: memory.c
 * Description: Creation and Deletion functions for all data structures.
 *
 * Author: Joerg Hoffmann
 *
 *********************************************************************/ 









#include "ff.h"
#include "memory.h"








/**********************
 * CREATION FUNCTIONS *
 **********************/











/* parsing
 */









char *new_Token( int len )

{

  char *tok = ( char * ) calloc( len, sizeof( char ) );
  CHECK_PTR(tok);

  return tok;

}



TokenList *new_TokenList( void )

{

  TokenList *result = ( TokenList * ) calloc( 1, sizeof( TokenList ) );
  CHECK_PTR(result);

  result->item = NULL; 
  result->next = NULL;

  return result;

}



FactList *new_FactList( void )

{

  FactList *result = ( FactList * ) calloc( 1, sizeof( FactList ) );
  CHECK_PTR(result);

  result->item = NULL; 
  result->next = NULL;

  return result;

}



TypedList *new_TypedList( void )

{

  TypedList *result = ( TypedList * ) calloc( 1, sizeof( TypedList ) );
  CHECK_PTR(result);

  result->name = NULL; 
  result->type = NULL;
  result->n = -1;

  return result;

}



TypedListList *new_TypedListList( void )

{

  TypedListList *result = ( TypedListList * ) calloc( 1, sizeof( TypedListList ) );
  CHECK_PTR(result);

  result->predicate = NULL; 
  result->args = NULL;

  return result;

}



PlNode *new_PlNode( Connective c )

{

  PlNode *result = ( PlNode * ) calloc( 1, sizeof( PlNode ) );
  CHECK_PTR(result);

  result->connective = c;
  result->atom = NULL;
  result->sons = NULL;
  result->next = NULL;

  return result;

}



PlOperator *new_PlOperator( char *name )

{

  PlOperator *result = ( PlOperator * ) calloc( 1, sizeof( PlOperator ) );
  CHECK_PTR(result);

  if ( name ) {
    result->name = new_Token(strlen(name)+1);
    CHECK_PTR(result->name);
    strcpy(result->name, name);
  } else {
    result->name = NULL;
  }

  result->params = NULL;
  result->preconds = NULL;
  result->effects = NULL;
  result->number_of_real_params = 0;
  result->next = NULL;

  return result;

}



PlOperator *new_axiom_op_list( void )

{

  static int count;
  char *name;
  PlOperator *ret;

  /* WARNING: count should not exceed 999 
   */
  count++;
  if ( count == 10000 ) {
    printf("\ntoo many axioms! look into memory.c, line 157\n\n");
    exit( 1 );
  }
  name = new_Token(strlen(HIDDEN_STR)+strlen(AXIOM_STR)+4+1);
  sprintf(name, "%s%s%4d", HIDDEN_STR, AXIOM_STR, count);

  ret = new_PlOperator(name);
  free(name);

  return ret;

}














/* instantiation
 */











Fact *new_Fact( void )

{

  Fact *result = ( Fact * ) calloc( 1, sizeof( Fact ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( Fact ) );

  return result;

}



Facts *new_Facts( void )

{

  Facts *result = ( Facts * ) calloc( 1, sizeof( Facts ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( Facts ) );

  result->fact = new_Fact();

  result->next = NULL;

  return result;

}



WffNode *new_WffNode( Connective c )

{

  WffNode *result = ( WffNode * ) calloc( 1, sizeof( WffNode ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( WffNode ) );

  result->connective = c;

  result->var = -1;
  result->var_type = -1;
  result->var_name = NULL;

  result->sons = NULL;
  result->next = NULL;
  result->prev = NULL;

  result->fact = NULL;
  result->NOT_p = -1;

  result->son = NULL;

  result->visited = FALSE;

  return result;

}



Literal *new_Literal( void ) 

{

  Literal *result = ( Literal * ) calloc( 1, sizeof( Literal ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( Literal ) );

  result->next = NULL;
  result->prev = NULL;

  return result; 

}



Effect *new_Effect( void )

{

  Effect *result = ( Effect * ) calloc( 1, sizeof( Effect ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( Effect ) );

  result->num_vars = 0;

  result->conditions = NULL;

  result->effects = NULL;

  result->next = NULL;
  result->prev = NULL;

  return result;

}



Operator *new_Operator( char *name, int norp )

{

  int i;

  Operator *result = ( Operator * ) calloc( 1, sizeof( Operator ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( Operator ) );

  if ( name ) {
    result->name = new_Token( strlen( name ) + 1 );
    CHECK_PTR( result->name );
    strcpy( result->name, name );
  } else {
    result->name = NULL;
  }

  result->num_vars = 0;
  result->number_of_real_params = norp;

  for ( i = 0; i < MAX_VARS; i++ ) {
    result->removed[i] = FALSE;
  }

  result->preconds = NULL;

  result->effects = NULL;

  result->hard = TRUE;

  return result;

}



NormEffect *new_NormEffect1( Effect *e )

{

  int i;

  NormEffect *result = ( NormEffect * ) calloc( 1, sizeof( NormEffect ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( NormEffect ) );

  result->num_vars = e->num_vars;
  for ( i = 0; i < e->num_vars; i++ ) {
    result->var_types[i] = e->var_types[i];
    result->inst_table[i] = -1;
  }

  result->conditions = NULL;
  result->num_conditions = 0;

  result->adds = NULL;
  result->num_adds = 0;
  result->dels = NULL;
  result->num_dels = 0;

  result->next = NULL;
  result->prev = NULL;

  return result;

}



NormEffect *new_NormEffect2( NormEffect *e )

{

  int i, j, a;

  NormEffect *result = ( NormEffect * ) calloc( 1, sizeof( NormEffect ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( NormEffect ) );

  result->num_vars = 0;

  result->conditions = ( Fact * ) calloc( e->num_conditions, sizeof( Fact ) );
  result->num_conditions = e->num_conditions;
  for ( i = 0; i < e->num_conditions; i++ ) {
    result->conditions[i].predicate = e->conditions[i].predicate;
    a = ( e->conditions[i].predicate < 0 ) ? 2 : garity[e->conditions[i].predicate];
    for ( j = 0; j < a; j++ ) {
      result->conditions[i].args[j] = e->conditions[i].args[j];
    }
  }

  result->adds = ( Fact * ) calloc( e->num_adds, sizeof( Fact ) );
  result->num_adds = e->num_adds;
  for ( i = 0; i < e->num_adds; i++ ) {
    result->adds[i].predicate = e->adds[i].predicate;
    for ( j = 0; j < garity[e->adds[i].predicate]; j++ ) {
      result->adds[i].args[j] = e->adds[i].args[j];
    }
  }
  result->dels = ( Fact * ) calloc( e->num_dels, sizeof( Fact ) );
  result->num_dels = e->num_dels;
  for ( i = 0; i < e->num_dels; i++ ) {
    result->dels[i].predicate = e->dels[i].predicate;
    for ( j = 0; j < garity[e->dels[i].predicate]; j++ ) {
      result->dels[i].args[j] = e->dels[i].args[j];
    }
  }

  result->next = NULL;
  result->prev = NULL;

  return result;

}



NormOperator *new_NormOperator( Operator *op )

{

  int i;

  NormOperator *result = ( NormOperator * ) calloc( 1, sizeof( NormOperator ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( NormOperator ) );

  result->operator = op;

  result->num_vars = op->num_vars;
  for ( i = 0; i < op->num_vars; i++ ) {
    result->var_types[i] = op->var_types[i];
    result->inst_table[i] = -1;
  }
  result->num_removed_vars = 0;

  result->preconds = NULL;
  result->num_preconds = 0;

  result->effects = NULL;

  return result;

}




EasyTemplate *new_EasyTemplate( NormOperator *op )

{

  EasyTemplate *result = ( EasyTemplate * ) calloc( 1, sizeof( EasyTemplate ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( EasyTemplate ) );

  result->op = op;

  result->prev = NULL;
  result->next = NULL;

  return result;

}



MixedOperator *new_MixedOperator( Operator *op )

{

  MixedOperator *result = ( MixedOperator * ) calloc( 1, sizeof( MixedOperator ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( MixedOperator ) );

  result->operator = op;

  result->preconds = NULL;
  result->num_preconds = 0;

  result->effects = NULL;

  return result;

}



PseudoActionEffect *new_PseudoActionEffect( void )

{

  PseudoActionEffect *result = 
    ( PseudoActionEffect * ) calloc( 1, sizeof( PseudoActionEffect ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( PseudoActionEffect ) );

  result->conditions = NULL;
  result->num_conditions = 0;

  result->adds = NULL;
  result->num_adds = 0;
  result->dels = NULL;
  result->num_dels = 0;

  result->next = NULL;

  return result;

}



PseudoAction *new_PseudoAction( MixedOperator *op )

{

  int i;

  PseudoAction *result = ( PseudoAction * ) calloc( 1, sizeof( PseudoAction ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( PseudoAction ) );

  result->operator = op->operator;
  for ( i = 0; i < op->operator->num_vars; i++ ) {
    result->inst_table[i] = op->inst_table[i];
  }

  result->preconds = op->preconds;
  result->num_preconds = op->num_preconds;

  result->effects = NULL;
  result->num_effects = 0;

  return result;

}



Action *new_Action( void )

{

  Action *result = ( Action * ) calloc( 1, sizeof( Action ) );
  CHECK_PTR(result);
  MEMORY_CHECK( sizeof( Action ) );

  result->norm_operator = NULL;
  result->pseudo_action = NULL;

  result->next = NULL;

  return result;

}



void make_state( State *S, int n )

{

  S->F = ( int * ) calloc( n, sizeof( int ) );
  S->num_F = 0;

}



EhcNode *new_EhcNode( void )

{

  EhcNode *result = ( EhcNode * ) calloc( 1, sizeof( EhcNode ) );
  CHECK_PTR(result);

  result->father = NULL;
  result->next = NULL;

  result->new_goal = -1;

  result->S.max_F = 0;

  return result;

}



EhcHashEntry *new_EhcHashEntry( void )

{

  EhcHashEntry *result = ( EhcHashEntry * ) calloc( 1, sizeof( EhcHashEntry ) );
  CHECK_PTR(result);

  result->ehc_node = NULL;

  result->next = NULL;

  return result;

}



PlanHashEntry *new_PlanHashEntry( void )

{

  PlanHashEntry *result = ( PlanHashEntry * ) calloc( 1, sizeof( PlanHashEntry ) );
  CHECK_PTR(result);

  result->next_step = NULL;

  result->next = NULL;

  result->S.max_F = 0;

  return result;

}



BfsNode *new_BfsNode( void )

{

  BfsNode *result = ( BfsNode * ) calloc( 1, sizeof( BfsNode ) );
  CHECK_PTR(result);

  result->father = NULL;
  result->next = NULL;
  result->prev = NULL;

  result->S.max_F = 0;

  return result;

}



BfsHashEntry *new_BfsHashEntry( void )

{

  BfsHashEntry *result = ( BfsHashEntry * ) calloc( 1, sizeof( BfsHashEntry ) );
  CHECK_PTR(result);

  result->bfs_node = NULL;

  result->next = NULL;

  return result;

}











/**********************
 * DELETION FUNCTIONS *
 **********************/












void free_TokenList( TokenList *source )

{

  if ( source ) {
    free_TokenList( source->next );
    if ( source->item ) {
      free( source->item );
    }
    free( source );
  }

}



void free_FactList( FactList *source )

{

  if ( source ) {
    free_FactList( source->next );
    free_TokenList( source->item );
    free( source );
  }

}



void free_PlNode( PlNode *node )

{
  
  if ( node ) {
    free_PlNode( node->sons );
    free_PlNode( node->next );
    free_TokenList( node->atom );
    free( node );
  }

}



void free_PlOperator( PlOperator *o )

{

  if ( o ) {
    free_PlOperator( o->next );

    if ( o->name ) {
      free( o->name );
    }
    
    free_FactList( o->params );
    free_PlNode( o->preconds );
    free_PlNode( o->effects );

    free( o );
  }

}



void free_Operator( Operator *o )

{

  if ( o ) {

    if ( o->name ) {
      free( o->name );
    }

    free( o );
  } 

}



void free_WffNode( WffNode *w )

{

  /* commented out due to memory mess too messy to figure out correctly -- sorry. */
  return;

  if ( w ) {
    free_WffNode( w->son );
    free_WffNode( w->sons );
    free_WffNode( w->next );
    if ( w->var_name ) {
      free( w->var_name );
    }
    if ( w->fact ) {
      free( w->fact );
    }
    free( w );
  }

}



void free_NormEffect( NormEffect *e )

{

  if ( e ) {
    free_NormEffect( e->next );

    if ( e->conditions ) {
      free( e->conditions );
    }
    if ( e->adds ) {
      free( e->adds );
    }
    if ( e->dels ) {
      free( e->dels );
    }

    free( e );
  }

}



void free_partial_Effect( Effect *e )

{

  if ( e ) {
    free_partial_Effect( e->next );

    free_WffNode( e->conditions );

    free( e );
  }

}



void free_NormOperator( NormOperator *o )

{

  if ( o ) {

    if ( o->preconds ) {
      free( o->preconds );
    }

    free_NormEffect( o->effects );

    free( o );
  }

}



void free_single_NormEffect( NormEffect *e )

{

  if ( e ) {
    if ( e->conditions ) {
      free( e->conditions );
    }
    if ( e->adds ) {
      free( e->adds );
    }
    if ( e->dels ) {
      free( e->dels );
    }

    free( e );
  }

}



void free_single_EasyTemplate( EasyTemplate *t )

{

  if ( t ) {
    free( t );
  }

}



void free_TypedList( TypedList *t )

{

  if ( t ) {
    if ( t->name ) {
      free( t->name );
      t->name = NULL;
    }
    if ( t->type ) {
      free_TokenList( t->type );
      t->type = NULL;
    }
    free_TypedList( t->next );

    free( t );
  }

}



void free_TypedListList( TypedListList *t )

{

  if ( t ) {
    if ( t->predicate ) {
      free( t->predicate );
      t->predicate = NULL;
    }
    if ( t->args ) {
      free_TypedList( t->args );
      t->args = NULL;
    }
    free_TypedListList( t->next );

    free( t );
  }

}



void free_single_Action( Action *a )

{

  int i;

  if ( a ) {
    if ( a->preconds ) free( a->preconds );

    for ( i = 0; i < a->num_effects; i++ ) {
      if ( a->effects[i].conditions ) free( a->effects[i].conditions );
      if ( a->effects[i].adds ) free( a->effects[i].adds );
      if ( a->effects[i].dels ) free( a->effects[i].dels );
    }
    if ( a->effects ) free( a->effects );

    free( a );
  }

}
