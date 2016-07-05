
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
 * 
 * -------------------------------------
 * Modified for the learning part of IPC-2011
 *********************************************************************/



/* 
 * C code for generating gripper problems
 */





#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>






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
int grooms;
int grobots;


int main( int argc, char *argv[] )

{

  int i;

  /* command line treatment, first preset values
   */
  gobjects = -1;
  grooms = -1;
  grobots = -1;

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

  /* header
   */
  printf("(define (problem gripper-%d-%d-%d)", grobots, grooms, gobjects);
  printf("\n(:domain gripper-strips)");

  printf("\n(:objects ");
  for ( i = 0; i < grobots; i++ ) printf("robot%d ", i + 1);
  printf("- robot\n");

  for ( i = 0; i < grobots; i++ ) {
    printf("rgripper%d ", i + 1);
    printf("lgripper%d ", i + 1);
  }
  printf("- gripper\n");

  for ( i = 0; i < grooms; i++ ) printf("room%d ", i + 1);
  printf("- room\n");

  for ( i = 0; i < gobjects; i++ ) printf("ball%d ", i + 1);
  printf("- object)");

  printf("\n(:init");

  for ( i = 0; i < grobots; i++ ) {
    printf("\n(at-robby robot%d room%d)", i + 1, 1+(int)(random() / (((double)RAND_MAX + 1)/ grooms)));
    printf("\n(free robot%d rgripper%d)", i + 1, i + 1);
    printf("\n(free robot%d lgripper%d)", i + 1, i + 1);
  }
  
  for ( i = 0; i < gobjects; i++ ) printf("\n(at ball%d room%d)", i + 1, 1+(int)(random() / (((double)RAND_MAX + 1)/ grooms)));
  printf("\n)");

  printf("\n(:goal");
  printf("\n(and");
  for ( i = 0; i < gobjects; i++ ) printf("\n(at ball%d room%d)", i + 1, 1+(int)(random() / (((double)RAND_MAX + 1)/ grooms)));
  printf("\n)");
  printf("\n)");

  printf("\n)\n");

  exit( 0 );

}
  
  









/* random problem generation functions
 */





/* command line functions
 */










void usage( void ) {
    printf("\nusage:\n");

    printf("\nOPTIONS   DESCRIPTIONS\n\n");
    printf("-n <num>    number of robots (minimal 1)\n\n");
    printf("-r <num>    number of rooms (minimal 1)\n\n");
    printf("-o <num>    number of balls (minimal 1)\n\n");
    printf("-s <num>    random seed (optional)\n\n");
}

Bool process_command_line( int argc, char *argv[] ) {
    int seed = -1;
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
                            sscanf( *argv, "%d", &grobots );
                            break;
                        case 'r':
                            sscanf( *argv, "%d", &grooms );
                            break;
                        case 'o':
                            sscanf( *argv, "%d", &gobjects );
                            break;
                        case 's':
                            sscanf(*argv, "%d", &seed);
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

    if ( grobots < 1 ) {
        return FALSE;
    }

    if ( grooms < 1 ) {
        return FALSE;
    }

    if ( gobjects < 1 ) {
        return FALSE;
    }

    /* seed the random() function
    */
    if (seed == -1) {
        seed = (int)time(NULL);
    }

    srandom(seed);

    return TRUE;
}

