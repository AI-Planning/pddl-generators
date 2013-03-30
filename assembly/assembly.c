


/*********************************************************************
 * (C) Copyright 2001 Albert Ludwigs University Freiburg
 *     Institute of Computer Science
 *
 * All rights reserved. Use of this software is permitted for 
 * non-commercial research purposes, and it may be copied only 
 * for that use.  All copies must include this copyright message.
 * This software is made available AS IS, and neither the authors
 * nor the  Albert Ludwigs University Freiburg make any warranty
 * about the software or its performance. 
 *********************************************************************/


/* 
 * C code for generating random assembly problems
 *
 */





#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>






/* data structures ... (ha ha)
 */
typedef unsigned char Bool;
#define TRUE 1
#define FALSE 0


#define MAX_DEPTH 20
#define MAX_SONS 50
#define MAX_ORDERINGS 1000






typedef struct Assembly {

  int row, col;

  int requires;
  int num_sons, start_son, end_son;

  int part_of_row, part_of_col;
  int num_ao;
  int aop_row[MAX_ORDERINGS], aop_col[MAX_ORDERINGS];
  int aof_row[MAX_ORDERINGS], aof_col[MAX_ORDERINGS];

  int num_tp, tp_row[MAX_ORDERINGS], tp_col[MAX_ORDERINGS];
  
  int num_ro;
  int rop_row[MAX_ORDERINGS], rop_col[MAX_ORDERINGS];
  int rof_row[MAX_ORDERINGS], rof_col[MAX_ORDERINGS];

} Assembly, *Assembly_pointer;



/* helpers
 */


/* random problem generation
 */
void create_tree( void );
void setup_transient_parts( void );


/* printing
 */
void print_objects( void );
void print_part_of( void );
void print_assemble_order( void );
void print_requires( void );
void print_available( void );
void print_transient_part( void );
void print_remove_order( void );
void print( void );

/* command line
 */
void usage( void );
Bool process_command_line( int argc, char *argv[] );




/* globals
 */


/* command line params
 */
int gdepth, gmax_sons, gresources, gp_has_sons;
int gp_assemble_order, gp_requires, gp_transient_part, gp_remove_order;


/* problem structure
 */
Assembly_pointer gtree[MAX_DEPTH+1];
int gnum_cols[MAX_DEPTH+1];




int main( int argc, char *argv[] )

{

  /* seed the random() function
   */
  struct timeb tp;
  ftime( &tp );
  srandom( tp.millitm );


  /* command line treatment, first preset all values
   */
  gdepth = 2;
  gmax_sons = 5;
  gresources = 1;
  gp_has_sons = 90;
  gp_assemble_order = 25;
  gp_requires = 5;
  gp_transient_part = 5;
  gp_remove_order = 50;

  if ( argc == 2 && *++argv[0] == '?' ) {
    usage();
    exit( 1 );
  }
  if ( !process_command_line( argc, argv ) ) {
    usage();
    exit( 1 );
  }


  /* create randomized problem;
   *
   */
  create_tree();
  setup_transient_parts();
  
  /* now output problem in PDDL syntax
   */
  printf("\n\n\n");

  /* header
   */
  printf("(define (problem assembly-d%d-m%d-n%d-h%d-a%d-r%d-t%d-o%d)",
	 gdepth, gmax_sons, gresources, gp_has_sons, 
	 gp_assemble_order, gp_requires, gp_transient_part, gp_remove_order);
  printf("\n   (:domain assembly)");

  /* objects
   */
  printf("\n   (:objects ");
  /* separated this from main for the sake of clarity
   */
  print_objects();
  printf(")");

  /* initial state
   */
  printf("\n\n\n(:init");
  print_part_of();
  print_assemble_order();
  print_requires();
  print_available();
  print_transient_part();
  print_remove_order();
  printf("\n)");

  /* goal condition
   */
  printf("\n\n\n(:goal (complete a-0-0))");

  /* that's it
   */
  printf("\n)");

  printf("\n\n\n");

  exit( 0 );

}
  
  









/* random problem generation functions
 */











void create_tree( void )

{

  int i, j, k, r, frow, fcol;
  int current_range_low, size;

  /* choose number of sons from small intervals, to keep 
   * random size variation within reasonable bounds
   *
   * those intervals wander down with increasing depth
   * --> same - sized partitioning of max_size, with
   * individual ranges rounded lower
   */
  size = (int) (((float) gmax_sons) / ((float) gdepth));
  if ( size == 0 ) size = 1;
  current_range_low = gmax_sons - size + 1;

  gnum_cols[0] = 1;
  gtree[0] = ( Assembly * ) calloc( 1, sizeof( Assembly ) );
  gtree[0][0].row = 0;
  gtree[0][0].col = 0;
  gtree[0][0].num_ao = 0;
  for ( i = 0; i <= gdepth; i++ ) {
    if ( i < gdepth ) {
      gnum_cols[i+1] = 0;
    }
    for ( j = 0; j < gnum_cols[i]; j++ ) {
      gtree[i][j].row = i;
      gtree[i][j].col = j;
      if ( i < gdepth ) {
	r = random() % 100;
	if ( r < gp_requires ) {
	  gtree[i][j].requires = random() % gresources;
	} else {
	  gtree[i][j].requires = -1;
	}
	r = random() % 100;
	if ( i == 0 ||
	     r < gp_has_sons ) {
	  gtree[i][j].num_sons = (random() % size) + current_range_low;
	  if ( gtree[i][j].num_sons > 0 ) {
	    gtree[i][j].start_son = gnum_cols[i+1];
	    gnum_cols[i+1] += gtree[i][j].num_sons;
	    gtree[i][j].end_son = gnum_cols[i+1] - 1;
	  }
	} else {
	  gtree[i][j].num_sons = 0;
	}
      } else {
	gtree[i][j].requires = -1;
	gtree[i][j].num_sons = 0;
      }
      gtree[i][j].num_ao = 0;
      gtree[i][j].num_ro = 0;
      if ( i > 0 ) {
	frow = gtree[i][j].part_of_row;
	fcol = gtree[i][j].part_of_col;
	for ( k = j + 1; k <= gtree[frow][fcol].end_son; k++ ) {
	  r = random() % 100;
	  if ( r < gp_assemble_order ) {
	    gtree[i][j].aop_row[gtree[i][j].num_ao] = i; 
	    gtree[i][j].aop_col[gtree[i][j].num_ao] = k; 
	    gtree[i][j].aof_row[gtree[i][j].num_ao] = frow; 
	    gtree[i][j].aof_col[gtree[i][j].num_ao] = fcol;
	    gtree[i][j].num_ao++;
	  }
	}
      }
    }
    if ( i < gdepth ) {
      gtree[i+1] = ( Assembly * ) calloc( gnum_cols[i+1], sizeof( Assembly ) );
      for ( j = 0; j < gnum_cols[i]; j++ ) {
	if ( gtree[i][j].num_sons == 0 ) continue;
	for ( k = gtree[i][j].start_son; k <= gtree[i][j].end_son; k++ ) {
	  gtree[i+1][k].part_of_row = i;
	  gtree[i+1][k].part_of_col = j;
	}
      }
    }
    current_range_low -= (size + 1);
    if ( current_range_low < 0 ) current_range_low = 0;
  }

}



void setup_transient_parts( void )

{

  int i, j, k, l, r, frow, fcol, aerow, aecol, r2, rerow, recol;

  for ( i = 1; i <= gdepth; i++ ) {
    for ( j = 0; j < gnum_cols[i]; j++ ) {
      frow = gtree[i][j].part_of_row;
      fcol = gtree[i][j].part_of_col;
      gtree[i][j].num_tp = 0;
      for ( k = 0; k < i; k++ ) {
	for ( l = 0; l < gnum_cols[k]; l++ ) {
	  if ( k == frow &&
	       l == fcol ) continue;/* not transient in own father */
	  if ( gtree[k][l].num_sons == 0 ) {
	    /* a part can have transient parts only if it has at least one
	     * true son
	     */
	    continue;
	  }
	  r = random() % 100;
	  if ( r < gp_transient_part ) {
	    gtree[i][j].tp_row[gtree[i][j].num_tp] = k;
	    gtree[i][j].tp_col[gtree[i][j].num_tp] = l;
	    gtree[i][j].num_tp++;
	    /* now give an assemble order to one son of upper part,
	     * such that the transient part needs to be added after all
	     */
	    r = random() % gtree[k][l].num_sons;
	    aerow = k + 1;
	    aecol = gtree[k][l].start_son + r;
	    gtree[i][j].aop_row[gtree[i][j].num_ao] = aerow;
	    gtree[i][j].aop_col[gtree[i][j].num_ao] = aecol;
	    gtree[i][j].aof_row[gtree[i][j].num_ao] = k;
	    gtree[i][j].aof_col[gtree[i][j].num_ao] = l;
	    gtree[i][j].num_ao++;
	    /* if wanted, give another son a remove order
	     */
	    if ( gtree[k][l].num_sons >= 2 ) {
	      r2 = random() % 100;
	      if ( r2 < gp_remove_order ) {
		while ( (r2 = random() % gtree[k][l].num_sons) == r );
		rerow = k + 1;
		recol = gtree[k][l].start_son + r;
		gtree[rerow][recol].rop_row[gtree[rerow][recol].num_ro] = i;
		gtree[rerow][recol].rop_col[gtree[rerow][recol].num_ro] = j;
		gtree[rerow][recol].rof_row[gtree[rerow][recol].num_ro] = k;
		gtree[rerow][recol].rof_col[gtree[rerow][recol].num_ro] = l;
		gtree[rerow][recol].num_ro++;
	      }
	    }
	  }
	}
      }
    }
  }

}









/* printing functions
 */






  


void print_objects( void )

{

  int i, j;

  for ( i = 0; i < gresources; i++ ) {
    printf("r%d ", i);
  }
  printf("- resource");

  printf("\n             ");
  for ( i = 0; i <= gdepth; i++ ) {
    for ( j = 0; j < gnum_cols[i]; j++ ) {
      printf("a-%d-%d ", i, j);
    }
  }
  printf("- assembly");

}



void print_part_of( void )

{

  int i, j, k;

  for ( i = 0; i < gdepth; i++ ) {
    for ( j = 0; j < gnum_cols[i]; j++ ) {
      if ( gtree[i][j].num_sons == 0 ) continue;
      for ( k = gtree[i][j].start_son; k <= gtree[i][j].end_son; k++ ) {
	printf("\n(part-of a-%d-%d a-%d-%d)", i+1, k, i, j);
      }
    }
  }

}



void print_assemble_order( void )

{

  int i, j, k;

  for ( i = 0; i <= gdepth; i++ ) {
    for ( j = 0; j < gnum_cols[i]; j++ ) {
      for ( k = 0; k < gtree[i][j].num_ao; k++ ) {
	printf("\n(assemble-order a-%d-%d a-%d-%d a-%d-%d)",
	       i, j, 
	       gtree[i][j].aop_row[k], gtree[i][j].aop_col[k],
	       gtree[i][j].aof_row[k], gtree[i][j].aof_col[k]);
      }
    }
  }

}



void print_requires( void )

{

  int i, j;

  for ( i = 0; i < gdepth; i++ ) {
    for ( j = 0; j < gnum_cols[i]; j++ ) {
      if ( gtree[i][j].requires != -1 ) {
	printf("\n(requires a-%d-%d r%d)", i, j, gtree[i][j].requires);
      }
    }
  }

}



void print_available( void )

{

  int i, j;

  for ( i = 0; i <= gdepth; i++ ) {
    for ( j = 0; j < gnum_cols[i]; j++ ) {
      if ( gtree[i][j].num_sons == 0 ) {
	printf("\n(available a-%d-%d)", i, j);
      }
    }
  }
  for ( j = 0; j < gresources; j++ ) {
    printf("\n(available r%d)", j);
  }

}



void print_transient_part( void )

{

  int i, j, k;

  for ( i = 0; i <= gdepth; i++ ) {
    for ( j = 0; j < gnum_cols[i]; j++ ) {
      for ( k = 0; k < gtree[i][j].num_tp; k++ ) {
	printf("\n(transient-part a-%d-%d a-%d-%d)",
	       i, j, 
	       gtree[i][j].tp_row[k],
	       gtree[i][j].tp_col[k]);
      }
    }
  }

}



void print_remove_order( void )

{

  int i, j, k;

  for ( i = 0; i <= gdepth; i++ ) {
    for ( j = 0; j < gnum_cols[i]; j++ ) {
      for ( k = 0; k < gtree[i][j].num_ro; k++ ) {
	printf("\n(remove-order a-%d-%d a-%d-%d a-%d-%d)",
	       i, j, 
	       gtree[i][j].rop_row[k], gtree[i][j].rop_col[k],
	       gtree[i][j].rof_row[k], gtree[i][j].rof_col[k]);
      }
    }
  }

}



void print( void )

{

  int i, j;

  for ( i = 0; i <= gdepth; i++ ) {
    for ( j = 0; j < gnum_cols[i]; j++ ) {
      printf("\nnode %d %d", i, j);
      printf("\nrequires %d", gtree[i][j].requires);
      printf("\nnum_sons %d", gtree[i][j].num_sons);
      printf("\nstart_son %d", gtree[i][j].start_son);
      printf("\nend_son %d", gtree[i][j].end_son);
      printf("\npart of row %d", gtree[i][j].part_of_row);
      printf("\npart of col %d", gtree[i][j].part_of_col);
      printf("\nnum ao %d", gtree[i][j].num_ao);
      printf("\n\n");
    }
  }



}





/* command line functions
 */










void usage( void )

{

  printf("\nusage:\n");

  printf("\nOPTIONS   DESCRIPTIONS\n\n");
  printf("-d <num>    depth of tree (minimal 0) (preset: %d)\n", gdepth);
  printf("-m <num>    max number of sons (minimal 1) (preset: %d)\n", gmax_sons);
  printf("-h <num>    p of any node has sons at all (preset: %d)\n", gp_has_sons);
  printf("-n <num>    number of resources (preset: %d)\n\n", gresources);

  printf("-a <num>    p of assemble order to any higher part (preset: %d)\n", 
	 gp_assemble_order);
  printf("-r <num>    p of requires any resource (preset: %d)\n", gp_requires);
  printf("-t <num>    p of transient part to any node in father rows (preset: %d)\n",
	 gp_transient_part);
  printf("-o <num>    p of remove order to transient part (preset: %d)\n\n",
	 gp_remove_order);

}



Bool process_command_line( int argc, char *argv[] )

{

  char option;

  while ( --argc && ++argv ) {
    if ( *argv[0] != '-' || strlen(*argv) != 2 ) {
      return FALSE;
    }
    option = *++argv[0];
    switch ( option ) {
    default:
      if ( --argc && ++argv ) {
	switch ( option ) {
	case 'd':
	  sscanf( *argv, "%d", &gdepth);
	  break;
	case 'm':
	  sscanf( *argv, "%d", &gmax_sons);
	  break;
	case 'n':
	  sscanf( *argv, "%d", &gresources);
	  break;
	case 'h':
	  sscanf( *argv, "%d", &gp_has_sons);
	  break;
	case 'a':
	  sscanf( *argv, "%d", &gp_assemble_order);
	  break;
	case 'r':
	  sscanf( *argv, "%d", &gp_requires);
	  break;
	case 't':
	  sscanf( *argv, "%d", &gp_transient_part);
	  break;	
	case 'o':
	  sscanf( *argv, "%d", &gp_remove_order);
	  break;
	default:
	  printf( "\n\nunknown option: %c entered\n\n", option );
	  return FALSE;
	}
      } else {
	return FALSE;
      }
    }
  }

  if ( gdepth < 1 ||
       gdepth > MAX_DEPTH ||
       gmax_sons < 1 ||
       gmax_sons > MAX_SONS ) {
    return FALSE;
  }

  return TRUE;

}






