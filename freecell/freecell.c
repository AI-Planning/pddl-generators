


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
 * C code for generating randomized freecell problems...
 */





#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>



#define MAX_STACK 20



/* data structures ... (ha ha)
 */
typedef unsigned char Bool;
#define TRUE 1
#define FALSE 0



/* helpers
 */
void create_random_stacks( void );
void print_stacks( void );

void print_objects( void );
void print_suit( int s );
void print_card( int m );
void print_statics( void );
void print_dynamics( void );



/* command line
 */
void usage( void );
Bool process_command_line( int argc, char *argv[] );
Bool setup_suit_sizes( int vec );




/* globals
 */

/* command line params
 */
int gcells, gcols, gsuits, *gsuit_size, gstacks;

/* random configuration
 */
int **gstack_suit, **gstack_member;
int *gnum_stack;






int main( int argc, char *argv[] )

{

  int i;

  /* command line treatment, first preset values
   */
  gcells = -1;
  gcols = -1;
  gsuits = -1;
  gstacks = -1;

  if ( argc == 1 || ( argc == 2 && *++argv[0] == '?' ) ) {
    usage();
    exit( 1 );
  }
  if ( !process_command_line( argc, argv ) ) {
    usage();
    exit( 1 );
  }

  create_random_stacks();
  if ( 0 ) {
    print_stacks();
  }

  /* now output problem in PDDL syntax
   */

  /* header
   */
  printf("(define (problem freecell-f%d-c%d-s%d-i%d", 
	 gcells, gcols, gsuits, gstacks);
  for ( i = 0; i < gsuits; i++ ) {
    printf("-%d%d", i, gsuit_size[i]);
  }
  printf("\n)(:domain freecell)");
  print_objects();

  printf("\n(:init");
  print_statics();
  print_dynamics();
  printf("\n)");

  printf("\n(:goal");
  printf("\n(and");
  for ( i = 0; i < gsuits; i++ ) {
    printf("\n(HOME ");
    print_suit( i );
    print_card( gsuit_size[i] - 1 );
    printf(")");
  }
  printf("\n)");
  printf("\n)");

  printf("\n)\n");

  exit( 0 );

}
  
  









/* random problem generation functions
 */






void create_random_stacks( void )

{

  int i, j, r1, r2, r3;
  int *num_processed;
  Bool **processed;

  gnum_stack = ( int * ) calloc( gstacks, sizeof( int ) );
  gstack_suit = ( int ** ) calloc( gstacks, sizeof( int * ) );
  gstack_member = ( int ** ) calloc( gstacks, sizeof( int * ) );
  for ( i = 0; i < gstacks; i++ ) {
    gnum_stack[i] = 0;
    gstack_suit[i] = ( int * ) calloc( MAX_STACK, sizeof( int ) );
    gstack_member[i] = ( int * ) calloc( MAX_STACK, sizeof( int ) );
  }
  num_processed = ( int * ) calloc( gsuits, sizeof( int ) );
  processed = ( Bool ** ) calloc( gsuits, sizeof( Bool * ) );
  for ( i = 0; i < gsuits; i++ ) {
    num_processed[i] = 0;
    processed[i] = ( Bool * ) calloc( gsuit_size[i], sizeof( Bool ) );
  }
  for ( i = 0; i < gsuits; i++ ) {
    for ( j = 0; j < gsuit_size[i]; j++ ) {
      processed[i][j] = FALSE;
    }
  }

  while ( TRUE ) {
    for ( i = 0; i < gsuits; i++ ) {
      if ( num_processed[i] < gsuit_size[i] ) break;
    }
    if ( i == gsuits ) break;
    while ( TRUE ) {
      r1 = random() % gsuits;
      if ( num_processed[r1] < gsuit_size[r1] ) break;
    }
    while ( TRUE ) {
      r2 = random() % gsuit_size[r1];
      if ( !processed[r1][r2] ) break;
    }
    r3 = random() % gstacks;
    gstack_suit[r3][gnum_stack[r3]] = r1;
    gstack_member[r3][gnum_stack[r3]] = r2;
    gnum_stack[r3]++;
    processed[r1][r2] = TRUE;
    num_processed[r1]++;
  }

}



void print_stacks( void )

{

  int i, j;

  for ( i = 0; i < gstacks; i++ ) {
    printf("\nstack %3d: ", i);
    for ( j = 0; j < gnum_stack[i]; j++ ) {
      printf("%3d %3d - ", gstack_suit[i][j], gstack_member[i][j]);
    }
  }


}








/* printing fns
 */


void print_objects( void )

{

  int i, j, max_suit_size;

  printf("\n(:objects ");

  for ( i = 0; i < gsuits; i++ ) {
    printf("\n          ");
    print_suit( i );
    printf("0");
    for ( j = 0; j < gsuit_size[i]; j++ ) {
      printf(" ");
      print_suit( i );
      print_card( j );
    }
  }
  printf("\n - card");

  printf("\n          ");
  for ( i = 0; i <= gcells; i++ ) {
    printf("CELLN%d ", i);
  }
  printf("\n - cellnum");

  printf("\n          ");
  for ( i = 0; i <= gcols; i++ ) {
    printf("COLN%d ", i);
  }
  printf("\n - colnum");

  max_suit_size = -1;
  for ( i = 0; i < gsuits; i++ ) {
    if ( max_suit_size == -1 ||
	 gsuit_size[i] > max_suit_size ) {
      max_suit_size = gsuit_size[i];
    }
  }
  printf("\n          ");
  for ( i = 0; i <= max_suit_size; i++ ) {
    printf("N%d ", i);
  }
  printf("\n - num");

  printf("\n          ");
  for ( i = 0; i < gsuits; i++ ) {
    printf(" ");
    print_suit( i );
  }
  printf("\n - suit");
   
  printf("\n)");

}



void print_suit( int s )

{

  switch( s ) {
  case 0:
    printf("C");
    break;
  case 1:
    printf("H");
    break;
  case 2:
    printf("S");
    break;
  case 3:
    printf("D");
    break;
  default:
    printf("\nunknown suit\n\n");
    exit( 1 );
  }

}



void print_card( int m )

{

  if ( m == 0 ) {
    printf("A");
  } else {
    printf("%d", m+1);
  }

}



void print_statics( void )

{

  int i, j, max_suit_size;

  for ( i = 0; i < gsuits; i++ ) {
    printf("\n(VALUE ");
    print_suit( i );
    printf("0 N0)");
    for ( j = 0; j < gsuit_size[i]; j++ ) {
      printf("\n(VALUE ");
      print_suit( i );
      print_card( j );
      printf(" N%d)", j + 1);
    }
  }

  for ( i = 0; i < gcells; i++ ) {
    printf("\n(CELLSUCCESSOR CELLN%d CELLN%d)", i+1, i);
  }
  for ( i = 0; i < gcols; i++ ) {
    printf("\n(COLSUCCESSOR COLN%d COLN%d)", i+1, i);
  }
  max_suit_size = -1;
  for ( i = 0; i < gsuits; i++ ) {
    if ( max_suit_size == -1 ||
	 gsuit_size[i] > max_suit_size ) {
      max_suit_size = gsuit_size[i];
    }
  }
  for ( i = 0; i < max_suit_size; i++ ) {
    printf("\n(SUCCESSOR N%d N%d)", i+1, i);
  }

  for ( i = 0; i < gsuits; i++ ) {
    printf("\n(SUIT ");
    print_suit( i );
    printf("0 ");
    print_suit( i );
    printf(")");
    for ( j = 0; j < gsuit_size[i]; j++ ) {
      printf("\n(SUIT ");
      print_suit( i );
      print_card( j );
      printf(" ");
      print_suit( i );
      printf(")");
    }
  }

  for ( i = 1; i < gsuit_size[0]; i++ ) {
    if ( gsuits >= 2 &&
	 gsuit_size[1] > i + 1 ) {
      printf("\n(CANSTACK ");
      print_suit( 0 );
      print_card( i );
      printf(" ");
      print_suit( 1 );
      print_card( i + 1 );
      printf(")");
    }
    if ( gsuits >= 4 &&
	 gsuit_size[3] > i + 1 ) {
      printf("\n(CANSTACK ");
      print_suit( 0 );
      print_card( i );
      printf(" ");
      print_suit( 3 );
      print_card( i + 1 );
      printf(")");
    }
  }
  if ( gsuits >= 2 ) {
    for ( i = 1; i < gsuit_size[1]; i++ ) {
      if ( gsuit_size[0] > i + 1 ) {
	printf("\n(CANSTACK ");
	print_suit( 1 );
	print_card( i );
	printf(" ");
	print_suit( 0 );
	print_card( i + 1 );
	printf(")");
      }
      if ( gsuits >= 3 &&
	   gsuit_size[2] > i + 1 ) {
	printf("\n(CANSTACK ");
	print_suit( 1 );
	print_card( i );
	printf(" ");
	print_suit( 2 );
	print_card( i + 1 );
	printf(")");
      }
    }
  }
  if ( gsuits >= 3 ) {
    for ( i = 1; i < gsuit_size[2]; i++ ) {
      if ( gsuit_size[1] > i + 1 ) {
	printf("\n(CANSTACK ");
	print_suit( 2 );
	print_card( i );
	printf(" ");
	print_suit( 1 );
	print_card( i + 1 );
	printf(")");
      }
      if ( gsuits >= 4 &&
	   gsuit_size[3] > i + 1 ) {
	printf("\n(CANSTACK ");
	print_suit( 2 );
	print_card( i );
	printf(" ");
	print_suit( 3 );
	print_card( i + 1 );
	printf(")");
      }
    }
  }
  if ( gsuits >= 4 ) {
    for ( i = 1; i < gsuit_size[3]; i++ ) {
      if ( gsuit_size[0] > i + 1 ) {
	printf("\n(CANSTACK ");
	print_suit( 3 );
	print_card( i );
	printf(" ");
	print_suit( 0 );
	print_card( i + 1 );
	printf(")");
      }
      if ( gsuit_size[2] > i + 1 ) {
	printf("\n(CANSTACK ");
	print_suit( 3 );
	print_card( i );
	printf(" ");
	print_suit( 2 );
	print_card( i + 1 );
	printf(")");
      }
    }
  }
      
}



void print_dynamics( void )

{

  int i, j;

  for ( i = 0; i < gsuits; i++ ) {
    printf("\n(HOME ");
    print_suit( i );
    printf("0)");
  }

  printf("\n(CELLSPACE CELLN%d)", gcells);

  printf("\n(COLSPACE COLN%d)", gcols - gstacks);

  for ( i = 0; i < gstacks; i++ ) {
    if ( gnum_stack[i] >= 1 ) {
      printf("\n\n(BOTTOMCOL ");
      print_suit( gstack_suit[i][0] );
      print_card( gstack_member[i][0] );
      printf(")");
      for ( j = 1; j < gnum_stack[i]; j++ ) {
	printf("\n(ON ");
	print_suit( gstack_suit[i][j] );
	print_card( gstack_member[i][j] );
	printf(" ");
	print_suit( gstack_suit[i][j-1] );
	print_card( gstack_member[i][j-1] );
	printf(")");
      }
      printf("\n(CLEAR ");
      print_suit( gstack_suit[i][gnum_stack[i]-1] );
      print_card( gstack_member[i][gnum_stack[i]-1] );
      printf(")");
    }
  }

}








/* command line functions
 */

void usage( void ) {
  printf("\nusage:\n");

  printf("\nOPTIONS   DESCRIPTIONS\n\n");
  printf("-f <num>    number of (free)cells (minimal 0)\n");
  printf("-c <num>    number of cols (minimal 1)\n");
  printf("-s <num>    number of suits (minimal 1)\n");
  printf("-0 .. -3 <num>    suit sizes\n");
  printf("-i <num>    number of initial stacks (minimal 1)\n\n");
  printf("-r <num>    random seed (optional)\n\n");
}

Bool process_command_line( int argc, char *argv[] ) {
    int seed = -1;
    char option;
    gsuit_size = ( int * ) calloc( 4, sizeof( int ) );

    while ( --argc && ++argv ) {
        if ( *argv[0] != '-' || strlen(*argv) != 2 ) {
            return FALSE;
        }

        option = *++argv[0];

        switch ( option ) {
            default:
                if ( --argc && ++argv ) {
                    switch ( option ) {
                        case 'f':
                            sscanf( *argv, "%d", &gcells );
                            break;
                        case 'c':
                            sscanf( *argv, "%d", &gcols );
                            break;
                        case 's':
                            sscanf( *argv, "%d", &gsuits );
                            break;
                        case '0':
                            sscanf( *argv, "%d", &(gsuit_size[0]) );
                            break;
                        case '1':
                            sscanf( *argv, "%d", &(gsuit_size[1]) );
                            break;
                        case '2':
                            sscanf( *argv, "%d", &(gsuit_size[2]) );
                            break;
                        case '3':
                            sscanf( *argv, "%d", &(gsuit_size[3]) );
                            break;
                        case 'i':
                            sscanf( *argv, "%d", &gstacks );
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

    if ( gcells < 0 ||
            gcols < 1 ||
            gsuits < 1 ||
            gsuits > 4 ||
            gstacks < 1 ) {
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


