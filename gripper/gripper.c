


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
 * C code for generating gripper problems
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

  /* now output problem in PDDL syntax
   */
  printf("\n\n\n");

  /* header
   */
  printf("(define (problem gripper-%d)", gobjects);
  printf("\n(:domain gripper-strips)");

  printf("\n(:objects ");
  printf(" rooma roomb left right ");
  for ( i = 0; i < gobjects; i++ ) printf("ball%d ", i + 1);
  printf(")");

  printf("\n(:init");
  printf("\n(room rooma)");
  printf("\n(room roomb)");
  printf("\n(gripper left)");
  printf("\n(gripper right)");
  for ( i = 0; i < gobjects; i++ ) printf("\n(ball ball%d)", i + 1);
  printf("\n(free left)");
  printf("\n(free right)");
  for ( i = 0; i < gobjects; i++ ) printf("\n(at ball%d rooma)", i + 1);
  printf("\n(at-robby rooma)");
  printf("\n)");

  printf("\n(:goal");
  printf("\n(and");
  for ( i = 0; i < gobjects; i++ ) printf("\n(at ball%d roomb)", i + 1);  
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
  printf("-n <num>    number of balls (minimal 1)\n\n");

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

