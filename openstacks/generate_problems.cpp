#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define RANGE_MIN 0
#define RANGE_MAX 100

int A[300][300];

int generate_problem(int Products,int Orders,char *filename,int Density);

int main(int argc, char **argv)
{
	srand( (unsigned)time( NULL ) );
	if (argc!=7) {
		printf("Use the following parameters:\n");
		printf("MinItems, MaxItems, Step, MinID, MaxID, Density\n\nwhere:\n\n");
		printf("MinItems : the minimum number of Products=Orders per problem\n");
		printf("MaxItems : the minimum number of Products=Orders per problem\n");
		printf("Step : the increment on number of items\n");
		printf("MinID : the start number of number problems of the same size\n");
		printf("MaxID : the end number of number problems of the same size\n");
		printf("Density : the % probability that each product is included in each order\n");
		return -1;
	}
	int MinItems,MaxItems,Step,MinID,MaxID,Density;
	int counter;
	MinItems=atoi(argv[1]);
	MaxItems=atoi(argv[2]);
	Step=atoi(argv[3]);
	MinID=atoi(argv[4]);
	MaxID=atoi(argv[5]);
	Density=atoi(argv[6]);

	int Orders, Products;
	char filename[100];
	for (Products=MinItems;Products<=MaxItems;Products+=Step)
		for(counter=MinID;counter<=MaxID;counter++)
		{
			char temps[35];
			filename[0]='\0';
			strcat(filename, "p");
			sprintf(temps, "%d", Products);
			strcat(filename,temps);
			strcat(filename,"_");
			sprintf(temps, "%d", counter);
			strcat(filename,temps);
			strcat(filename,".txt");
			Orders=Products;
			generate_problem(Products,Orders,filename,Density);
		}

	return 0;
}

int compute_distance(int order, int product)
{
	int d;
	d=abs(order-product);
	return d;
}

int gaussian(int d, int Products, int Density)
{
	double s, p;
	s=(Products/6.0);
	p=(1/(s*sqrt(2*3.141)))*exp(-(d*d)/(2*s*s));
	return (int) ((double) p*Density*Products/(2.7*sqrt(Products/5.0)));
}

void swap_orders(int o1, int o2, int Orders, int Products)
{
	int Temp[300], i;
	if (o1==o2) return;
	for(i=0;i<Products;i++)
		Temp[i]=A[o1][i];
	for(i=0;i<Products;i++)
		A[o1][i]=A[o2][i];
	for(i=0;i<Products;i++)
		A[o2][i]=Temp[i];
}

int generate_problem(int Products,int Orders,char *filename,int Density){

	FILE *outfile;
	int i,j;
	int rand100;
	int count;

	outfile=fopen(filename,"w");
	fprintf(outfile,"%d %d\n",Orders,Products);

	for(i=0;i<Orders;i++)
		for(j=0;j<Products;j++)
		{
			int p;
			int D=compute_distance(i,j);
			rand100 = (int)(((double) rand() /
                         (double) RAND_MAX) * RANGE_MAX + RANGE_MIN);
//			if (rand100<=Density)
			p=gaussian(D,Products,Density);
			if (rand100<=p)
				A[i][j]=1;
			else
					A[i][j]=0;
		}

	// check for empty orders
	for(i=0;i<Orders;i++)
	{
		count=0;
		for(j=0;j<Products;j++)
			count+=A[i][j];
		if (count==0)
		{
			rand100 = (int)(((double) rand() /
                         (double) RAND_MAX) * (Products-1) );
			A[i][rand100]=1;
		}
	}

	// check for not used products
	for(j=0;j<Products;j++)
	{
		count=0;
		for(i=0;i<Orders;i++)
			count+=A[i][j];
		if (count==0)
		{
			rand100 = (int)(((double) rand() /
                         (double) RAND_MAX) * (Orders-1) );
			A[rand100][j]=1;
		}
	}

	// Reorder orders
	for(i=0;i<Orders;i++)
	{
		int o1, o2;
		o1= (int)(((double) rand() / (double) RAND_MAX) * (Orders-1) );
		o2= (int)(((double) rand() / (double) RAND_MAX) * (Orders-1) );
		swap_orders(o1,o2,Orders,Products);
	}

	// Writing to file...
	for(i=0;i<Orders;i++)
	{
		for(j=0;j<Products;j++)
			if (A[i][j]==1)
				fprintf(outfile,"1 ");
			else
				fprintf(outfile,"0 ");
		fprintf(outfile,"\n");
	}

	for (j=1;j<=Products;j++)
	{
		rand100 = 10*(int)(((double) rand() /
                         (double) RAND_MAX) * 10+1);
		fprintf(outfile,"%d ", rand100);
	}
	fprintf(outfile,"\n");

	fclose(outfile);
	return 1;
}
