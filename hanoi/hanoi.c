



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
 * C code for generating hanoi problems...
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


/* command line
 */
void usage( void );
Bool process_command_line( int argc, char *argv[] );




/* globals
 */

/* command line params
 */
int gobjects;






int main( int argc, char *argv[] )

{

  int i, j;

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

  /* now output problem in PDDL syntax
   */
  printf("\n\n\n");

  /* header
   */
  printf("(define (problem hanoi-%d)", gobjects);
  printf("\n(:domain hanoi)");

  printf("\n(:objects peg1 peg2 peg3 ");
  for ( i = 0; i < gobjects; i++ ) {
    printf("d%d ", i + 1);
  }
  printf(")");

  printf("\n(:init");
  for ( i = 0; i < gobjects; i++ ) {
    printf("\n(smaller peg1 d%d)", i+1);
  }
  for ( i = 0; i < gobjects; i++ ) {
    printf("\n(smaller peg2 d%d)", i+1);
  }
  for ( i = 0; i < gobjects; i++ ) {
    printf("\n(smaller peg3 d%d)", i+1);
  }
  for ( i = 0; i < gobjects - 1; i++ ) {
    for ( j = i + 1; j < gobjects; j++ ) {
      printf("\n(smaller d%d d%d)", j+1, i+1);
    }
  }
  printf("\n(clear peg2)
          \n(clear peg3)
          \n(clear d1)");

  printf("\n(on d%d peg1)", gobjects);
  for ( i = gobjects-1; i >= 1; i-- ) {
    printf("\n(on d%d d%d)", i, i+1);
  }
  printf("\n)");

  printf("\n(:goal");
  printf("\n(and ");
  printf("\n(on d%d peg3)", gobjects);
  for ( i = gobjects-1; i >= 1; i-- ) {
    printf("\n(on d%d d%d)", i, i+1);
  }
  printf("\n)");
  printf("\n)");
  printf("\n)");

  printf("\n\n\n");

  exit( 0 );

}
  
  









/* random problem generation functions
 */





/* command line functions
 */










void usage( void )

{

  printf("\nusage:\n");

  printf("\nOPTIONS   DESCRIPTIONS\n\n");
  printf("-n <num>    number of discs\n\n");

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
	case 'n':
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

