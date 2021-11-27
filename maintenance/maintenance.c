
/* Problem generator for the maintenance scheduling domain. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

/*  Whether all actions can be taken simultanously. If not defined,
the problem will be completely sequential. */

/* names of top level PDDL definitions */

#define DOMAIN "maintenance-scheduling-domain"
#define PROBLEM "maintenance-scheduling"

/* names of state variables */

#define COMPLETED "done"
#define AVAILABLE "today"
#define DAYNAME "d"

#define PLANE "ap"

char *airport(int i) {
  switch(i%3) {
  case 0: return "FRA";
  case 1: return "BER";
  default:
  case 2: return "HAM";
  }
}

int numberOfDays;
int numberOfVehicles;
int numberOfMechanics;
int numberOfCities;
int numberOfVisits;

#define TAG_NEG 0x08000000
#define NEG(l) ((l)^TAG_NEG)
#define PROP(l) ((l)&(0xFFFFFFFF^TAG_NEG))

/* Lists */

typedef struct el {
  int head;
  struct el *tail;
} *intlist;

#define hd(L) ((L)->head)
#define tl(L) ((L)->tail)
#define EMPTYLIST NULL

intlist *visitsone;
intlist *visitstwo;
intlist *visitsthree;

intlist cons(int i,intlist l) {
  intlist m;
  m = (intlist)malloc(sizeof(struct el));
  m->head = i;
  m->tail = l;
  return m;
}

int main(int argc,char **argv) {
  int i,j,k;
  intlist l;
  FILE *fd,*fi;
  char sd[100],si[100];

  intlist visits[3][1000];

  long seed;

  if (argc < 7 || argc > 8) {
    fprintf(stderr,"maintenance <days> <planes> <mechanics> <cities> <visits> [<seed>]\n");
    exit(1);
  }

  if(!sscanf(argv[1],"%d",&numberOfDays)) {
    printf("# days not an integer.\n",argv[1]);
    exit(1);
  }

  if(!sscanf(argv[2],"%d",&numberOfVehicles)) {
    printf("# planes not an integer.\n",argv[2]);
    exit(1);
  }

  if(!sscanf(argv[3],"%d",&numberOfMechanics)) {
    printf("# mechanics not an integer.\n",argv[3]);
    exit(1);
  }

  if(!sscanf(argv[4],"%d",&numberOfCities)) {
    printf("# cities not an integer.\n",argv[4]);
    exit(1);
  }

  if(!sscanf(argv[5],"%d",&numberOfVisits)) {
    printf("# visits not an integer.\n",argv[5]);
    exit(1);
  }

  if (argc == 8 && !sscanf(argv[7], "%ld", &seed)) {
      printf("seed was not an integer.\n", argv[7]);
      exit(1);
  } else if (argc == 7) {
      struct timeval tv;
      struct timezone tz;
      gettimeofday(&tv, &tz);
      seed = tv.tv_usec;
  }

  if(numberOfMechanics >= numberOfCities) {
    fprintf(stderr,"More mechanics than cities: not very interesting!\n");
  }

  srand(seed);

  {

  for(i=0;i<numberOfDays;i++) {
    for(j=0;j<numberOfCities;j++) {
      visits[j][i] = EMPTYLIST;
    }
  }

  for(i=0;i<numberOfVehicles;i++) {
    int j;
    for(j=0;j<numberOfVisits;j++) {
      int day,airport;
      day = rand() % numberOfDays;
      airport = rand() % 3;
      visits[airport][day] = cons(i,visits[airport][day]);
    }
  }


  printf("(define (problem %s-%i-%i-%i-%i-%i)",PROBLEM,numberOfMechanics,numberOfCities,numberOfDays,numberOfVehicles,numberOfVisits);
  printf("\n (:domain %s)",DOMAIN);

  printf("\n (:objects");
  printf("\n  ");
  for(i=0;i<=numberOfDays;i++) {
    printf(" %s%i",DAYNAME,i+1);
  }
  printf(" - day");
  printf("\n  ");
  for(i=0;i<numberOfCities;i++) {
    printf(" %s",airport(i));
  }
  printf(" - airport");
  printf("\n  ");
  for(i=0;i<numberOfVehicles;i++) {
    printf(" %s%i",PLANE,i+1);
  }
  printf(" - plane)");

  printf("\n (:init");
  for(i=0;i<numberOfDays;i++) {
    printf("\n  (%s %s%i)",AVAILABLE,DAYNAME,i+1);
  }
  for(k=0;k<numberOfVehicles;k++) {
    for(j=0;j<numberOfCities;j++) {
      for(i=0;i<numberOfDays;i++) {
	l = visits[j][i];
	while(l != EMPTYLIST) {
	  if(k==hd(l))
	  printf("\n  (at %s%i %s%i %s)",PLANE,hd(l)+1,DAYNAME,i+1,airport(j));
	  l = tl(l);
	}
      }
    }
  }
  printf(")");

  printf("\n  (:goal ",COMPLETED);
  printf("\n    (and ");
  for(i=0;i<numberOfVehicles;i++) {
      printf("\n      (done %s%i)",PLANE,i+1);
  }
  printf(")))\n");


  }

  return 0;
}
