

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
 * C code for generating randomized schedule problems...
 */





#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>




#define MAX_LENGTH 256




/* data structures ... (ha ha)
 */
typedef unsigned char Bool;
#define TRUE 1
#define FALSE 0



/* helpers
 */
char gashape[3][MAX_LENGTH], gcolour[4][MAX_LENGTH];
char gwidth[3][MAX_LENGTH], ganorient[2][MAX_LENGTH];
char gsurface[3][MAX_LENGTH];

void create_random_I( void );
void create_random_G( void );

void print_random_I( void );
void print_random_G( void );



/* command line
 */
void usage( void );
Bool process_command_line( int argc, char *argv[] );




/* globals
 */

/* command line params
 */
int gparts, gashapes, gcolours, gwidths, ganorients;
int gp_G_cylindrical, gp_I_colour, gp_G_colour, gp_I_hole, gp_G_hole, gp_G_surface;


/* random values
 */
int *gI_ashape, *gI_surface, *gI_colour, *gI_width, *gI_anorient;
int *gG_ashape, *gG_surface, *gG_colour, *gG_width, *gG_anorient;




int main( int argc, char *argv[] )

{

  int i;

  /* seed the random() function
   */
  struct timeb tp;
  ftime( &tp );
  srandom( tp.millitm );

  sprintf( gashape[0], "CYLINDRICAL");
  sprintf( gashape[1], "CIRCULAR");
  sprintf( gashape[2], "OBLONG");  
  sprintf( gcolour[0], "BLUE");
  sprintf( gcolour[1], "YELLOW");
  sprintf( gcolour[2], "RED");
  sprintf( gcolour[3], "BLACK");
  sprintf( gwidth[0], "ONE");
  sprintf( gwidth[1], "TWO");
  sprintf( gwidth[2], "THREE");
  sprintf( ganorient[0], "FRONT");
  sprintf( ganorient[1], "BACK");
  sprintf( gsurface[0], "POLISHED");
  sprintf( gsurface[1], "ROUGH");
  sprintf( gsurface[2], "SMOOTH");

  /* command line treatment, first preset values
   */
  gparts = -1;
  gashapes = 0;
  gcolours = 2;
  gwidths = 2;
  ganorients = 2;
  gp_G_cylindrical = 80;
  gp_I_colour = 50;
  gp_G_colour = 80;
  gp_I_hole = 50;
  gp_G_hole = 80;
  gp_G_surface = 50;

  if ( argc == 1 || ( argc == 2 && *++argv[0] == '?' ) ) {
    usage();
    exit( 1 );
  }
  if ( !process_command_line( argc, argv ) ) {
    usage();
    exit( 1 );
  }

  gI_ashape = ( int * ) calloc( gparts, sizeof( int ) );
  gI_surface = ( int * ) calloc( gparts, sizeof( int ) );
  gI_colour = ( int * ) calloc( gparts, sizeof( int ) );
  gI_width = ( int * ) calloc( gparts, sizeof( int ) );
  gI_anorient = ( int * ) calloc( gparts, sizeof( int ) );

  gG_ashape = ( int * ) calloc( gparts, sizeof( int ) );
  gG_surface = ( int * ) calloc( gparts, sizeof( int ) );
  gG_colour = ( int * ) calloc( gparts, sizeof( int ) );
  gG_width = ( int * ) calloc( gparts, sizeof( int ) );
  gG_anorient = ( int * ) calloc( gparts, sizeof( int ) );

  create_random_I();
  create_random_G();

  /* now output problem in PDDL syntax
   */
  printf("\n\n\n");

  /* header
   */
  printf("(define (problem schedule-p%d-s%d-c%d-w%d-o%d)", 
	 gparts, gashapes, gcolours, gwidths, ganorients);
  printf("\n(:domain schedule)");

  printf("\n(:objects ");
  for ( i = 0; i < gparts; i++ ) {
    printf("\n    P%d", i);
  }
  printf("\n - part");
  if ( gashapes >= 1 ) {
    for ( i = 1; i < gashapes+1; i++ ) {
      printf("\n    %s", gashape[i]);
    }
    printf("\n - ashape");
  }
  for ( i = 0; i < gcolours; i++ ) {
    printf("\n    %s", gcolour[i]);
  }
  printf("\n - colour");
  for ( i = 0; i < gwidths; i++ ) {
    printf("\n    %s", gwidth[i]);
  }
  printf("\n - width");
  for ( i = 0; i < ganorients; i++ ) {
    printf("\n    %s", ganorient[i]);
  }
  printf("\n - anorient");
  printf("\n)");

  printf("\n(:init");
  for ( i = 0; i < gcolours; i++ ) {
    printf("\n(HAS-PAINT IMMERSION-PAINTER %s)", gcolour[i]);
    printf("\n(HAS-PAINT SPRAY-PAINTER %s)", gcolour[i]);
  }
  for ( i = 0; i < ganorients; i++ ) {
    printf("\n(CAN-ORIENT DRILL-PRESS %s)", ganorient[i]);
    printf("\n(CAN-ORIENT PUNCH %s)", ganorient[i]);
  }
  for ( i = 0; i < gwidths; i++ ) {
    printf("\n(HAS-BIT DRILL-PRESS %s)", gwidth[i]);
    printf("\n(HAS-BIT PUNCH %s)", gwidth[i]);
  }
  print_random_I();
  printf("\n)");

  printf("\n(:goal");
  printf("\n(and");
  print_random_G();
  printf("\n)");
  printf("\n)");

  printf("\n)");

  printf("\n\n\n");

  exit( 0 );

}
  
  









/* random problem generation functions
 */












void create_random_I( void )

{

  int i, r;

  for ( i = 0; i < gparts; i++ ) {
    r = random() % (gashapes + 1);
    gI_ashape[i] = r;
    r = random() % 3;
    gI_surface[i] = r;
    r = random() % 100;
    if ( r < gp_I_colour ) {
      r = random() % gcolours;
      gI_colour[i] = r;
    } else {
      gI_colour[i] = -1;
    }
    r = random() % 100;
    if ( r < gp_I_hole ) {
      r = random() % gwidths;
      gI_width[i] = r;
      r = random() % ganorients;
      gI_anorient[i] = r;
    } else {
      gI_width[i] = -1;
      gI_anorient[i] = -1;
    }
  }

}



void create_random_G( void )

{

  int i, r;

  for ( i = 0; i < gparts; i++ ) {
    r = random() % 100;
    if ( r < gp_G_cylindrical ) {
      gG_ashape[i] = 0;
    } else {
      gG_ashape[i] = -1;
    }
    r = random() % 100;
    if ( r < gp_G_surface ) {
      r = random() % 3;
      gG_surface[i] = r;
    } else {
      gG_surface[i] = -1;
    }
    r = random() % 100;
    if ( r < gp_G_colour ) {
      r = random() % gcolours;
      gG_colour[i] = r;
    } else {
      gG_colour[i] = -1;
    }
    r = random() % 100;
    if ( r < gp_G_hole ) {
      r = random() % gwidths;
      gG_width[i] = r;
      r = random() % ganorients;
      gG_anorient[i] = r;
    } else {
      gG_width[i] = -1;
      gG_anorient[i] = -1;
    }
  }

}










/* printing fns
 */




void print_random_I( void )

{

  int i;

  for ( i = 0; i < gparts; i++ ) {
    printf("\n(SHAPE P%d %s)", i, gashape[gI_ashape[i]]);
    printf("\n(SURFACE-CONDITION P%d %s)", i, gsurface[gI_surface[i]]);
    if ( gI_colour[i] > -1 ) {
      printf("\n(PAINTED P%d %s)", i, gcolour[gI_colour[i]]);
    }
    if ( gI_width[i] > -1 ) {
      printf("\n(HAS-HOLE P%d %s %s)", i, 
	     gwidth[gI_width[i]],
	     ganorient[gI_anorient[i]]);
    }
    printf("\n(TEMPERATURE P%d COLD)", i);
  }

}



void print_random_G( void )

{

  int i;

  for ( i = 0; i < gparts; i++ ) {
    if ( gG_ashape[i] > -1 ) {
      printf("\n(SHAPE P%d %s)", i, gashape[gG_ashape[i]]);
    }
    if ( gG_surface[i] > -1 ) {
      printf("\n(SURFACE-CONDITION P%d %s)", i, gsurface[gG_surface[i]]);
    }
    if ( gG_colour[i] > -1 ) {
      printf("\n(PAINTED P%d %s)", i, gcolour[gG_colour[i]]);
    }
    if ( gG_width[i] > -1 ) {
      printf("\n(HAS-HOLE P%d %s %s)", i, 
	     gwidth[gG_width[i]],
	     ganorient[gG_anorient[i]]);
    }
  }

}


  





/* command line functions
 */










void usage( void )

{

  printf("\nusage:\n");

  printf("\nOPTIONS   DESCRIPTIONS\n\n");
  printf("-p <num>    number of parts (minimal 1)\n");
  printf("-s <num>    number of shapes (preset: %d, maximal: 2)\n", gashapes);
  printf("-c <num>    number of colors (preset: %d, minimal 1, maximal: 4)\n",
	 gcolours);
  printf("-w <num>    number of widths (preset: %d, minimal 1, maximal: 3)\n",
	 gwidths);
  printf("-o <num>    number of orientations (preset: %d, minimal 1, maximal: 2)\n\n",
	 ganorients);
  printf("-Q <num>    probability cylindrical goal (preset: %d)\n", gp_G_cylindrical);
  printf("-W <num>    probability of colour in I (preset: %d)\n", gp_I_colour);
  printf("-E <num>    probability of colour in G (preset: %d)\n\n", gp_G_colour);
  printf("-R <num>    probability of hole in I (preset: %d)\n", gp_I_hole);
  printf("-T <num>    probability of hole in G (preset: %d)\n", gp_G_hole);
  printf("-Y <num>    probability of surface condition in G (preset: %d)\n\n", 
	 gp_G_surface);

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
	case 'p':
	  sscanf( *argv, "%d", &gparts );
	  break;
	case 's':
	  sscanf( *argv, "%d", &gashapes );
	  break;
	case 'c':
	  sscanf( *argv, "%d", &gcolours );
	  break;
	case 'w':
	  sscanf( *argv, "%d", &gwidths );
	  break;
	case 'o':
	  sscanf( *argv, "%d", &ganorients );
	  break;
	case 'Q':
	  sscanf( *argv, "%d", &gp_G_cylindrical );
	  break;
	case 'W':
	  sscanf( *argv, "%d", &gp_I_colour );
	  break;
	case 'E':
	  sscanf( *argv, "%d", &gp_G_colour );
	  break;
	case 'R':
	  sscanf( *argv, "%d", &gp_I_hole );
	  break;
	case 'T':
	  sscanf( *argv, "%d", &gp_G_hole );
	  break;
	case 'Y':
	  sscanf( *argv, "%d", &gp_G_surface );
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

  if ( gparts < 1 ||
       gashapes > 2 ||
       gcolours < 1 ||
       gcolours > 4 ||
       gwidths < 1 ||
       gwidths > 3 ||
       ganorients < 1 ||
       ganorients > 2 ) {
    return FALSE;
  }

  return TRUE;

}


