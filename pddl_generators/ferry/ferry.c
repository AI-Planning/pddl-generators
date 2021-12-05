


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
 * C code for generating random ferry problems
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
int gcars, glocs;

/* random values
 */
int gorigin, *go_origin, *go_destin;





int main( int argc, char *argv[] )

{

  int i, j;

  /* command line treatment, first preset values
   */
  gcars = 0;
  glocs = -1;

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

  /* header
   */
  printf("(define (problem ferry-l%d-c%d)", glocs, gcars);
  printf("\n(:domain ferry)");

  printf("\n(:objects ");
  for ( i = 0; i < glocs; i++ ) printf("l%d ", i);
  printf("\n          ");
  for ( i = 0; i < gcars; i++ ) printf("c%d ", i);
  printf("\n)");

  printf("\n(:init");
  for ( i = 0; i < glocs; i++ ) printf("\n(location l%d)", i);
  for ( i = 0; i < gcars; i++ ) printf("\n(car c%d)", i);
  for ( i = 0; i < glocs - 1; i++ ) {
    for ( j = i + 1; j < glocs; j++ ) {
      printf("\n(not-eq l%d l%d)", i, j);
      printf("\n(not-eq l%d l%d)", j, i);
    }
  }
  printf("\n(empty-ferry)");
  
  print_random_origins();
  printf("\n)");

  printf("\n(:goal");
  printf("\n(and");
  print_random_destins();
  printf("\n)");
  printf("\n)");

  printf("\n)\n");


  exit( 0 );

}
  
  









/* random problem generation functions
 */







void create_random_locations( void )

{

  int r, i;

  go_origin = ( int * ) calloc( gcars, sizeof( int ) );
  go_destin = ( int * ) calloc( gcars, sizeof( int ) );

  for ( i = 0; i < gcars; i++ ) {
    r = random() % glocs;
    go_origin[i] = r;
  }
  for ( i = 0; i < gcars; i++ ) {
    r = random() % glocs;
    go_destin[i] = r;
  }
  gorigin = random() % glocs;

}







/* printing fns
 */


  




void print_random_origins( void )

{

  int i;

  for ( i = 0; i < gcars; i++ ) {
    printf("\n(at c%d l%d)", i, go_origin[i] );
  }
  printf("\n(at-ferry l%d)", gorigin);

}



void print_random_destins( void )

{

  int i;

  for ( i = 0; i < gcars; i++ ) {
    printf("\n(at c%d l%d)", i, go_destin[i] );
  }

}




/* command line functions
 */

void usage( void ) {
    printf("\nusage:\n");

    printf("\nOPTIONS   DESCRIPTIONS\n\n");
    printf("-l <num>    number of locations (minimal 1)\n");
    printf("-c <num>    number of cars\n\n");
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
                        case 'l':
                            sscanf( *argv, "%d", &glocs );
                            break;
                        case 'c':
                            sscanf( *argv, "%d", &gcars );
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

    if ( glocs < 1 || gcars < 0 ) {
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

