


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
 * C code for generating randomozied mprime problems...
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
void create_random_spaces( void );
void create_random_fuels( void );
void create_random_locations( void );
void print_random_fuels( void );
void print_random_spaces( void );
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
int glocations, gmax_fuel, gmax_space, gvehicles, gcargos;

/* random values
 */
int *gspace, *gfuel, *gv_origin, *gc_origin, *gc_destin;





int main( int argc, char *argv[] )

{

  int i, j;

  /* command line treatment, first preset values
   */
  glocations = -1;
  gmax_fuel = -1;
  gmax_space = -1;
  gvehicles = -1;
  gcargos = -1;

  if ( argc == 1 || ( argc == 2 && *++argv[0] == '?' ) ) {
    usage();
    exit( 1 );
  }
  if ( !process_command_line( argc, argv ) ) {
    usage();
    exit( 1 );
  }

  create_random_locations();
  create_random_fuels();
  create_random_spaces();

  /* now output problem in PDDL syntax
   */

  /* header
   */
  printf("(define (problem strips-mprime-l%d-f%d-s%d-v%d-c%d)", 
	 glocations, gmax_fuel, gmax_space, gvehicles, gcargos);
  printf("\n(:domain mprime-strips)");

  printf("\n(:objects ");
  for ( i = 0; i < gmax_fuel + 1; i++ ) printf("f%d ", i);
  printf("- fuel");
  printf("\n          ");
  for ( i = 0; i < gmax_space + 1; i++ ) printf("s%d ", i);
  printf("- space");
  printf("\n          ");
  for ( i = 0; i < glocations; i++ ) printf("l%d ", i);
  printf("- location");
  printf("\n          ");
  for ( i = 0; i < gvehicles; i++ ) printf("v%d ", i);
  printf("- vehicle");
  printf("\n          ");
  for ( i = 0; i < gcargos; i++ ) printf("c%d ", i);
  printf("- cargo)");

  printf("\n(:init");
  for ( i = 0; i < glocations; i++ ) {
    for ( j = 0; j < glocations; j++ ) {
      if ( i == j ) continue;
      printf("\n(not-equal l%d l%d)", i, j);
    }
  }
  for ( i = 0; i < gmax_fuel; i++ ) {
    printf("\n(fuel-neighbor f%d f%d)", i, i+1);
  }
  for ( i = 0; i < gmax_space; i++ ) {
    printf("\n(space-neighbor s%d s%d)", i, i+1);
  }
  for ( i = 0; i < glocations - 1; i++ ) {
    printf("\n(conn l%d l%d)", i, i+1);
    printf("\n(conn l%d l%d)", i+1, i);
  }
  printf("\n(conn l%d l0)", glocations - 1);
  printf("\n(conn l0 l%d)", glocations - 1);
  print_random_fuels();
  print_random_spaces();
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





void create_random_spaces( void )

{

  int r, i;

  gspace = ( int * ) calloc( gvehicles, sizeof( int ) );

  for ( i = 0; i < gvehicles; i++ ) {
    r = random() % gmax_space;
    gspace[i] = r + 1;
  }

}



void create_random_fuels( void )

{

  int r, i;

  gfuel = ( int * ) calloc( glocations, sizeof( int ) );

  for ( i = 0; i < glocations; i++ ) {
    r = random() % (gmax_fuel + 1);
    gfuel[i] = r;
  }

}



void create_random_locations( void )

{

  int r, i;

  gv_origin = ( int * ) calloc( gvehicles, sizeof( int ) );
  gc_origin = ( int * ) calloc( gcargos, sizeof( int ) );
  gc_destin = ( int * ) calloc( gcargos, sizeof( int ) );

  for ( i = 0; i < gvehicles; i++ ) {
    r = random() % glocations;
    gv_origin[i] = r;
  }
  for ( i = 0; i < gcargos; i++ ) {
    r = random() % glocations;
    gc_origin[i] = r;
  }
  for ( i = 0; i < gcargos; i++ ) {
    r = random() % glocations;
    gc_destin[i] = r;
  }

}







/* printing fns
 */


  


void print_random_fuels( void )

{

  int i;

  for ( i = 0; i < glocations; i++ ) {
    printf("\n(has-fuel l%d f%d)", i, gfuel[i]);
  }
 
}



void print_random_spaces( void )

{

  int i;

  for ( i = 0; i < gvehicles; i++ ) {
    printf("\n(has-space  v%d s%d)", i, gspace[i]);
  }

}



void print_random_origins( void )

{

  int i;

  for ( i = 0; i < gvehicles; i++ ) {
    printf("\n(at v%d l%d)", i, gv_origin[i]);
  }
  for ( i = 0; i < gcargos; i++ ) {
    printf("\n(at c%d l%d)", i, gc_origin[i]);
  }

}



void print_random_destins( void )

{

  int i;

  for ( i = 0; i < gcargos; i++ ) {
    printf("\n(at c%d l%d)", i, gc_destin[i]);
  }

}




/* command line functions
 */










void usage( void ) {
    printf("\nusage:\n");

    printf("\nOPTIONS   DESCRIPTIONS\n\n");
    printf("-l <num>    number of locations (minimal 2)\n\n");
    printf("-f <num>    max amount of fuel (minimal 1)\n");
    printf("-s <num>    max amount of space (minimal 1)\n\n");
    printf("-v <num>    number of vehicles (minimal 1)\n");
    printf("-c <num>    number of cargos (minimal 1)\n\n");
    printf("-r <num>    random seed (optional)\n\n");
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
                            sscanf( *argv, "%d", &glocations );
                            break;
                        case 'f':
                            sscanf( *argv, "%d", &gmax_fuel );
                            break;
                        case 's':
                            sscanf( *argv, "%d", &gmax_space );
                            break;
                        case 'v':
                            sscanf( *argv, "%d", &gvehicles );
                            break;
                        case 'c':
                            sscanf( *argv, "%d", &gcargos );
                            break;
                        case 'r':
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

    if ( glocations < 2 ||
            gmax_space < 1 ||
            gmax_fuel < 1 ||
            gvehicles < 1 ||
            gcargos < 1 ) {
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

