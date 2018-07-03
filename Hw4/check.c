#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *in;
	char name[20]="output_x.txt";
	int now,next,res,i;
	for(i=1;i<=8;i++)
	{
		name[7]='0'+i;
		if((in=fopen(name,"r"))==NULL)
		{
			printf("Read file Error!\n");
			exit(0);
		}
		fscanf(in,"%d",&now);
		printf("%s\n",name);
		while(res=fscanf(in,"%d",&next))
		{
			if(res==EOF)
				break;
			if(next<now)
				printf("%d Error at %d %d\n",i,now,next);
			now=next;
		}
		fclose(in);
	}
	return 0;
}