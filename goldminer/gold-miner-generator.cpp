/***************************************************************************
 *   Copyright (C) 2008 by Madhu Srinivasan,,,   *
 *   srinivma@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <utility>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <vector>

using namespace std;

//it is important not to change the 
//order of these enums
enum{SOFT,HARD,ROBOT,GOLD_MINE,BOMB_SUPPLY};
const int CLEAR=9;


//grid
int **bomber_world;
//location of goal
int goal_i, goal_j;
//goal path
vector<pair<int,int> > goal_path;

//command line params
int num_rows, num_cols;

//forward decl.
void usage();
bool process_command_line(int argc, char *argv[]);
void show_grid();

void init_robot_location();
void init_bomb_supply_location();
void init_gold_location();
void init_rocks();
void guarantee_solution();

void output_untyped_strips();
void output_typed_strips();
void print_connectivity_predicates();
void print_position_predicates();
void print_goal_path_predicates();

int main(int argc, char *argv[])
{
  /* command line treatment, first preset values
   */
  num_rows = -1;
  num_cols = -1;


  if ( argc == 1 || ( argc == 2 && *++argv[0] == '?' ) ) {
    usage();
    exit( 1 );
  }
  if ( !process_command_line( argc, argv ) ) {
    usage();
    exit( 1 );
  }
  
  //allocate space
  bomber_world = new int*[num_rows];
  for(int i=0;i<num_rows;++i){
   bomber_world[i]=new int[num_cols];
   //initialize to -1
   for(int j=0;j<num_cols;++j){
    bomber_world[i][j]=CLEAR;
   }
  }
  
  init_robot_location();
  init_bomb_supply_location();
  init_rocks();
  init_gold_location();
  
  guarantee_solution();
  
  //output the problem
  //in untyped strips
  //output_untyped_strips();
  
  //output the problem 
  //in typed strips
  output_typed_strips();
  
  //only for debugging
  //show_grid();
  return EXIT_SUCCESS;
}



//robot is at a random row of the 
//first column
void init_robot_location(){
 //a random number between [0,num_rows-1]
 bomber_world[random()%num_rows][0]=ROBOT;
}

//bomb location is at a random row of the
//first column
void init_bomb_supply_location(){
 //a random number between [0,num_rows-1]
 int row;
 while(bomber_world[(row=random()%num_rows)][0]!=CLEAR);
 bomber_world[row][0]=BOMB_SUPPLY;
}

//gold mine is at a random row at the 
//last column
void init_gold_location(){
 //a random number between [0,num_rows-1]
 bomber_world[(goal_i=random()%num_rows)][(goal_j=num_cols-1)]=GOLD_MINE;
}

void init_rocks(){
 //initialize with rocks
 for(int i=0;i<num_rows;++i){
   for(int j=1;j<num_cols;++j){
    bomber_world[i][j]=random()%2;
   }
  }
}

void guarantee_solution(){
 enum{UP,LEFT,DOWN};
 int curr_i=goal_i;
 int curr_j=goal_j;
 
 int move=random()%3;
 int num_steps;
 while(curr_j>1){

 switch(move){
  
  case UP:
   //cout<<"In case UP:"<<endl;
   num_steps=(random()%(curr_i+1))+2;
   while((--num_steps>0)&&curr_i>0){
    bomber_world[--curr_i][curr_j]=SOFT;
    goal_path.push_back(make_pair(curr_i,curr_j));
   }
   
   //dont move DOWN now
   move=random()%2?UP:LEFT;
   break;
  
  case DOWN:
   //cout<<"In case DOWN:"<<endl;
   num_steps=(random()%(num_rows-curr_i))+2;
   while((--num_steps>0)&&curr_i<num_rows-1){
    bomber_world[++curr_i][curr_j]=SOFT;   
    goal_path.push_back(make_pair(curr_i,curr_j));
   }
   
   //dont move UP now
   move=random()%2?DOWN:LEFT;
   break;
  
  case LEFT:
   //cout<<"In case LEFT:"<<endl;
   num_steps=3;//(random()%(curr_j))+3;
   while((--num_steps>0)&&curr_j>1){
    bomber_world[curr_i][--curr_j]=SOFT;
    goal_path.push_back(make_pair(curr_i,curr_j));
   }   
   
   //move anywhere
   move=random()%3;
   break;
  default:
   cout<<"Error:guarantee_solution() ";
   exit(9);
 };
 }
}

void output_typed_strips(){
  /* now output problem in PDDL syntax
   */

  /* header
   */
  printf("(define (problem typed-bomberman-rows%d-cols%d)",
	 num_rows, num_cols);
  printf("\n(:domain gold-miner-typed)");

  printf("\n(:objects ");
  for (int i=0; i < num_rows; ++i) {
    printf("\n        ");
    for (int j=0; j < num_cols; ++j) {
      printf("f%d-%df ", i, j);
    }
  }
  printf(" - LOC\n)");
  
  //definition of intitial state
  printf("\n(:init");
  printf("\n(arm-empty)");
  print_connectivity_predicates();
  print_position_predicates();
  print_goal_path_predicates();
  printf("\n)");
  
  //what is the goal
    printf("\n(:goal");
  printf("\n(holds-gold)");
  printf("\n)\n");
}

void output_untyped_strips(){
  /* now output problem in PDDL syntax
   */

  /* header
   */
  printf("(define (problem untyped-bomberman-rows%d-cols%d)",
	 num_rows, num_cols);
  printf("\n(:domain untyped-bomberman)");

  printf("\n(:objects ");
  for (int i=0; i < num_rows; ++i) {
    printf("\n        ");
    for (int j=0; j < num_cols; ++j) {
      printf("f%d-%df ", i, j);
    }
  }
  printf("\n)");
  
  //definition of intitial state
  printf("\n(:init");
  printf("\n(arm-empty)");
  print_connectivity_predicates();
  print_position_predicates();
  print_goal_path_predicates();
  printf("\n)");
  
  //what is the goal
    printf("\n(:goal");
  printf("\n(holds-gold)");
  printf("\n)");
  printf("\n)");
  printf("\n\n\n");
}


void print_goal_path_predicates(){
// for(int i=0;i<goal_path.size();++i){
//   printf("\n(goal-path f%d-%df)",goal_path[i].first,goal_path[i].second);
// }
}


void print_position_predicates(){
  for (int i = 0; i < num_rows; ++i ) {
    for (int j = 0; j < num_cols; ++j ) {
      switch(bomber_world[i][j]){
       case ROBOT:
          printf("\n(robot-at f%d-%df)",i,j);
          printf("\n(clear f%d-%df)",i,j);
          break;
       case GOLD_MINE:
          printf("\n(gold-at f%d-%df)",i,j);
          printf("\n(soft-rock-at f%d-%df)",i,j);
          break;
       case BOMB_SUPPLY:
          printf("\n(bomb-at f%d-%df)",i,j);
		  printf("\n(laser-at f%d-%-df)",i,j);
          printf("\n(clear f%d-%df)",i,j);
          break;
       case SOFT:
          printf("\n(soft-rock-at f%d-%df)",i,j);
          break;
       case HARD:
          printf("\n(hard-rock-at f%d-%df)",i,j);
          break;
       case CLEAR:
          printf("\n(clear f%d-%df)",i,j);
          break;
       default:
          printf("\nERROR !!!");
          exit(1);
      };
    }
  }
}


void print_connectivity_predicates(){
  for (int i = 0; i < num_rows; ++i ) {
    for (int j = 0; j < num_cols-1; ++j ) {
      printf("\n(connected f%d-%df f%d-%df)",i,j,i,j+1);
    }
  }
  for (int i = 0; i < num_rows-1; ++i ) {
    for (int j = 0; j < num_cols; ++j ) {
      printf("\n(connected f%d-%df f%d-%df)",i,j,i+1,j);
    }
  }
  
  for (int i = 0; i < num_rows; ++i ) {
    for (int j = 1; j < num_cols; ++j ) {
      printf("\n(connected f%d-%df f%d-%df)",i,j,i,j-1);
    }
  }
  
  for (int i = 1; i < num_rows; ++i ) {
    for (int j = 0; j < num_cols; ++j ) {
      printf("\n(connected f%d-%df f%d-%df)",i,j,i-1,j);
    }
  }
}



void show_grid(){
 cout<<endl;
 for(int i=0;i<num_rows;++i){
   for(int j=0;j<num_cols;++j){
    cout<<bomber_world[i][j]<<" ";
   }
   cout<<endl;
  }
  cout<<endl;
}

void usage() {
    printf("\nusage:\n");

    printf("\nOPTIONS   DESCRIPTIONS\n\n");
    printf("-r <num>    rows (minimal 1)\n");
    printf("-c <num>    columns (minimal 1)\n\n");
    printf("-s <num>    random seed (positive integer, optional)\n\n");
}

bool process_command_line( int argc, char *argv[] ) {
    int seed = -1;

    char option;
    while ( --argc && ++argv ) {
        if ( *argv[0] != '-' || strlen(*argv) != 2 ) {
            return false;
        }

        option = *++argv[0];
        switch ( option ) {
            default:
                if ( --argc && ++argv ) {
                    switch ( option ) {
                        case 'r':
                            sscanf( *argv, "%d", &num_rows);
                            break;
                        case 'c':
                            sscanf( *argv, "%d", &num_cols);
                            break;
                        case 's':
                            sscanf( *argv, "%d", &seed);
                            break;
                        default:
                            printf( "\n\nunknown option: %c entered\n\n", option );
                            return false;
                    }
                } else {
                    return false;
                }
        }
    }

    if ( num_rows < 1 || num_cols < 1 ) {
        return false;
    }

    /* seed the random() function
     */
    if (seed == -1) {
        seed = (int)time(NULL);
    }

    srandom(seed);

    return true;
}
