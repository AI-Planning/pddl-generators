#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define MAX_ELEVATORS 10
#define MAX_AREAS 10
#define MAX_PASSENGERS 300

#define SLOW_COST 1
#define FAST_COST 3

#define STOP_SLOW_COST 5
#define STOP_FAST_COST 1
#define SLOW_SPEED 8			// This it the time needed to travel one floor distance (i.e. the inverse of speed)
#define FAST_ACCELERATION 0.2	// This is the acceleration, i.e. how much changes the speed per second, provided that the distance between two adjacent floors equals 1.
#define DOORS_DELAY 4			

int Numbers;

int temporal=0;
int numeric=0;
int net_benefit=0;

int FLOORS;
int AREA_SIZE;
double STEP_SIZE;
int FAST_ELEVATORS;
int SLOW_ELEVATORS;
int FAST_CAPACITY;
int SLOW_CAPACITY;
int passengers;
int areas;

int fast_elevators[MAX_ELEVATORS];
int slow_elevators[MAX_AREAS][MAX_ELEVATORS];
int origin[MAX_PASSENGERS];
int destination[MAX_PASSENGERS];

char *itoa(int i, char* s, int dummy_radix) {
    sprintf(s, "%d", i);
    return s;
}

int max(int a, int b)
{
	if (a>b) 
		return a;
	else
		return b;
}

FILE *infile, *outfile;

int read_input(char* filename)
{
	int i,j;
	infile =fopen(filename,"r"); 
	fscanf(infile,"%d %d %d %d %d %d %d\n", &FLOORS, &AREA_SIZE, &FAST_ELEVATORS, &SLOW_ELEVATORS, &FAST_CAPACITY, &SLOW_CAPACITY, &passengers);
	
	STEP_SIZE=(int) (AREA_SIZE+0.0001)/2;
	
	for(i=0;i<FAST_ELEVATORS; i++)
		fscanf(infile, "%d", &fast_elevators[i]);
	
	for(i=0;i<FLOORS/AREA_SIZE;i++)
		for(j=0;j<SLOW_ELEVATORS;j++)
			fscanf(infile, "%d", &(slow_elevators[i][j]));
	
	for(i=0;i<passengers;i++)
		fscanf(infile, "%d %d",&origin[i], &destination[i]);

	fclose(infile);

	Numbers=max(FLOORS, max(FAST_CAPACITY, SLOW_CAPACITY));
	areas=FLOORS/AREA_SIZE;

	return 1;
}

int ceiling(double x)
{
	x=x-0.000001;
	return 1+(int)x;
}

void generate_problem(char *filename, int instance)
{
	int i, j, k;
	outfile=fopen(filename,"w");
	fprintf(outfile,"(define (problem elevators");
	if (net_benefit==1)
		fprintf(outfile,"-netbenefit");
	else if (temporal==1)
		fprintf(outfile,"-time");
	else
		fprintf(outfile,"-sequencedstrips");
	fprintf(outfile,"-p%d_%d_%d)\n",FLOORS, passengers, instance);

	fprintf(outfile,"(:domain elevators");
	if (net_benefit==1)
		fprintf(outfile,"-netbenefit");
	else if (temporal==1)
		fprintf(outfile,"-time");
	else
		fprintf(outfile,"-sequencedstrips");
	if (numeric==1)
		fprintf(outfile,"-numeric");
	fprintf(outfile,")\n");

	fprintf(outfile,"\n(:objects \n");

	if (numeric==0) 
		{for(i=0;i<=Numbers;i++) fprintf(outfile,"n%d ",i); fprintf(outfile," - count\n");}
	else
		{for(i=0;i<=FLOORS;i++) fprintf(outfile,"f%d ",i); fprintf(outfile," - floor\n");}
	for(i=0;i<passengers;i++) fprintf(outfile, "p%d ",i); fprintf(outfile," - passenger\n");
	for(i=0;i<FAST_ELEVATORS;i++) fprintf(outfile, "fast%d ",i); fprintf(outfile," - fast-elevator\n");
	for(i=0;i<areas;i++)
		for(j=0;j<SLOW_ELEVATORS;j++)
			fprintf(outfile,"slow%d-%d ",i,j);
	fprintf(outfile, "- slow-elevator\n");

	fprintf(outfile,")\n");

	fprintf(outfile,"\n(:init\n");
	
	if (numeric==0)
	{
		for(i=0;i<Numbers;i++) fprintf(outfile,"(next n%d n%d) ", i, i+1); 
		fprintf(outfile,"\n\n");
	}	

	for(i=0;i<FLOORS;i++)
	{
		for(j=i+1;j<=FLOORS;j++)
			if (numeric==0)
				fprintf(outfile,"(above n%d n%d) ", i, j); 
			else
				fprintf(outfile,"(above f%d f%d) ", i, j); 
		fprintf(outfile,"\n"); 
	}
	fprintf(outfile,"\n"); 
	

	for(i=0;i<FAST_ELEVATORS;i++)
	{
		if (numeric==0)
			fprintf(outfile, "(lift-at fast%d n%d)\n", i,fast_elevators[i]);
		else
			fprintf(outfile, "(lift-at fast%d f%d)\n", i,fast_elevators[i]);
		if (numeric==0)
			fprintf(outfile, "(passengers fast%d n0)\n",i);
		else
			fprintf(outfile, "(= (passengers fast%d) 0)\n",i);
		
		if (numeric==1)
			fprintf(outfile, "(= (capacity fast%d) %d)\n",i,FAST_CAPACITY);
		else
		{
			for (j=1;j<=FAST_CAPACITY;j++)
				fprintf(outfile, "(can-hold fast%d n%d) ",i,j);
			fprintf(outfile, "\n");
		}
		for (j=0;j<=FLOORS;j+=STEP_SIZE)
			if (numeric==0)
				fprintf(outfile, "(reachable-floor fast%d n%d)",i,j);
			else
				fprintf(outfile, "(reachable-floor fast%d f%d)",i,j);
		fprintf(outfile, "\n\n");
	}


	for(i=0;i<areas;i++)
		for(j=0;j<SLOW_ELEVATORS;j++)
		{
			if (numeric==0)
				fprintf(outfile, "(lift-at slow%d-%d n%d)\n", i,j,slow_elevators[i][j]);
			else
				fprintf(outfile, "(lift-at slow%d-%d f%d)\n", i,j,slow_elevators[i][j]);
			if (numeric==0)
				fprintf(outfile, "(passengers slow%d-%d n0)\n", i,j);
			else
				fprintf(outfile, "(= (passengers slow%d-%d) 0)\n", i,j);
			if (numeric==0)
			{
				for (k=1;k<=SLOW_CAPACITY;k++)
					fprintf(outfile, "(can-hold slow%d-%d n%d) ",i,j,k);
				fprintf(outfile, "\n");
			}
			else
				fprintf(outfile, "(= (capacity slow%d-%d) %d)\n",i,j,SLOW_CAPACITY);

			for (k=i*AREA_SIZE;k<=(i+1)*AREA_SIZE;k++)
				if (numeric==0)
					fprintf(outfile, "(reachable-floor slow%d-%d n%d)",i,j,k);
				else
					fprintf(outfile, "(reachable-floor slow%d-%d f%d)",i,j,k);
			fprintf(outfile, "\n\n");
		}

	for (i=0;i<passengers;i++)
		if (numeric==0)
			fprintf(outfile, "(passenger-at p%d n%d)\n", i,origin[i]);
		else
			fprintf(outfile, "(passenger-at p%d f%d)\n", i,origin[i]);
	fprintf(outfile, "\n");

	for(k=0;k<areas;k++)
	{
		for(i=k*AREA_SIZE;i<(k+1)*AREA_SIZE;i++)
		{
			for(j=i+1;j<=(k+1)*AREA_SIZE;j++)
				if (temporal==0)
					if (numeric==0)
						fprintf(outfile,"(= (travel-slow n%d n%d) %d) ", i, j,STOP_SLOW_COST+SLOW_COST*(j-i)); 
					else
						fprintf(outfile,"(= (travel-slow f%d f%d) %d) ", i, j,STOP_SLOW_COST+SLOW_COST*(j-i)); 
				else
					if (numeric==0)
						fprintf(outfile,"(= (travel-slow n%d n%d) %d) ", i, j,DOORS_DELAY + SLOW_SPEED * (j-i)); 
					else
						fprintf(outfile,"(= (travel-slow f%d f%d) %d) ", i, j,DOORS_DELAY + SLOW_SPEED * (j-i)); 
		}
		fprintf(outfile,"\n\n"); 
	}
	fprintf(outfile,"\n"); 

	for(i=0;i<Numbers;i+=STEP_SIZE)
	{
		for(j=i+STEP_SIZE;j<=Numbers;j+=STEP_SIZE)
			if (temporal==0)
				if (numeric==0)
					fprintf(outfile,"(= (travel-fast n%d n%d) %d) ", i, j,STOP_FAST_COST+FAST_COST*(j-i)); 
				else
					fprintf(outfile,"(= (travel-fast f%d f%d) %d) ", i, j,STOP_FAST_COST+FAST_COST*(j-i)); 
			else
				if (numeric==0)
					fprintf(outfile,"(= (travel-fast n%d n%d) %d) ", i, j,DOORS_DELAY + ceiling(2*sqrt((j-i)/FAST_ACCELERATION))); 
				else
					fprintf(outfile,"(= (travel-fast f%d f%d) %d) ", i, j,DOORS_DELAY + ceiling(2*sqrt((j-i)/FAST_ACCELERATION))); 
		fprintf(outfile,"\n\n"); 
	}

	if (temporal==0)
		fprintf(outfile, "(= (total-cost) 0)\n\n");

	fprintf(outfile,")\n\n");

	fprintf(outfile,"(:goal\n(and\n");
	
	for (i=0;i<passengers;i++)
	{
		if (net_benefit==1)
			fprintf(outfile,"(preference served%d ",i);
		if (numeric==0)
			fprintf(outfile, "(passenger-at p%d n%d)", i,destination[i]);
		else
			fprintf(outfile, "(passenger-at p%d f%d)", i,destination[i]);
		if (net_benefit==1)
			fprintf(outfile,") ");
		fprintf(outfile,"\n");
	}
	fprintf(outfile,"))\n\n");

	if (net_benefit==1)
	{
		fprintf(outfile,"(:metric maximize (- %d (+ (total-cost)\n", passengers*(2*FAST_COST*FLOORS+2*STOP_FAST_COST+4*SLOW_COST*AREA_SIZE+4*STOP_SLOW_COST));
		for(i=0;i<passengers;i++)
		{
			int penalty=(destination[i]-origin[i])*(destination[i]-origin[i]);
			if (destination[i]>origin[i])
				penalty*=2;
			fprintf(outfile,"(* (is-violated served%d) %d)\n",i,penalty);
		}
		fprintf(outfile,")))\n\n");
	}
	else if (temporal==0)
		fprintf(outfile,"(:metric minimize (total-cost))\n\n");
	else
		fprintf(outfile,"(:metric minimize (total-time))\n\n");


	fprintf(outfile,")\n");
	fclose(outfile);
}


int main(int argc, char** argv) {
	srand( (unsigned)time( NULL ) );
	if (argc!=9) {
			printf("error");
			return -1;
			}
	char infilename[100],problemfilename[100], temps[35];
	int i, j, k, floors1, floors2, floorsd, passengers1, passengers2, passengersd, min_instance, max_instance;
	floors1=atoi(argv[1]);
	floors2=atoi(argv[2]);
	floorsd=atoi(argv[3]);
	passengers1=atoi(argv[4]);
	passengers2=atoi(argv[5]);
	passengersd=atoi(argv[6]);
	min_instance=atoi(argv[7]);
	max_instance=atoi(argv[8]);

	for (i=floors1;i<=floors2;i+=floorsd)
		for(j=passengers1;j<=passengers2;j+=passengersd)
			for(k=min_instance;k<=max_instance;k++)
			{
				strcpy(infilename,"p");
				strcpy(problemfilename,"p");
				
				itoa(i,temps,10);
				strcat(infilename,temps);
				strcat(problemfilename,temps);
				
				strcat(infilename,"_");
				strcat(problemfilename,"_");
				
				itoa(j,temps,10);
				strcat(infilename,temps);
				strcat(problemfilename,temps);
				
				strcat(infilename,"_");
				strcat(problemfilename,"_");
				
				itoa(k,temps,10);
				strcat(infilename,temps);
				strcat(problemfilename,temps);
				
				strcat(infilename,".txt");
				strcat(problemfilename,".pddl");
				
				read_input(infilename);
					
				generate_problem(problemfilename, k);
			}
	return 1;
}
