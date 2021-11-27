
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
int numberOfInstances;

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
  int inst;
  intlist l;
  FILE *fd,*fi;
  char sd[100],si[100];

  intlist visits[3][1000];

  long seed;

  if (argc < 7 || argc > 8) {
    fprintf(stderr,"maintenance <days> <planes> <mechanics> <cities> <visits> <instances> [<seed>]\n");
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

  if(!sscanf(argv[6],"%d",&numberOfInstances)) {
    printf("# instances not an integer.\n",argv[6]);
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

  for(inst=0;inst<numberOfInstances;inst++) {

  sprintf(si,"maintenance.%i.%i.%.3i.%.3i.%i-%.3i.pddl",
	    numberOfMechanics,numberOfCities,numberOfDays,numberOfVehicles,numberOfVisits,inst);

  fi = fopen(si,"w");

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


  fprintf(fi,"(define (problem %s-%i-%i-%i-%i-%i-%i)\n",PROBLEM,numberOfMechanics,numberOfCities,numberOfDays,numberOfVehicles,numberOfVisits,inst);
  fprintf(fi," (:domain %s)\n",DOMAIN);

  fprintf(fi," (:objects");
  for(i=0;i<=numberOfDays;i++) {
    fprintf(fi," %s%i",DAYNAME,i+1);
  }
  fprintf(fi," - day\n  ");
  for(i=0;i<numberOfCities;i++) {
    fprintf(fi," %s",airport(i));
  }
  fprintf(fi," - airport\n  ");

  for(i=0;i<numberOfVehicles;i++) {
    fprintf(fi," %s%i",PLANE,i+1);
  }
  fprintf(fi," - plane)\n");

  fprintf(fi," (:init\n");
  for(i=0;i<numberOfDays;i++) {
    fprintf(fi,"  (%s %s%i)",AVAILABLE,DAYNAME,i+1);
  }
  for(k=0;k<numberOfVehicles;k++) {
    for(j=0;j<numberOfCities;j++) {
      for(i=0;i<numberOfDays;i++) {
	l = visits[j][i];
	while(l != EMPTYLIST) {
	  if(k==hd(l))
	  fprintf(fi,"  (at %s%i %s%i %s)\n",PLANE,hd(l)+1,DAYNAME,i+1,airport(j));
	  l = tl(l);
	}
      }
    }
  }
  fprintf(fi,")\n");

  fprintf(fi,"  (:goal (forall (?plane - plane) (%s ?plane)))\n",COMPLETED);

  fprintf(fi,"  )\n");

  fclose(fi);
  }

  return 0;
}
