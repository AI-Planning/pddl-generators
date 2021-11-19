#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define RANGE_MIN 0
#define RANGE_MAX 100

#define FLOORS 24
#define AREA_SIZE 8
#define FAST_ELEVATORS 3
#define SLOW_ELEVATORS 2
#define FAST_CAPACITY 6
#define SLOW_CAPACITY 4

int generate_problem(int passengers, char *filename);

char *itoa(int i, char* s, int dummy_radix) {
    sprintf(s, "%d", i);
    return s;
}


int main(int argc, char **argv)
{
	srand( (unsigned)time( NULL ) );
	if (argc!=6) {
		printf("Use the following parameters:\n");
		printf("MinPassengers, MaxPassenegrs, Step, MinID, MaxID\n\nwhere:\n\n");
		printf("MinPassengers : the minimum number of passengers per problem\n");
		printf("MaxPassengers : the minimum number of passengers per problem\n");
		printf("Step : the step to go from MinPassengers to MaxPassengers\n");
		printf("MinID : the start number of number problems of the same size\n");
		printf("MaxID : the end number of number problems of the same size\n");
		return -1;
	}
	int MinPassengers,MaxPassengers,Step,MinID,MaxID;
	int counter;
	MinPassengers=atoi(argv[1]);
	MaxPassengers=atoi(argv[2]);
	Step=atoi(argv[3]);
	MinID=atoi(argv[4]);
	MaxID=atoi(argv[5]);

	int passengers;
	char filename[100];
	for (passengers=MinPassengers;passengers<=MaxPassengers;passengers+=Step)
		for(counter=MinID;counter<=MaxID;counter++)
		{
			char temps[35];
			filename[0]='\0';
			strcat(filename, "p");
			itoa(FLOORS,temps,10);
			strcat(filename,temps);
			strcat(filename,"_");
			itoa(passengers,temps,10);
			strcat(filename,temps);
			strcat(filename,"_");
			itoa(counter,temps,10);
			strcat(filename,temps);
			strcat(filename,".txt");
			generate_problem(passengers,filename);
		}
	return 1;
}

int generate_problem(int passengers, char *filename)
{
	FILE *outfile;
	int i,j;
	int r;

	outfile=fopen(filename,"w");
	fprintf(outfile,"%d %d %d %d %d %d %d\n",FLOORS, AREA_SIZE, FAST_ELEVATORS, SLOW_ELEVATORS, FAST_CAPACITY, SLOW_CAPACITY, passengers);

	for(i=0;i<FAST_ELEVATORS; i++)
	{
		r = ((double) rand() / (double) RAND_MAX) * (2*FLOORS/AREA_SIZE+1);
		fprintf(outfile, "%d ", r*AREA_SIZE/2);
	}
	fprintf(outfile, "\n");

	for(i=0;i<FLOORS;i+=AREA_SIZE)
	{
		for(j=0;j<SLOW_ELEVATORS;j++)
		{
			r = ((double) rand() / (double) RAND_MAX) * (AREA_SIZE+1);
			fprintf(outfile, "%d ", i+r);
		}
		fprintf(outfile,"\n");
	}
	
	for(i=0;i<passengers;i++)
	{
		int r1, r2;
		r1 = ((double) rand() / (double) RAND_MAX) * (FLOORS+1);
		r2=r1;
		while (r2==r1)
			r2 = ((double) rand() / (double) RAND_MAX) * (FLOORS+1);
		fprintf(outfile, "%d %d\n",r1,r2);
	}

	fclose(outfile);
	return 1;
}
