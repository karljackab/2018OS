#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	int n,i,k;
	FILE *out;

	srand(time(NULL));
	out=fopen("input","w");
	scanf("%d",&n);
	fprintf(out,"%d \n",n);
	for(i=0;i<n;i++)
	{
		k=rand()%1000000;
		if(rand()%2)
			k*=-1;
		fprintf(out,"%d ",k);
	}
	return 0;
}