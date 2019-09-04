#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

int opt=1;
int sat=0;
int all=0;

int A[300][300];
int Duration[300];
int Orders, Products, Numbers;
int ADL=0;
int negated=0;
int semiground=1;
int temporal=0;
int numeric=0;
int net_benefit=1;

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
    fscanf(infile, "%d %d", &Orders, &Products);
	if (temporal==1)
		Numbers=(int) (0.9* max(Orders,Products));
	else
		Numbers=max(Orders,Products);
	for(i=0;i<Orders;i++)
		for(j=0;j<Products;j++)
			fscanf(infile,"%d",&A[i][j]);
	for(j=0;j<Products;j++)
		fscanf(infile,"%d",&Duration[j]);
	fclose(infile);
	return 1;
}

void print_problem()
{
	int i,j;
	printf("%d %d\n",Orders,Products);
	for(i=0;i<Orders;i++)
	{
		for(j=0;j<Products;j++)
			printf("%d ",A[i][j]);
		printf("\n");
	}
}

void generate_problem(char *filename, int items, int instance)
{
	int i, j;
	outfile=fopen(filename,"w");
	fprintf(outfile,"(define (problem os");
	if (net_benefit==1)
		fprintf(outfile,"-netbenefit");
	else if (temporal==1)
		fprintf(outfile,"-time");
	else
		fprintf(outfile,"-sequencedstrips");
	fprintf(outfile,"-p%d_%d)\n",items, instance);

	fprintf(outfile,"(:domain openstacks");
	if (net_benefit==1)
		fprintf(outfile,"-netbenefit");
	else if (temporal==1)
		fprintf(outfile,"-time");
	else
		fprintf(outfile,"-sequencedstrips");
	if (numeric==1)
		fprintf(outfile,"-numeric");
	if (ADL==0)
		fprintf(outfile,"-nonADL");
	else
		fprintf(outfile,"-ADL");
	if (negated==1)
		fprintf(outfile,")\n");
	else
		fprintf(outfile,"-nonNegated)\n");

	fprintf(outfile,"(:objects \n");
	if (numeric==0) {for(i=0;i<=Numbers;i++) fprintf(outfile,"n%d ",i); fprintf(outfile," - count\n");}
	if (semiground==0)
	{
		for(i=1;i<=Orders;i++) fprintf(outfile,"o%d ",i); fprintf(outfile," - order\n");
		for(i=1;i<=Products;i++) fprintf(outfile,"p%d ",i); fprintf(outfile," - product\n\n");
	}
	fprintf(outfile,")\n");

	fprintf(outfile,"\n(:init\n");
	if (semiground==0 && ADL==0 && numeric==0)
		fprintf(outfile,"(zero n0)\n");

	if (numeric==0)
	{
		for(i=0;i<Numbers;i++) fprintf(outfile,"(next-count n%d n%d) ", i, i+1);
		fprintf(outfile,"\n");
	}

	if (temporal==0 && net_benefit==0)
		if (numeric==0)
			fprintf(outfile,"(stacks-avail n0)\n");
		else
			fprintf(outfile,"(= (stacks-in-use) 0)\n");
	else
		if (temporal==1)
			if (numeric==0)
				fprintf(outfile,"(stacks-avail n%d)\n",Numbers);
			else
			{
				fprintf(outfile,"(= (stacks-in-use) 0)\n");
				fprintf(outfile,"(= (max-stacks) %d)\n",Numbers);
			}
		else
		{
			if (numeric==1)
			{
				fprintf(outfile,"(= (stacks-in-use) 0)\n");
				fprintf(outfile,"(= (max-in-use) 0)\n");
			}
			else
				fprintf(outfile,"(stacks-avail n0)\n");
		}

	for(i=1;i<=Orders;i++)
	{
		int count_products=0;
		fprintf(outfile,"\n(waiting o%d)\n",i);
		for(j=1;j<=Products;j++)
			if (A[i-1][j-1]==1)
			{
				count_products++;
				fprintf(outfile,"(includes o%d p%d)",i,j);
				if (semiground==0 && ADL==0)
				{
					fprintf(outfile,"(pending-order-for-product o%d p%d)",i,j);
					fprintf(outfile,"(pending-product-for-order p%d o%d)\n",j,i);
				}
			}
		if (semiground==0 && ADL==0)
			fprintf(outfile,"(pending-products-per-order o%d n%d)\n",i,count_products);
		fprintf(outfile,"\n");
	}

	if (semiground==0 && ADL==0)
		for(j=1;j<=Products;j++)
		{
			int count_orders=0;
			for(i=1;i<=Orders;i++)
				if (A[i-1][j-1]==1) count_orders++;
			fprintf(outfile,"(pending-orders-per-product p%d n%d)\n",j,count_orders);
		}

if (negated==0)
{
	fprintf(outfile,"\n");
	for(j=1;j<=Products;j++)
		fprintf(outfile,"(not-made p%d)\n",j);
}

	if (temporal==1 && (semiground==0||ADL==1))
	{
		fprintf(outfile,"\n");
		for(j=1;j<=Products;j++)
					fprintf(outfile,"(= (make-time p%d) %d)",j,Duration[j-1]);
		fprintf(outfile,"\n\n");
	}

	if (temporal==0)
		fprintf(outfile,"\n(= (total-cost) 0)\n\n");

	if (net_benefit==1)
		fprintf(outfile,"(= (stack-cost) %d)\n\n",(int) (0.5*Numbers));

	fprintf(outfile,")\n\n");

	fprintf(outfile,"(:goal\n(and\n");
	for(i=1;i<=Orders;i++)
		fprintf(outfile,"(shipped o%d)\n",i);

	if (net_benefit==1)
		for(i=1;i<=Orders;i++)
			for (j=1;j<=Products;j++)
				if (A[i-1][j-1]==1)
					fprintf(outfile,"(preference d-o%d-p%d (delivered o%d p%d))\n",i,j,i,j);
	fprintf(outfile,"))\n\n");
	if (net_benefit==1)
	{

		fprintf(outfile,"(:metric maximize (- %d (+\n", (int) (Products*Products/2));
		fprintf(outfile,"(total-cost)\n");
		for(i=1;i<=Orders;i++)
			for(j=1;j<=Products;j++)
				if (A[i-1][j-1]==1)
					fprintf(outfile,"(* (is-violated d-o%d-p%d) 1)\n",i,j);
		fprintf(outfile,")))\n)\n");
	}
	else if (temporal==0)
		fprintf(outfile,"(:metric minimize (total-cost))\n\n)\n\n");
	else
		fprintf(outfile,"(:metric minimize (total-time))\n\n)\n\n");

	fclose(outfile);
}

void generate_domain(char *filename)
{
	int i, j;
	outfile=fopen(filename,"w");
	fprintf(outfile,"(define (domain openstacks");
	if (net_benefit==1)
		fprintf(outfile,"-netbenefit");
	else if (temporal==1)
		fprintf(outfile,"-time");
	else
		fprintf(outfile,"-sequencedstrips");
	if (numeric==1)
		fprintf(outfile,"-numeric");
	if (ADL==0)
		fprintf(outfile,"-nonADL");
	else
		fprintf(outfile,"-ADL");
	if (negated==1)
		fprintf(outfile,")\n");
	else
		fprintf(outfile,"-nonNegated)\n");

	fprintf(outfile,"(:requirements :typing");
	if (temporal==0)
		fprintf(outfile," :action-costs");
	else
		fprintf(outfile," :durative-actions");
	if (numeric==1) fprintf(outfile," :fluents");
	fprintf(outfile,")\n");

	if (numeric==0)
		fprintf(outfile,"(:types order product count)\n");
	else
		fprintf(outfile,"(:types order product)\n");

	fprintf(outfile,"(:constants \n");
	for(j=1;j<=Products;j++)
		fprintf(outfile," p%d",j);
	fprintf(outfile," - product\n");

	for(i=1;i<=Orders;i++)
		fprintf(outfile," o%d",i);
	fprintf(outfile," - order\n");

	fprintf(outfile,")\n\n");

	fprintf(outfile,"(:predicates \n");
	fprintf(outfile,"\t(includes ?o - order ?p - product)\n");
	fprintf(outfile,"\t(waiting ?o - order)\n");
	fprintf(outfile,"\t(started ?o - order)\n");
	fprintf(outfile,"\t(shipped ?o - order)\n");
	if (net_benefit==1)
		fprintf(outfile,"\t(delivered ?o - order ?p - product)\n");
	fprintf(outfile,"\t(made ?p - product)\n");
	if (negated==0)
		fprintf(outfile,"\t(not-made ?p - product)\n");
	if (numeric==0)
	{
		fprintf(outfile,"\t(stacks-avail ?s - count)\n");
		fprintf(outfile,"\t(next-count ?s ?ns - count)\n");
	}
	fprintf(outfile,")\n\n");

	fprintf(outfile,"(:functions\n");
	if (temporal==0)
		fprintf(outfile,"(total-cost)\n");
	else
		if (semiground==0 || ADL==1)
			fprintf(outfile,"(make-time ?p - product) - number\n");
	if (numeric==1)
		if (net_benefit==0)
			fprintf(outfile,"(stacks-in-use) (max-stacks)\n");
		else
			fprintf(outfile,"(stacks-in-use) (max-in-use)\n");
	if (net_benefit==1)
		fprintf(outfile,"(stack-cost)\n");
	fprintf(outfile,")\n\n");


	if (temporal==0)
		if (numeric==0)
		{
			fprintf(outfile,"(:action open-new-stack\n");
			fprintf(outfile,":parameters (?open ?new-open - count)\n");
			fprintf(outfile,":precondition (and (stacks-avail ?open)(next-count ?open ?new-open))\n");
			fprintf(outfile,":effect (and (not (stacks-avail ?open))(stacks-avail ?new-open)");
			if (net_benefit==0)
				fprintf(outfile," (increase (total-cost) 1))\n");
			else
				fprintf(outfile," (increase (total-cost) (stack-cost)) )\n");
			fprintf(outfile,")\n\n");
		}
		else
		{
			fprintf(outfile,"(:action open-new-stack\n");
			fprintf(outfile,":parameters ()\n");
			fprintf(outfile,":precondition ()\n");
			if (net_benefit==0)
				fprintf(outfile,":effect (and (increase (total-cost) 1) (increase (stacks-in-use) 1))\n");
			else
				fprintf(outfile,":effect (and (increase (total-cost) (stack-cost)) (increase (max-in-use) 1))\n");
			fprintf(outfile,")\n\n");
		}

	// START-ORDER
	if (temporal==0)
		fprintf(outfile,"(:action start-order\n");
	else
		fprintf(outfile,"(:durative-action start-order\n");

	if (numeric==0)
		fprintf(outfile,":parameters (?o - order ?avail ?new-avail - count)\n");
	else
		fprintf(outfile,":parameters (?o - order)\n");

	if (temporal==1)
		fprintf(outfile,":duration (= ?duration 1)\n");

	if (temporal==0)
		fprintf(outfile,":precondition ");
	else
		fprintf(outfile,":condition ");

	if (temporal==0)
		fprintf(outfile, "(and (waiting ?o)");
	else
		fprintf(outfile, "(and (at start (waiting ?o))");

	if (temporal==0)
		if (numeric==0)
			fprintf(outfile, "(stacks-avail ?avail)(next-count ?new-avail ?avail))\n");
		else
			fprintf(outfile, "(< (stacks-in-use) (max-in-use)))\n");
	else
		if (numeric==0)
			fprintf(outfile, "(at start (stacks-avail ?avail))(at start (next-count ?new-avail ?avail)))\n");
		else
			fprintf(outfile, "(at start (< (stacks-in-use) (max-stacks))))\n");

	if (temporal==0)
		fprintf(outfile,":effect (and (not (waiting ?o))(started ?o)");
	else
		fprintf(outfile,":effect (and (at start (not (waiting ?o)))(at end (started ?o))");

	if (temporal==0)
		if (numeric==0)
			fprintf(outfile,"(not (stacks-avail ?avail))(stacks-avail ?new-avail))\n");
		else
			fprintf(outfile,"(increase (stacks-in-use) 1))\n");
	else
		if (numeric==0)
			fprintf(outfile,"(at start (not (stacks-avail ?avail)))(at end (stacks-avail ?new-avail)))\n");
		else
			fprintf(outfile,"(at start (increase (stacks-in-use) 1)))\n");

	fprintf(outfile,")\n\n");

	// MAKE-PRODUCT
	for(j=1;j<=Products;j++)
	{
		if (temporal==0)
			fprintf(outfile,"(:action make-product-p%d\n",j);
		else
			fprintf(outfile,"(:durative-action make-product-p%d\n",j);
	//	fprintf(outfile,":parameters ()\n");
		fprintf(outfile,":parameters ()\n");
		if (temporal==1)
			fprintf(outfile,":duration (= ?duration %d)\n",Duration[j-1]);
		if (temporal==0)
			fprintf(outfile,":precondition ");
		else
			fprintf(outfile,":condition ");

		if (temporal==0)
			if (negated==1)
				fprintf(outfile,"(and (not (made p%d))",j);
			else
				fprintf(outfile,"(and (not-made p%d)",j);
		else
			if (negated==1)
				fprintf(outfile,"(and (at start (not (made p%d)))",j);
			else
				fprintf(outfile,"(and (at start (not-made p%d))",j);

		for(i=1;i<=Orders;i++)
			if (A[i-1][j-1]==1)
				if (temporal==0)
					fprintf(outfile,"(started o%d)",i);
				else
					fprintf(outfile,"(at start (started o%d))",i);
		fprintf(outfile,")\n");

		if (temporal==0)
			if (negated==1)
				fprintf(outfile,":effect (and (made p%d))\n",j);
			else
				fprintf(outfile,":effect (and (not (not-made p%d)) (made p%d))\n",j,j);
		else
			if (negated==1)
				fprintf(outfile,":effect (and (at end (made p%d)))\n",j);
			else
				fprintf(outfile,":effect (and (at start (not (not-made p%d))) (at end (made p%d)))\n",j,j);
		fprintf(outfile,")\n\n");
	}

	// SHIP-ORDER
	for(i=1;i<=Orders;i++)
	{
		if (temporal==0)
			fprintf(outfile,"(:action ship-order-o%d\n",i);
		else
			fprintf(outfile,"(:durative-action ship-order-o%d\n",i);

		if (numeric==0)
			fprintf(outfile,":parameters (?avail ?new-avail - count)\n");
		else
			fprintf(outfile,":parameters ()\n");

		if (temporal==1)
			fprintf(outfile,":duration (= ?duration 1)\n");

		if (temporal==0)
			fprintf(outfile,":precondition (and (started o%d)",i);
		else
			fprintf(outfile,":condition (and (at start (started o%d))",i);
		for(j=1;j<=Products;j++)
			if (A[i-1][j-1]==1)
				if (temporal==0)
					fprintf(outfile,"(made p%d)",j);
				else
					fprintf(outfile,"(at start (made p%d))",j);

		if (numeric==0)
			if (temporal==0)
				fprintf(outfile,"(stacks-avail ?avail)(next-count ?avail ?new-avail)");
			else
				fprintf(outfile,"(at start (stacks-avail ?avail))(at start (next-count ?avail ?new-avail))");
		fprintf(outfile,")\n");

		if (temporal==0)
			fprintf(outfile,":effect (and (not (started o%d))(shipped o%d)",i,i);
		else
			fprintf(outfile,":effect (and (at start (not (started o%d)))(at end (shipped o%d))",i,i);
		if (temporal==0)
			if (numeric==0)
				fprintf(outfile,"(not (stacks-avail ?avail))(stacks-avail ?new-avail))");
			else
				fprintf(outfile,"(decrease (stacks-in-use) 1))");
		else
			if (numeric==0)
				fprintf(outfile,"(at start (not (stacks-avail ?avail)))(at end (stacks-avail ?new-avail)))");
			else
				fprintf(outfile,"(at end (decrease (stacks-in-use) 1)))");

		fprintf(outfile,")\n\n");
	}

	fprintf(outfile,")\n\n");

	fclose(outfile);
}

/*
int main(int argc, char** argv) {
	srand( (unsigned)time( NULL ) );
	if (argc!=6) return -1;
	char infilename[100],problemfilename[100], domainfilename[100], temps[35];
	int i, j, start, finish, step, min_instance, max_instance;
	start=atoi(argv[1]);
	finish=atoi(argv[2]);
	step=atoi(argv[3]);
	min_instance=atoi(argv[4]);
	max_instance=atoi(argv[5]);

	for (i=start;i<=finish;i+=step)
		for(j=min_instance;j<=max_instance;j++)
		{
			strcpy(infilename,"p");
			strcpy(problemfilename,"p");
			strcpy(domainfilename,"domain");

			itoa(i,temps,10);
			strcat(infilename,temps);
			strcat(problemfilename,temps);
			strcat(domainfilename,temps);

			strcat(infilename,"_");
			strcat(problemfilename,"_");
			strcat(domainfilename,"_");

			itoa(j,temps,10);
			strcat(infilename,temps);
			strcat(problemfilename,temps);
			strcat(domainfilename,temps);

			strcat(infilename,".txt");
			strcat(problemfilename,".pddl");
			strcat(domainfilename,".pddl");

			read_input(infilename);
	//		print_problem();

			generate_problem(problemfilename, i,j);
			if (semiground==1 && net_benefit==0)
				generate_domain(domainfilename);
		}
	return 1;
}
*/


void generate_instances(int start, int finish, int step, int min_instance, int max_instance)
{
	int i, j;
	char infilename[100],problemfilename[100], domainfilename[100], temps[35];
	for (i=start;i<=finish;i+=step)
		for(j=min_instance;j<=max_instance;j++)
		{
			strcpy(infilename,"p");
			strcpy(problemfilename,"p");
			strcpy(domainfilename,"domain");

			sprintf(temps, "%d", i);
			strcat(infilename,temps);
			strcat(problemfilename,temps);
			strcat(domainfilename,temps);

			strcat(infilename,"_");
			strcat(problemfilename,"_");
			strcat(domainfilename,"_");

			sprintf(temps, "%d", j);
			strcat(infilename,temps);
			strcat(problemfilename,temps);
			strcat(domainfilename,temps);

			strcat(infilename,".txt");
			strcat(problemfilename,".pddl");
			strcat(domainfilename,".pddl");

			read_input(infilename);
	//		print_problem();

			generate_problem(problemfilename, i,j);
			if (semiground==1)
				generate_domain(domainfilename);
		}
}

int main(int argc, char** argv) {
	srand( (unsigned)time( NULL ) );
/*	if (argc!=6) return -1;

*/

	if (opt==1)
	{
		generate_instances(5, 34, 1, 1, 1);
	}
	else if (sat==1)
	{
		generate_instances(5, 20, 5, 1, 3);
		generate_instances(30, 60, 10, 1, 3);
		generate_instances(80, 100, 20, 1, 3);
	}
	else if (all==1)
	{
		generate_instances(5, 34, 1, 1, 3);
		generate_instances(35, 100, 5, 1, 3);
	}
	return 0;
}
