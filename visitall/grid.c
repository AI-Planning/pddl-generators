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
 * C code for generating randomozied grid problems...
 */

/* 
 * The visitall domain seems to be very hard, especially for
 * seq-opt. One of the reasons is that this generator requires the
 * planner to visit all (hence the name) squares. This version of the
 * generator submitted by Nir Lipovetzky adds a new flag to randomly
 * pickup cells in the resulting grid of size nxn
 */

/* Since the organizers of the Answer Set Programming Competition
   decided to add the visit-all domain in the edition of 2013, a new
   flag is added now to this generator: a ratio of the number of
   unavailable locations */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* defs  */
typedef unsigned char Bool;
#define TRUE 1
#define FALSE 0

/* prototypes */
void usage( void );
Bool process_command_line( int argc, char *argv[] );

/* globals */
int x; 				/* size of the grid: x*x */
float r=1.0; 			/* ratio of cells in the goal state */
int u=0; 			/* number of unavailable locations */
int seed = -1;

/* main body */
int main( int argc, char *argv[] )
{
  int i, j;
  int initial_row, initial_col;

  /* command line treatment, first preset values */
  x = -1;

  if ( argc == 1 || ( argc == 2 && *++argv[0] == '?' ) ) {
    usage();
    exit( 1 );}

  if ( !process_command_line( argc, argv ) ) {
    usage();
    exit( 1 );}

  // if seed still unspecified, initialize from the time
  if (seed == -1) {
    seed = (int)time(NULL);
  }

  /* make sure that the number of unavailable locations does not
     exceed the area of the grid */
  if (u>=x*x) {
    printf ("\n You cannot arrange more than %d unavailable locations in a %dx%d grid\n\n", u-1, x, x);
    exit (1);
  }

  /* create a mask with the usable locations */
  char* mask = malloc (x*x);

  /* make them all usable by default */
  for (i=0;i<x*x;mask[i++]=1);

  /* initialize the random generator */
  static char randstate[2048];
  if (!initstate(seed, randstate, 256)) {
    printf(" \n Fatal Error while initializing the random generator\n\n");
    exit (1);
  }

  /* and now, create up to u holes in the grid */
  i=0;
  while (i<u) {
    int hole = random ()%(x*x);	/* pick up a random location */
    printf ("%d\n", hole);
    if (mask[hole] &&		/* just in case it is currently reachable */
	(hole != (x+1)*x/2)) {	/* and it is not the initial location */
      mask[hole] = 0;		/* make it unreachable */
      i++;			/* and proceed with the next one */
    }
  }

  /* now output problem in PDDL syntax */

  /* header */
  printf("(define (problem grid-%d)", x);
  printf("\n(:domain grid-visit-all)");

  printf("\n(:objects \n");
  for ( i = 0; i < x; i++ ) {
    for ( j = 0; j < x; j++ ) {
      if (mask[i*x+j]) 		/* in case this location is reachable */
	printf("\tloc-x%d-y%d\n", i, j);
    }
  }

  printf("- place \n        ");
  printf("\n)");

  /* initial state */
  printf("\n(:init");

  do {
      initial_row = random()%x;
      initial_col = random()%x;
  } while (!mask[initial_row*x + initial_col]);

  printf("\n\t(at-robot loc-x%d-y%d)", initial_row, initial_col);
  printf("\n\t(visited loc-x%d-y%d)", initial_row, initial_col);

  printf("\n");
  
  for ( i = 0; i < x; i++ ) {
    for ( j = 0; j < x; j++ ) {
      // can go up
      if (( i != 0) && (mask[i*x+j]) && (mask[(i-1)*x+j]))
	printf("\t(connected loc-x%d-y%d loc-x%d-y%d)\n ", i, j, i - 1, j);
      // can go down
      if (( i != x - 1) && (mask[i*x+j]) && (mask[(i+1)*x+j]))
	printf("\t(connected loc-x%d-y%d loc-x%d-y%d)\n ", i, j, i + 1, j);
      // can go left
      if (( j != 0 ) && (mask[i*x+j]) && (mask[i*x+j-1]))
	printf("\t(connected loc-x%d-y%d loc-x%d-y%d)\n ", i, j, i, j - 1);
      // can go right
      if (( j != x - 1 ) && (mask[i*x+j]) && (mask[i*x+j+1]))
	printf("\t(connected loc-x%d-y%d loc-x%d-y%d)\n ", i, j, i, j + 1);
    }
  }

  printf("\n)");

  /* goal state */
  printf("\n(:goal");
  printf("\n(and \n");

  for ( i = 0; i < x; i++ ) {
    for ( j = 0; j < x; j++ ) {
      /* throw a dice to decide wether (i,j) shall be included in the
         goal state or not. With ratio r=1, all are included as in the
         original version */
      if ((mask[i*x+j] && random() % (x*x) < (r*x*x)) || ((i==initial_row) && (j==initial_col)))
        printf("\t(visited loc-x%d-y%d)\n", i, j);
    }
  }

  printf(")");
  printf("\n)");  
  printf("\n)\n");

  exit( 0 );
}

/* printing fns
 */
void usage( void )
{

  printf ("\n usage:\n");

  printf ("\n OPTIONS   DESCRIPTIONS\n\n");
  printf (" -n <num>    size of square grid\n");
  printf (" -r <num>    ratio of cells in the goal state\n");
  printf (" -u <num>    number of unavailable locations ---which are randomly arranged\n\n");
  printf (" -s <num>    random seed (optional)\n");
}

Bool process_command_line( int argc, char *argv[] )
{
  char option;

  while ( --argc && ++argv ) {

    if ( *argv[0] != '-' || strlen(*argv) != 2 )
      return FALSE;

    option = *++argv[0];
    switch ( option ) {
    default:
      if ( --argc && ++argv ) {
	switch ( option ) {
	case 'n':
	  sscanf( *argv, "%d", &x );
	  break;
	case 'r':
	  sscanf( *argv, "%f", &r );
	  break;
	case 'u':
	  sscanf( *argv, "%d", &u );
	  break;
    case 's':
      sscanf( *argv, "%d", &seed);
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
  
  return ( x > 1 || ( r >= 0.0 && r <= 1.0 ) );
}
