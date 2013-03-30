


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
 * C code for generating fridge problems...
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
int gscrews, gfridges;





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
  gscrews = -1;
  gfridges = -1;

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
  printf("(define (problem fridge-s%d-f%d)", gscrews, gfridges);
  printf("\n(:domain fridge-domain)");

  printf("\n(:objects ");
  printf("\n");
  for ( i = 0; i < gfridges; i++ ) {
    for ( j = 0; j < gscrews; j++ ) {
      printf("s%d-%d ", i, j);
    }
  }
  printf("\n- screw");
  printf("\n");
  for ( i = 0; i < gfridges; i++ ) {
    printf("c%d-0 c%d-1 ", i, i);
  }
  printf("\n- compressor");
  printf("\n");
  for ( i = 0; i < gfridges; i++ ) {
    printf("f%d ", i);
  }
  printf("\n- fridge)");
 
  printf("\n(:init");
  for ( i = 0; i < gfridges; i++ ) {
    printf("\n(fridge-on f%d)", i);
    printf("\n(attached c%d-0 f%d)", i, i);
  }
  for ( i = 0; i < gfridges; i++ ) {
    for ( j = 0; j < gscrews; j++ ) {
      printf("\n(fits s%d-%d, c%d-0)", i, j, i);
      printf("\n(fits s%d-%d, c%d-1)", i, j, i);
      printf("\n(screwed s%d-%d)", i, j);
    }
  }
  printf("\n)");

  printf("\n(:goal");
  printf("\n(and");
  for ( i = 0; i < gfridges; i++ ) {
    printf("\n(attached c%d-1 f%d)", i, i);
    printf("\n(fridge-on f%d)", i);
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
  printf("-s <num>    number of screws per plane (minimal 1)\n");
  printf("-f <num>    number of fridges (minimal 1)\n\n");

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
	case 's':
	  sscanf( *argv, "%d", &gscrews );
	  break;
	case 'f':
	  sscanf( *argv, "%d", &gfridges );
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

  if ( gscrews < 1 ||
       gfridges < 1 ) {
    return FALSE;
  }

  return TRUE;

}

