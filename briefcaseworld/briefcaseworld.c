


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
 * C code for generating random briefcaseworld problems
 */





#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>






/* data structures ... (ha ha)
 */
typedef unsigned char Bool;
#define TRUE 1
#define FALSE 0



/* helpers
 */
void create_random_locations( void );
void print_random_origins( void );
void print_random_destins( void );



/* command line
 */
void usage( void );
Bool process_command_line( int argc, char *argv[] );




/* globals
 */

/* command line params
 */
int gobjects;

/* random values
 */
int gorigin, *go_origin, gdestin, *go_destin;





int main( int argc, char *argv[] )

{

  int i;

  /* seed the random() function
   */
  struct timeb tp;
  ftime( &tp );
  srandom( tp.millitm );


  /* command line treatment, first preset values
   */
  gobjects = -1;

  if ( argc == 1 || ( argc == 2 && *++argv[0] == '?' ) ) {
    usage();
    exit( 1 );
  }
  if ( !process_command_line( argc, argv ) ) {
    usage();
    exit( 1 );
  }

  create_random_locations();

  /* now output problem in PDDL syntax
   */
  printf("\n\n\n");

  /* header
   */
  printf("(define (problem briefcase-o%d)", gobjects);
  printf("\n(:domain briefcase)");

  printf("\n(:objects ");
  for ( i = 0; i < gobjects + 1; i++ ) printf("l%d ", i);
  printf("- location");
  printf("\n          ");
  for ( i = 0; i < gobjects; i++ ) printf("o%d ", i);
  printf("- portable)");

  printf("\n(:init");
  print_random_origins();
  printf("\n)");

  printf("\n(:goal");
  printf("\n(and");
  print_random_destins();
  printf("\n)");
  printf("\n)");

  printf("\n)");

  printf("\n\n\n");

  exit( 0 );

}
  
  









/* random problem generation functions
 */







void create_random_locations( void )

{

  int r, i;

  go_origin = ( int * ) calloc( gobjects, sizeof( int ) );
  go_destin = ( int * ) calloc( gobjects, sizeof( int ) );

  for ( i = 0; i < gobjects; i++ ) {
    r = random() % (gobjects + 1);
    go_origin[i] = r;
  }
  for ( i = 0; i < gobjects; i++ ) {
    r = random() % (gobjects + 1);
    go_destin[i] = r;
  }
  gorigin = random() % (gobjects + 1);
  gdestin = random() % (gobjects + 1);

}







/* printing fns
 */


  




void print_random_origins( void )

{

  int i;

  for ( i = 0; i < gobjects; i++ ) {
    printf("\n(at o%d l%d)", i, go_origin[i] );
  }
  printf("\n(is-at l%d)", gorigin);

}



void print_random_destins( void )

{

  int i;

  for ( i = 0; i < gobjects; i++ ) {
    printf("\n(at o%d l%d)", i, go_destin[i] );
  }
  printf("\n(is-at l%d)", gdestin);

}




/* command line functions
 */










void usage( void )

{

  printf("\nusage:\n");

  printf("\nOPTIONS   DESCRIPTIONS\n\n");
  printf("-o <num>    number of objects (minimal 1)\n\n");

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
	case 'o':
	  sscanf( *argv, "%d", &gobjects );
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

  if ( gobjects < 1 ) {
    return FALSE;
  }

  return TRUE;

}

