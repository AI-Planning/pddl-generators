#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <getopt.h>
#include <math.h>

#define FALSE 0
#define TRUE 1
#define LONG 2
#define SHORT 1
#define NOCONN 0

#define MAX_PREFERENCES   100

#define PREF_START 'A'
#define PREF_BASE ('Z'-'A'+1)


typedef unsigned short int Bool;


/*Automatic problem generator: TPP domain
//Occhi Andrea, Periti Francesca
//Intelligenza Artificiale B - aa 2004/2005
/*/

//global variables

int deb;    // Serial Number of Search instance. Used to keep track of
// the nodes visited during each instance
unsigned short product, truck, market, depot, money_level, product_level, truck_capacity;


unsigned short fuel_level;
int *truck_fuel;
int seed;

static int use_fuel = 0;
static int use_money = 0;
static int use_capacity = 1;
static int use_far_connections = 0;
static int use_cheap = 0;
static int simple_pref = 0;
static int constraints_pref = 0;

int incompatibility;
//counters
int i, j;
//used for generating boolean values
int boolean=2;

Bool all_connections = FALSE;
//durative problem
int durative;
//data structure representing problem
int **map; //represent the market's map
int *map_depot; //represent depot connection to the market
int *map_depot_length; //represent the length of the connection between the i-th depot and the map_depot[i]-th market
int start_money;
int **prod_dist;//if <0 the product is cheap!
int **incomp;
int *truck_pos;
int *goal;
int *total_quantity;
int *has_goal;
int verbose;
char *domain_name;
char *problem_name;
//data structure for dijkstra
int *Q;     /* coda per Dijkstra */
int *pi;        /* elenco nodi visitati */
int *flag;      /* indica i nodi gia` estratti */

int used_pref[MAX_PREFERENCES];


void print_ds();
int generate_number(int number);
int generate_boolean();


int ceil_div(int x, int y) {
    assert(x >= 0 && y >= 0);
    return (x + y - 1) / y;
}


//**********************MAP GENERATION********************\\
// Search for a path between from and to
int search(int from, int to, int deb, int num_markets) {
  int k;
  //printf("\tLevel %i Searching %i,%i... \n", deb,from+1,to+1);
  //printf("%i,%i=%i | ",from+1, to+1, map[from][to]);
  if (map[from][to]) {/*printf("(%i,%i)=DC\n", from+1,to+1);*/ return TRUE;}
  for (k=0;k<num_markets;k++) {
    //printf("\n\t(k=%i) Searching (%i,%i)... ",k,from+1,k+1);
    if (from != k && (map[from][k] < deb+2) && (map[from][k] != 0)) {
      //printf("\n\t\tVisited (%i,%i) ",from+1,k+1);

      // Mark the node as visited
      map[from][k] = deb + 2;
      map[k][from] = deb + 2;
      if (search(k,to, deb, num_markets)) return TRUE;
    }
  }
  return FALSE;
}


// Removes an edge from graph
int remove_conn(int num_markets, int num_depots){
  int from;
  int to;

  if (simple_pref || constraints_pref)
    return FALSE;

  do {
    from = generate_number(num_markets) - 1;
    to = generate_number(num_markets ) - 1;

  } while (from == to );

  //printf("From: m%i To: m%i Linked Status Before: %i\n", from +1, to+1, map[from][to]);
  // Already cut
  if (map[from][to] == NOCONN || map[to][from] == NOCONN) return FALSE;

  // Remove the link in both directions
  map[from][to] = NOCONN;
  map[to][from] = NOCONN;

  // Check if all the markets are still reacheable
  int i,j;
  for (i=0;i< num_markets;i++) {
    for (j=i;j< num_markets;j++) {
      //if (i!= j) printf("Can I go from %i to %i? ",i+1,j+1);
      if (j !=i && !search(i, j, deb++, num_markets)) {
    // If 2 markets aren't reacheable, restore the link

    map[from][to] = SHORT;
    map[to][from] = SHORT;
    if (verbose>=3) printf("\tCouldn't remove connection from m%i and m%i\n", from+1, to+1);
    return FALSE;
      }
    }
  }
  if (verbose>=3) printf("\tRemoved connection from m%i and m%i\n", from+1, to+1);
  return TRUE;

  //remove self connection
  for (i=0;i< num_markets;i++) {
    map[i][i]=NOCONN;
  }

}

//initialize data structure (problem structure too)
void initialize_data(){

  //memory allocation

  map = (int **)calloc(market, sizeof(int*));
  for (i=0;i<market;i++)
    map[i] = (int *) calloc(market, sizeof(int));

  map_depot=(int *)calloc(depot, sizeof(int));
  map_depot_length=(int *)calloc(depot, sizeof(int));

  int start_money=money_level;

  prod_dist = (int **)calloc(market, sizeof(int*));
  for (i=0;i<market;i++)
    prod_dist[i] = (int *)calloc(product,sizeof(int));

  incomp = (int **)calloc(product, sizeof(int*));
  for (i=0;i<product;i++)
    incomp[i] = (int *)calloc(product, sizeof(int));

  truck_pos =(int *)calloc(truck, sizeof(int));

  goal = (int *)calloc(product, sizeof(int));

  if (use_fuel)
    truck_fuel =(int *)calloc(truck, sizeof(int));

  total_quantity =(int *)calloc(product, sizeof(int));
  //to detect if every prodict has at least one goal
  has_goal=(int *) calloc(product, sizeof(int));
  //data initialization

  for (i = 0; i < market-1; i++)
    for (j = i+1; j < market; j++)
      {
    map[i][j] = map[j][i] = SHORT;
      }

  for (i=0;i<depot;i++)
    map_depot_length[i] = NOCONN;

  for (i=0;i<product;i++)
    for (j = 0;j<product;j++)
      incomp[i][j] = FALSE;

  memset(used_pref, 0, MAX_PREFERENCES * sizeof(Bool));

}

//make map
void make_map(int num_markets, int num_depots){
  // Make map fully connected
  //initialize_map(num_markets, num_depots);

  //int i,j;

  // Remove few edges..
  int num_max, quantity;

  if (num_markets > 2)
    num_max = generate_number((num_markets * (num_markets -1)) / 2);
  else num_max = 0;
  if (verbose>=3) printf("\tTrying to remove %i connections...\n",num_max);

  deb = 0;


  if (!all_connections)
    {
      for (i=0;i<num_max;i++) {
    remove_conn(num_markets, num_depots);       //while (!remove_conn())  // If it fails, do not retry
      }
    }


  // Write distance and connected
  for (i=0;i<num_markets;i++)
    for (j = i;j<num_markets;j++)
      if (map[i][j])
    {
      if (use_far_connections)
        quantity = generate_boolean();
      else
        quantity = FALSE;

      if (quantity == TRUE)
        {
          map[i][j]=LONG;
          map[j][i]=LONG;
        }
      else
        {
          map[i][j]=SHORT;
          map[j][i]=SHORT;
        }
    }

  // Attach depots to markets
  for (i=0;i<num_depots;i++) {

    int chosen_market = rand() % (num_markets) + 1;

    if (use_far_connections)
      quantity = generate_boolean();
    else
      quantity = FALSE;

    if (quantity == TRUE)
      {
    map_depot[i]=chosen_market;
    map_depot_length[i] = LONG;
      }
    else
      {
    map_depot[i]=chosen_market;
    map_depot_length[i] = SHORT;
      }
  }
}

//*********** MAP GENERATION ENDS ************************\\

//prints usage options to video
void usage()
{
  printf("\nUsage: gen-TPP [options] file");

  printf("\n\nCommand line options:\n");
  printf("-h\t\tDisplay this help\n");
  printf("-s <num>\tSet the seed for the random number generator\n");
  printf("-p <num>\tSet the number of product (default 2)\n");
  printf("-m <num>\tSet the number of market (default 4)\n");
  printf("-t <num>\tSet the number of truck (default 2)\n");
  printf("-d <num>\tSet the number of depot (default 2)\n");
  printf("-l <num>\tSet the maximum level of goods (default 10)\n");
  /*
  printf("-c Completely connected markets\n");
  printf("-v be verbose (more v, more verbosity!)\n");
  printf("-q be quiet (no output)\n");
  printf("--fuel generate problem with fuel usage\n");
  printf("-F Set the capacity of the truck's petrol tank (default 10 - only with --fuel)\n");
  printf("--nocapacity generate problem without loaded levels\n");
  printf("-T Set the capacity of the truck (default 10 - only with --capacity)\n");
  printf("--money generate problem with money usage\n");
  printf("--farnear generate problem with \"far\" and \"near\" connections\n");
  printf("--cheap generate problem with cheap products\n");
  printf("-N <domain-name> specify the name of the domain (default TPP-Propositional)\n");
  printf("--name Set problem name\n");
  printf("-D generate durative problem \n");
  printf("-I generate problem without incompatibilities\n");
  printf("--simple-pref generate problem with simple preferences\n");
  printf("--constraints-pref generate problem with constraints preferences\n");
  */
}

//returns a boolean value (o or 1)
int generate_boolean()
{
  int num = rand() % 2;
  return num;
}

//returns a random value smaller than "number" and different from 0
int generate_number(int number)
{
  int num;
  if (number <= 0)
    {
      printf("Bugs in generate_number.\n");
      printf("Number:  %d", number);
      print_ds();
      exit(1);
    }
  if (number == 1)
    return (1);

  num = rand() % number + 1;

  return num;

}

//returns a random value smaller than number, different from "from" and 0
int generate_number_different(int number,int from)
{
  int num;
  if ((number <=0)||(number==from))
    {
      printf("Problems in generate_number_different()\n");
      printf("number= %d, from= %d\n",number,from);
      print_ds();
      exit(1);
    }
  if (number=1)
    return (1);
  do
    {
      num=rand()%number;
    }
  while(num==from || num==0);
  return num;

}


//modify the data structure distributing products among market
void make_product_dist()
{
  int m;
  int is_cheap = 0;
  int max_forsale_level = 0;

  if (market > 2)
    // divido per 2 perchè un mercato vende un prodotto con probabilità del 50%
    max_forsale_level = ceil_div(product_level, (market / 2));

  else
    max_forsale_level = product_level;

  if (verbose >= 2)
    printf("\nMax forsale level = %d\n", max_forsale_level);

  m = generate_number(market) - 1;

  for(i=0; i<product; i++)
    {
      int total_product=0;
      for(j=0; j<market; j++)
    {

      if (total_product >= product_level)
        break;

      if((generate_boolean() || (j == m)))
        {
          int p_level;

          p_level = generate_number(max_forsale_level);

          if (p_level > (product_level - total_product))
        p_level = product_level - total_product;

          total_product += p_level;

          prod_dist[j][i] = p_level;

          if (use_cheap)
        is_cheap = generate_boolean();

          if(is_cheap)
        {
          prod_dist[j][i] = -p_level;
        }
          if (verbose>=2)
        {
          if(is_cheap) printf("\tProduct goods%d is for sale market market%d in quantity level%d and is cheap!\n", i+1, j+1, p_level );
          else printf("\tProduct goods%d is for sale market market%d in quantity level%d\n", i+1, j+1, p_level );
        }
        }
      else
        {
          //product pr is not for sale in market m
          prod_dist[j][i]=0;
          if (verbose>=2) printf("\tProduct goods%d is not for sale in market m%d\n",i+1,j+1);
        }
    }

      total_quantity[i] = total_product;
    }
}

int extract_min( int *Q )
{
  int   i;
  int   min;

  for( i = 0; i < market; i++ )
    if( Q[i] != -1 && Q[i] != 0 && flag[i] == 0 )
      break;

  if( i == market )
    return( -1 );

  min = i;

  for( i = min+1; i < market; i++ )
    if( Q[i] != -1 && Q[i] != 0 && flag[i] == 0 && Q[i] < Q[min] )
      min = i;

  return( min );
}

//return the max lenght of the minimum path form market s to the other market
int dijkstra( int s )
{
  int i, u, n;
  s--;
  flag = (int *)calloc(market, sizeof( int ));
  Q = (int *)calloc(market, sizeof( int ));
  pi = (int *)calloc(market, sizeof( int ));
  if( flag == NULL || Q == NULL|| pi == NULL )
    {
      printf( "Impossibile allocare la memoria\n" );
      print_ds();
      exit( 1 );
    }

  for( i = 0; i < market; i++ )
    Q[i] = -1;

  Q[s] = 0;
  flag[s] = 1;
  pi[s] = s;

  /*
    cost initializing: Q[i] = -1 for every node not connected to s,
    otherwise  Q[i] = map[s][i]
  */
  for( i = 0; i < market; i++ )
    if( map[s][i] != 0 )
      {
    Q[i] = map[s][i];
    pi[i] = s;
      }

  n = 1;
  while( n < market )
    {
      //if (verbose>=3) printf( "\tPasso %d\n", n );

      u = extract_min( Q );
      if( u == -1 )
    break;

      //if (verbose>=3) printf( "\tEstratto: %d\n", u+1 );

      flag[u] = 1;

      for( i = 0; i < market; i++ )
    if( map[u][i] != 0 )
      if( Q[i] > Q[u] + map[u][i] || Q[i] == -1 )
        {
          Q[i] = Q[u] + map[u][i];
          pi[i] = u;
        }
      n++;

    }
  int max_cost=0;
  for( i = 0; i < market; i++ )
    if (Q[i]>max_cost)
      max_cost=Q[i];
  free( flag );
  free( Q );
  free( pi );
  return (max_cost);
}

void make_money()
{

  int money_prod=0;
  for (i=0;i<product;i++)
    money_prod+=goal[i]*2;

  if (verbose>=3) printf("\tThe upper bound evaluation of purchasing product is: %d\n", money_prod);
  int money_travel=0;
  //    inizio del calcolo euristico sul consumo di carburante
  //    le semplificazioni sono:
  //    1. tutti i prodotti sono incompatibili tra loro
  //    2. suppongo di dover prendere tutto il prodotto dal mercato con il cammino minimo più alto
  //    3. trasporto il prodotto una unità alla volta dal mercato "più lontano"

  //    heuristic calculus about fuel consumption
  //    we simplify using:
  //    1. all product are incompatible
  //    2. I take all the product in the market with maximum minimum path  (find using dijkstra algorithm)
  //    3. I carry the product one unit at a time

  for (i=0;i<depot;i++)
    {
      int prod_quantity=0;

      for (j=0;j<product;j++)
    prod_quantity += goal[j];

      money_travel+=prod_quantity * 2 * (dijkstra(map_depot[i]) + map_depot_length[i]);
    }
  money_level=money_prod+money_travel;
  //start_money=money_level;
  if (verbose>=3) printf("\tThe upper bound evaluation of transporting product is:  %d\n",money_travel);
}
//modify the data structure generating incompatibilities
void make_incomp()
{
  //incompatibilities among products
  for (i=0; i<product; i++)
    {
      int inc_product, rand;
      //30% probability of incompatibilities among products
      rand = generate_number(3);
      if (rand==1 && product>1)
    {
      //if products are 2, directly handled
      if (product==2)
        {
          i=0;
          inc_product=1;
          incomp[i][inc_product]=TRUE;
          incomp[inc_product][i]=TRUE;
          if (verbose>=2) printf("\tProduct goods%d and goods%d incompatible!\n",i+1, inc_product+1 );
        }
      else
        {
          inc_product=generate_number_different(product,i);
          incomp[i][inc_product]=TRUE;
          incomp[inc_product][i]=TRUE;
          if (verbose>=2) printf("\tProduct goods%d and goods%d incompatible!\n",i+1, inc_product +1);
        }
    }
    }
}

//modify the data structure positioning truck in depot and setting fuel level
void make_truck_pos()
{
  for(i=1; i<=truck; i++)
    {
      int quantity;
      quantity=generate_number(depot);
      truck_pos[i-1]=quantity;
      if (verbose>=2) printf("\tThe truck %d is in depot%d ", i, quantity);
      if (use_fuel)
    {
      quantity=generate_number(fuel_level);
      truck_fuel[i-1]=quantity;
      if (verbose>=2) printf("with %d unit of fuel\n", quantity);
    }
    }
}


//generate the goal for the problem (and modify the money!)
void make_goal()
{
  for(i=0; i<product; i++)
    {
      //this should never happen!
      if (total_quantity[i] == 0)
    {
      printf("ERROR: GOAL GENERATION FAILED FOR PRODUCT %d!\n", i);
      print_ds();
      exit(1);
    }

      //random product with a random product level in a random depot
      int p_level;
      int tmp_tot_quantity = (total_quantity[i] < product_level)?total_quantity[i]:product_level;

      goal[i] = generate_number(tmp_tot_quantity);

      if (verbose>=2) printf("\tWe must take %d unit of goods%d\n",goal[i], i+1);
      has_goal[i]= TRUE;

    }

}
void print_ds()
{
  printf("\n\tmap (row: market - column: market)\n");
  for (i=0;i<market;i++)
    {   printf("\t " );
    for (j = 0;j<market;j++)
      printf ("%d ",map[i][j] );
    printf("\n" );
    }
  printf("\tmap_depot, map depot_lenght (row: depot - 1st column: market connected, 2nd column length of connection)\n\t ");
  for (i=0;i<depot;i++)
    {   printf ("%d ",map_depot[i]);

    }
  printf("\n\t " );
  for (i=0;i<depot;i++)
    printf ("%d ",map_depot_length[i]);

  printf("\n\tProduct distribution (row: market - column: product) (negative values means cheap product)\n");
  for (i=0;i<market;i++)
    {
      printf("\t " );
      for (j = 0;j<product;j++)
    printf ("%d ",prod_dist[i][j] );
      printf("\n" );
    }
  printf("\tIncompatibility (row: product - column: product )\n");
  for (i=0;i<product;i++)
    {   printf("\t " );
    for (j = 0;j<product;j++)
      printf ("%d ",incomp[i][j]);
    printf("\n" );
    }

  if (use_fuel)
    {
      printf("\tTruck position and fuel (row: truck - 1st column: truck position, 2nd column initial fuel)\n");
      for (i=0;i<truck;i++)
    {
      printf("\t %d %d\n",truck_pos[i],truck_fuel[i]);
    }
    }
  else
    {
      printf("\tTruck position (row: truck - 1st column: truck position)\n");
      for (i=0;i<truck;i++)
    {
      printf("\t %d \n",truck_pos[i]);
    }
    }

  printf("\tTotal product quantity (row: depot - 1st column: total quantity for sale in all market, 2nd column type of goal generation (1 normal, 0 tricked)))\n");
  for (i=0;i<depot;i++)
    {
      printf ("\t%d ",total_quantity[i]);
      printf ("%d \n",has_goal[i]);
    }
  printf("\tGoal (row: truck - column: depot)\n");
  for (i=0;i<product;i++)
    {
      printf("\t " );
      printf ("%d ",goal[i]);
      printf("\n" );
    }
  if (incompatibility==TRUE) printf("The problem is without incompatibilities\n");
  if (incompatibility==FALSE) printf("The problem is with incompatibilities\n");
  if (durative==TRUE) printf("The problem is durative\n");
}


char *pref_symb(int p, int num)
{
  char *res;
  int r, q, l;

  res = (char *)calloc(10, sizeof(char));
  sprintf(res, "p%d", p);

  l = strlen(res);

  q = num-1;
  do
    {
      r = q % PREF_BASE;
      q = q / PREF_BASE;
      res[l++] = (char)(r + PREF_START);
    }
  while (q > 0);

  res[l] = '\0';

  return res;
}


void print_simple_preferences(FILE *f)
{
  int A, B;
  int i, j, p, num;
  char *symb;
  int goods[product];

  if (!constraints_pref)
    {
      for (i = 0, p = 0; i < product_level; i++, p++)
    {
      used_pref[p]++;
      symb = pref_symb(p,used_pref[p]);
      fprintf(f, "\n\t(forall (?g - goods) (preference %s (stored ?g level%d)))", symb, i+1);
    }
    }
  else
    {

      for (i = 0, p = 2 + product_level - 1; i < (product_level - 1); i++, p--)
    //for (i = 0, p = 3 + product_level; i < product_level; i++, p--) VECCHIO CODICE
    {
      used_pref[p]++;
      symb = pref_symb(p,used_pref[p]);
      fprintf(f, "\n\t(forall (?g - goods)\n\t   (preference %s (exists (?l - level)\n\t\t\t     (and (not (= ?l level0)) (not (= ?l level%d))\n\t\t\t\t  (stored ?g ?l)))))", symb, i+1);
    }


    }


  if ((product > 1) && !constraints_pref)
    {
      p = product_level;

      //num = generate_number(product);

      num = ceil_div(product, 3);

      for (i = 0; i < num; i++)
    {
      A = generate_number(product - 1);
      B = generate_number(product - A) + A;

      if (generate_boolean())
        {
          used_pref[p]++;
          symb = pref_symb(p,used_pref[p]);
          fprintf(f, "\n\n\t(forall (?l - level)\n\t   (preference %s (imply (stored goods%d ?l) (stored goods%d ?l))))", symb, A, B);
        }
      else
        {
          fprintf(f, "\n");
          for (j = 0; j < (product_level / 2); j++)
        {
          used_pref[p]++;
          symb = pref_symb(p,used_pref[p]);
          fprintf(f, "\n\t(preference %s (imply (stored goods%d level%d) (stored goods%d level%d)))", symb, A,j+1,B,(2*(j+1)));
        }

          for (;j < product_level; j++)
        {
          used_pref[p]++;
          symb = pref_symb(p,used_pref[p]);
          fprintf(f, "\n\t(preference %s (not (stored goods%d level%d)))", symb,  A, j+1);
        }
        }
    }
    }

  if (!constraints_pref)
    {
      p++;
      used_pref[p]++;
      symb = pref_symb(p,used_pref[p]);
      fprintf(f, "\n\n\t(forall (?g - goods)\n\t   (preference %s\n\t      (and (forall (?m - market) (ready-to-load ?g ?m level0))\n\t\t   (forall (?t - truck) (loaded ?g ?t level0)))))", symb);
      //fprintf(f, "\n\n\t(forall (?t - truck ?g - goods)\n\t   (preference p%d (loaded ?g ?t level0)))", p);
    }
}



void print_constraints_preferences(FILE *f)
{

  int A, B;
  int i, j, p, num;
  long int choices[product];
  long int adr, num_c;
  int skip;
  char *symb;

  fprintf(f, "\n(:constraints (and");

  p = product_level + 3;
  used_pref[p]++;
  symb = pref_symb(p,used_pref[p]);
  fprintf(f, "\n\t(forall (?g - goods)\n\t   (preference %s\n\t      (at end (and (forall (?m - market) (ready-to-load ?g ?m level0))\n\t\t\t   (forall (?t - truck) (loaded ?g ?t level0))))))", symb);

  p = 0;
  used_pref[p]++;
  symb = pref_symb(p,used_pref[p]);
  fprintf(f, "\n\n\t(forall (?m - market ?t - truck) (preference %s (at-most-once (at ?t ?m))))", symb);

  p = 0;
  used_pref[p]++;
  symb = pref_symb(p,used_pref[p]);
  fprintf(f, "\n\n\t(forall (?t - truck ?g - goods)\n\t   (preference %s (at-most-once (exists (?l - level)\n\t\t\t      (and (loaded ?g ?t ?l) (not (= ?l level0)))))))", symb);

  p = 1;
  used_pref[p]++;
  symb = pref_symb(p,used_pref[p]);
  fprintf(f, "\n\n\t(forall (?m - market ?t1 ?t2 - truck)\n\t   (preference %s (always (imply (and (at ?t1 ?m) (at ?t2 ?m))\n\t\t\t\t\t  (= ?t1 ?t2)))))", symb);

  p = 2;
  used_pref[p]++;
  symb = pref_symb(p,used_pref[p]);
  fprintf(f, "\n\n\t(forall (?t - truck)\n\t   (preference %s (sometime (exists (?g - goods ?l - level)\n\t\t\t      (and (loaded ?g ?t ?l) (not (= ?l level0)))))))", symb);

  for (i = 0; i < product; i++)
    choices[i] = 0;
  num_c = 0;

  p = product_level + 2;
  if (product > 1)
    {
      fprintf(f, "\n");

      num = generate_number(ceil_div(product, 3));
      for (i = 0; i < num; i++)
    {

      do
        {
          skip = 0;

          A = generate_number(product - 1);
          B = generate_number(product - A) + A;

          adr = A*A+B;
          for (j = 0; j < num_c; j++)
        {
          if (choices[j] == adr)
            {
              skip = 1;
              break;
            }
        }
        }
      while (skip);

      choices[num_c++] = adr;

      if (TRUE || generate_boolean())
        {
          used_pref[p]++;
          symb = pref_symb(p,used_pref[p]);
          fprintf(f, "\n\t(forall (?l - level) (preference %s\n\t   (sometime-before (and (not (= ?l level0)) (stored goods%d ?l))\n\t\t\t    (stored goods%d ?l))))", symb, A, B);
        }
      else
        {
          for (j = 0; j < (product_level / 2); j++)
        {
          used_pref[p]++;
          symb = pref_symb(p,used_pref[p]);
          fprintf(f, "\n\t(preference %s (sometime-before (stored goods%d level%d) (stored goods%d level%d)))",symb, A,j+1,B,(2*(j+1)));
        }

          for (;j < product_level; j++)
        {
          used_pref[p]++;
          symb = pref_symb(p,used_pref[p]);
          fprintf(f, "\n\t(preference %s (always (not (stored goods%d level%d))))", symb, A, j+1);
        }
        }
    }

    }


  /*
  p = product_level + 1;
  used_pref[p]++;
  fprintf(f, "\n\n\t(forall (?m - market)\n\t   (preference p%d (at-most-once (exists (?g - goods ?l - level)\n\t\t   (and (ready-to-load ?g ?m ?l) (not (= ?l level0)))))))", p);
  */


  /*
  fprintf(f, "\n\n\t(forall (?m - market ?g - goods)\n\t   (preference p%d (sometime-after (exists (?l - level)\n\t\t    (and (ready-to-load ?g ?m ?l) (not (= ?l level0)))) (on-sale ?g ?m level0))))", p);

  */
    fprintf(f, "))");



}




void print_preferences_metric(FILE *f)
{
  int i, j;
  Bool first;
  int cost;

  first = TRUE;

  if (constraints_pref)
    cost = 0;
  else
    cost = 1;

  fprintf(f, "\n\n(:metric minimize (+ ");
  for (i = 0; i < MAX_PREFERENCES; i++)
    {
      if (used_pref[i] <= 0)
    continue;

      if (constraints_pref)
    {
      if ((i < 3) || (i >= (2 + product_level)))
        cost++;
      else
        {
          /* STESSI COSTI DELLA VERSIONE PRECEDENTE */
          if (i == 3)
        cost += 5;
          else
        cost += 2;
        }
    }

      for (j = 0; j < used_pref[i]; j++)
    {
      if (!first)
        fprintf(f, "\n\t\t     ");
      fprintf(f, "(* %d (is-violated %s))", cost, pref_symb(i, j + 1));
      first = FALSE;
    }

      if (!constraints_pref)
    cost *= 2;
    }

  fprintf(f, "\n\t\t     ");
  fprintf(f, "(* 1 (is-violated p-drive))");

  fprintf(f,"))");
}



//generate pddl file from the data structure
void generate_pddl(char *nomefile)
{
  //opening file
  FILE *file;
  file=fopen(nomefile, "w");
  if(file==NULL)
    {
      //      printf("File open failure!\n");
      printf("TPP-Propositional Problem file generator");
      usage();
      exit(1);
    }
  // generate comment at the beginning of the file
  /*
    fprintf(file, ";Problem generated by version 1.0 of automatic problem generator\n");
    fprintf(file, ";code by Andrea Occhi and Francesca Periti\n");
    fprintf(file, ";Problem file generated with:\n");
    fprintf(file, ";Number of depots= %d\n", depot);
    fprintf(file, ";Number of markets= %d\n", market);
    fprintf(file, ";Number of product= %d\n", product);
    fprintf(file, ";Number of truck= %d\n", truck);
    fprintf(file, ";Money amount= %d\n", money_level);

    if (use_fuel)
    fprintf(file, ";Tank capacity= %d\n", fuel_level);

    fprintf(file, ";Max product-level= %d\n", product_level);
    fprintf(file, ";Truck capacity= %d\n", truck_capacity);
    if (durative == TRUE)
    fprintf(file, ";The problem is durative!\n");
    if (incompatibility == TRUE)
    fprintf(file, ";The problem is without incompatibility!\n");
    else
    fprintf(file, ";The problem is with incompatibility!\n");
  */

  //Writing pddl file

  fprintf(file, "; Domain designed by Alfonso Gerevini and Alessandro Saetti\n; This file has been automatically generated by the generator available from\n; http://zeus.ing.unibs.it/ipc-5/generators/index.html\n\n");

  fprintf(file, "(define (problem %s)\n", problem_name);
  //domain
  fprintf(file, "(:domain %s)\n", domain_name);
  //objects
  fprintf(file, "(:objects\n\t");
  for(i=1; i<=product; i++)
    fprintf(file, "goods%d ", i );
  fprintf(file, "- goods\n\t", i );

  for(i=1; i<=truck; i++)
    fprintf(file, "truck%d ", i );
  fprintf(file, "- truck\n\t", i );

  for(i=1; i<=market; i++)
    fprintf(file, "market%d ", i );
  fprintf(file, "- market\n\t", i );

  for(i=1; i<=depot; i++)
    fprintf(file, "depot%d ", i );
  fprintf(file, "- depot", i );

  if (use_money)
    {
      fprintf(file, "\n\t");
      for(i=0; i<=money_level; i++)
    fprintf(file, "m%d ", i );
      fprintf(file, "- money-level", i );
    }

  if (use_fuel)
    {
      fprintf(file, "\n\t");
      for(i=0; i<=fuel_level; i++)
    fprintf(file, "f%d ", i );
      fprintf(file, "- fuel-level", i );
    }

  if (use_fuel || use_money)
    {
      fprintf(file, "\n\t");
      for(i=0; i<= product_level; i++)
    {
      if ((i > 0) || !(constraints_pref))
        fprintf(file, "level%d ", i );
      fprintf(file, "- purchased-level", i );
    }
    }
  else
    {
      fprintf(file, "\n\t");
      for(i=0; i <= product_level; i++)
    {
      if ((i > 0) || (!constraints_pref && !simple_pref))
        fprintf(file, "level%d ", i );
    }
      fprintf(file, "- level", i );
    }

  if (use_fuel || use_money)
    {
      fprintf(file, "\n\t");
      for(i=0; i<=truck_capacity; i++)
    fprintf(file, "t%d ", i );
      fprintf(file, "- truck-level", i );
    }

  fprintf(file, ")\n\n");

  //init
  fprintf(file, "(:init");


  if (use_money)
    {
      //initializing money levels
      for(i=1; i <= money_level; i++)
    fprintf(file, "\n\t(next m%d m%d)", i, i-1);
    }

  if (use_fuel)
    //initializing fuel levels
    for(i=1; i <= fuel_level; i++)
      fprintf(file, "\n\t(next f%d f%d)", i, i-1);

  //initializing product levels
  for(i=1; i <= product_level; i++)
    {
      fprintf(file, "\n\t(next level%d level%d)", i, i-1);
    }

  if (use_capacity && (use_money || use_fuel))
    {
      //initializing truck-capacity levels
      for(i=1; i<=truck_capacity; i++)
    fprintf(file, "\n\t(next t%d t%d)", i, i-1);
    }

  //product level in every market is 0
  for(i=1; i<=product; i++)
    {
      for(j=1; j<=market; j++)
    fprintf(file, "\n\t(ready-to-load goods%d market%d level0)",i,j);

      if (simple_pref || constraints_pref)
    {
      for (j = 1; j <= depot; j++)
        fprintf(file, "\n\t(ready-to-load goods%d depot%d level0)",i,j);
    }
    }

  //product level is 0
  for(i=1; i<=product; i++)
    fprintf(file, "\n\t(stored goods%d level0)",i);

  if (use_capacity)
    {
      //product level in every truck is 0
      for(i=1; i <= product; i++)
    for(j=1; j <= truck; j++)
      fprintf(file, "\n\t(loaded goods%d truck%d level0)",i,j);
    }


  if (use_money)
    {
      //money level ready-to-load at start
      fprintf(file, "\n\t(money m%d)",money_level);
    }

  //writing market connection
  for(i=0; i<market; i++)
    {   for(j=0; j<market; j++)
      {
    if (map[i][j]==SHORT)
      {
        if (use_far_connections)
          fprintf(file, "\n\t(near-connected market%d market%d)",i+1,j+1);
        else
          fprintf(file, "\n\t(connected market%d market%d)",i+1,j+1);
      }
    else if (map[i][j]==LONG)
      {
        if (use_far_connections)
          fprintf(file, "\n\t(far-connected market%d market%d)",i+1,j+1);
        else
          fprintf(file, "\n\t(connected market%d market%d)",i+1,j+1);
      }
      }
    }

  //writing depot to market connection
  for(i=0; i<depot; i++)
    {
      if (simple_pref || constraints_pref)
    {
      for(j = 0; j < market; j++)
        {
          fprintf(file, "\n\t(connected depot%d market%d)",i+1,j+1);
          fprintf(file, "\n\t(connected market%d depot%d)",j+1,i+1);
        }
    }
      else
    {
      if (!all_connections)
        {
          if (use_far_connections)
        {
          if (map_depot_length[i]==SHORT)
            {
              fprintf(file, "\n\t(near-connected depot%d market%d)",i+1,map_depot[i]);
              fprintf(file, "\n\t(near-connected market%d depot%d)",map_depot[i],i+1);
            }
          else if (map_depot_length[i]==LONG)
            {
              fprintf(file, "\n\t(far-connected depot%d market%d)",i+1,map_depot[i]);
              fprintf(file, "\n\t(far-connected market%d depot%d)",map_depot[i],i+1);
            }
        }
          else
        {
          fprintf(file, "\n\t(connected depot%d market%d)",i+1,map_depot[i]);
          fprintf(file, "\n\t(connected market%d depot%d)",map_depot[i],i+1);
        }
        }
      else
        {
          for (j = 0; j < market; j++)
        {
          if (use_far_connections)
            {
              if (map_depot_length[i]==SHORT)
            {
              fprintf(file, "\n\t(near-connected depot%d market%d)",i+1,j+1);
              fprintf(file, "\n\t(near-connected market%d depot%d)",j+1,i+1);
            }
              else if (map_depot_length[i]==LONG)
            {
              fprintf(file, "\n\t(far-connected depot%d market%d)",i+1,j+1);
              fprintf(file, "\n\t(far-connected market%d depot%d)",j+1,i+1);
            }
            }
          else
            {
              fprintf(file, "\n\t(connected depot%d market%d)",i+1,j+1);
              fprintf(file, "\n\t(connected market%d depot%d)",j+1,i+1);
            }
        }
        }
    }
    }
  //writing product distribution into market
  for(i=0; i<market; i++)
    {   for(j=0; j<product; j++)
      {
    if (prod_dist[i][j]==0)
      fprintf(file, "\n\t(on-sale goods%d market%d level0)",j+1,i+1);
    else if (prod_dist[i][j]>0)
      fprintf(file, "\n\t(on-sale goods%d market%d level%d)",j+1,i+1,prod_dist[i][j]);
    else
      {
        fprintf(file, "\n\t(on-sale goods%d market%d level%d)",j+1,i+1,-prod_dist[i][j]);
        fprintf(file, "\n\t(cheap goods%d market%d)",j+1,i+1);
      }
      }
    }
  //writing incompatibilities among products
  if (incompatibility == FALSE)
    {
      for(i=0; i<product; i++)
    {   for(j=0; j<product; j++)
      {
        if (incomp[i][j]==FALSE)
          {
        fprintf(file, "\n\t(compatible goods%d goods%d)",i+1,j+1);
        fprintf(file, "\n\t(compatible goods%d goods%d)",j+1,i+1);
          }
      }
    }
    }
  //writing truck position and fuel level
  for(i=0; i<truck; i++)
    {
      fprintf(file,"\n\t(at truck%d depot%d)",i+1,truck_pos[i]);

      if (use_fuel)
    fprintf(file,"\n\t(fuel truck%d f%d)",i+1,truck_fuel[i]);

    }


  fprintf(file,")\n\n");

  //writing goal
  fprintf(file,"(:goal (and");

  if (!simple_pref)
    {
      for(i=0; i<product; i++)
    {
      fprintf(file, "\n\t(stored goods%d level%d)",i+1, goal[i]);
    }
    }
  else
    {
      print_simple_preferences(file);
    }

  //end and for goals
  fprintf(file,"))\n");
  //end goal

  if (constraints_pref)
     print_constraints_preferences(file);

  //metric*/
  if (simple_pref)
    print_preferences_metric(file);
  else if (durative == TRUE)
    fprintf(file,"(:metric minimize (total-time))");

  fprintf(file, "\n)\n");
  fclose(file);
}


//main method
int main(int argc, char *argv[])
{

  static struct option long_options[] = {};

  int option_index = 0;
  char sw;
   //counters
  int i, j;
  //used for generating boolean values
  int boolean=2;
  char *nomefile;

  //durative problem
  durative = FALSE;
  //incompatibilities option
  incompatibility=FALSE;
  //default values
  verbose=1;
  product=2;
  truck=2;
  market=4;
  depot=2;
  money_level=100;
  fuel_level=10;
  domain_name = NULL;
  problem_name = "TPP";

  product_level=10;
  truck_capacity=10;
  incompatibility=TRUE;

  i=0;
  srand(time(NULL));

  extern char *optarg;

  while ( (sw = getopt_long(argc, argv, "hs:p:m:t:d:l:", long_options, &option_index)) != -1)
    {
      switch (sw)
    {
    case 0:
      if (long_options[option_index].flag != 0)
        break;
      switch (option_index)
        {
        case 5:
          problem_name = (char *)calloc(strlen(optarg)+1, sizeof(char));
          sprintf(problem_name, "%s", optarg);
          break;
        default:
          break;
        }
      break;
      /*
    case 'c':
      all_connections = TRUE;
      break;
      */
    case 'h': /* help option */
      usage();
      return 1;
      break;
      /*
    case 'v': // be more verbose
      verbose++;
      break;
    case 'q': // be quiet
      verbose=0;
      break;
      */
    case 'p': /* product number */
      product=atoi(optarg);
      break;
    case 't': /* truck number */
      truck=atoi(optarg);
      break;
    case 'm': /* market number */
      market=atoi(optarg);
      break;
    case 's':
      seed=atoi(optarg);
      srand(seed);
      break;
    case 'd': /* depot number */
      depot=atoi(optarg);
      break;
      /*
    case 'f':
      use_fuel = 1;
      break;
    case 'M':
      use_money = 1;
      break;
    case 'F': // level of fuel in a truck
      fuel_level=atoi(optarg);
      break;
      */
    case 'l': /* level od product in a market/depot*/
      product_level=atoi(optarg);
      break;
      /*
    case 'T': // truck capacity of carrying products
      truck_capacity=atoi(optarg);
      break;
    case 'D': // durative problem
      durative=TRUE;
      break;
        case 'N':
      domain_name = optarg;
      break;
    case 'I': // incompatibility option
      incompatibility=TRUE;
      break;
      */
    case '?': /* unrecognized options */
      usage();
      exit(1);
      break;
    default: /* should not be reached */
      printf ("General error\n");
    }
    }
  //input variables must be different from 0
  if(product==0 || truck==0 || market==0 || depot ==0 || (use_fuel && (fuel_level==0)) || product_level==0 || truck_capacity==0)
    {
      printf("\tWARNING! Values must be > 0\n");
      return(1);
    }

  if (domain_name == NULL)
    {
      if (constraints_pref)
    domain_name = "TPP-PropositionalPreferences";
      else if (simple_pref)
    domain_name = "TPP-PropositionalSimplePreferences";
      else
    domain_name = "TPP-Propositional";
    }

  if (constraints_pref)
    simple_pref = 1;

  //get file name
  nomefile=argv[optind];

  //initializing data structure...
  initialize_data();
  if (verbose>=3) printf("Making map not fully connected...\n");
  make_map(market,depot);
  if (verbose>=3) printf("Generating problem...\n");
  make_product_dist();
  make_incomp();
  make_truck_pos();
  make_goal();
  if (verbose>=3) printf("Evaluating money requirement...\n");

  if (use_money)
    make_money();

  if (verbose>=3)
    {
      printf("Data structure:");
      print_ds();
    }
  generate_pddl(nomefile);
  if (verbose >= 2)
    {
      printf("Problem file generated with:\n");
      printf("\tNumber of depots= %d\n", depot);
      printf("\tNumber of markets= %d\n", market);
      printf("\tNumber of product= %d\n", product);
      printf("\tNumber of truck= %d\n", truck);
      printf("\tMoney amount= %d\n", money_level);

      if (use_fuel)
    printf("\tTank capacity= %d\n", fuel_level);

      printf("\tMax product-level= %d\n", product_level);
      printf("\tTruck capacity= %d\n", truck_capacity);
      if (durative == TRUE)
    printf("\tThe problem is durative!\n");
      if (incompatibility == TRUE)
    printf("\tThe problem is without incompatibilities!\n");
      if (incompatibility == FALSE)
    printf("\tThe problem is with incompatibilities!\n");
    }
  exit(0);
}
