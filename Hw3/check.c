#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *in;
	char name[100];
	int now,next,res;
	scanf("%s",name);
	if((in=fopen(name,"r"))==NULL)
	{
		printf("Read file Error!\n");
		exit(0);
	}
	fscanf(in,"%d",&now);
	while(res=fscanf(in,"%d",&next))
	{
		if(res==EOF)
			break;
		if(next<now)
			printf("Error at %d %d\n",now,next);
		now=next;
	}
	fclose(in);
	return 0;
}