#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct
{
	int head;
	int tail;
	int id;
}parameter;

sem_t sem[16];
sem_t final;
int *seq1,*seq2;
parameter par[16];

void swap(int *a,int *b)
{
	int k=*a;
	*a=*b;
	*b=k;
	return;
}

void *bubble(void *idp)
{
	int i,j;
	int id=*(int*)idp;
	sem_wait(&sem[id]);
	for(i=par[id].head;i<=par[id].tail;i++)
		for(j=i+1;j<=par[id].tail;j++)
			if(*(seq2+j)<*(seq2+i))
				swap(seq2+j,seq2+i);
	sem_post(&sem[1]);
}

void *partition(void *idp)
{
	int id=*(int*)idp;
	sem_wait(&sem[id]);
	int head=par[id].head,tail=par[id].tail,i;
	while(head<tail)
	{
		while(head<tail&& *(seq2+head)<=*(seq2+tail))
			tail--;
		if(head<tail)
			swap(seq2+head,seq2+tail);
		while(head<tail&& *(seq2+head)<=*(seq2+tail))
			head++;
		if(head<tail)
			swap(seq2+head,seq2+tail);
	}
	par[id*2].head=par[id].head;
	par[id*2].tail=tail;
	par[id*2+1].head=tail+1;
	par[id*2+1].tail=par[id].tail;
	sem_post(&sem[id*2]);
	sem_post(&sem[id*2+1]);

	if(id==1)
	{
		for(i=0;i<8;i++)
			sem_wait(&sem[1]);
		FILE *out;
		out=fopen("output1.txt","w");
		int num=par[1].tail;
		for(i=0;i<=num;i++)
			fprintf(out,"%d ",*(seq2+i));
		fclose(out);
		sem_post(&final);
	}
}

void single_bubble(int *seq,int num)
{
	int i,j,k;
	for(i=0;i<num;i++)
		for(j=i+1;j<num;j++)
			if(*(seq+j)<*(seq+i))
				swap(seq+j,seq+i);
	return ;
}

void single_sort(int *seq,int num,int times)
{
	int head=0,tail=num-1;
	if(times<3)
	{
		while(head<tail)
		{
			while(head<tail&& *(seq+head)<=*(seq+tail))
				tail--;
			if(head<tail)
				swap(seq+head,seq+tail);
			while(head<tail&& *(seq+head)<=*(seq+tail))
				head++;
			if(head<tail)
				swap(seq+head,seq+tail);
		}
		single_sort(seq,head,times+1);
		single_sort(seq+head,num-head,times+1);
	}
	else
		single_bubble(seq,num);
	return ;
}

int main()
{
	int num,i,j,k,sec,usec;
	char name[100];
	struct timeval start, end;
	printf("Enter the input file name: ");
	scanf("%s",name);
	FILE *in,*out;
	if((in=fopen(name,"r"))==NULL)
	{
		printf("Read file error\n");
		exit(0);
	}
	fscanf(in,"%d",&num);
	seq1=(int*)malloc(sizeof(int)*num);
	seq2=(int*)malloc(sizeof(int)*num);
	for(i=0;i<num;i++)
	{
		fscanf(in,"%d",seq1+i);
		*(seq2+i)=*(seq1+i);
	}
	
	// ST
	gettimeofday(&start,0);
	single_sort(seq1,num,0);
	gettimeofday(&end,0);
	sec=end.tv_sec-start.tv_sec;
	usec=end.tv_usec-start.tv_usec;
	out=fopen("output2.txt","w");
	for(i=0;i<num;i++)
		fprintf(out,"%d ",*(seq1+i));
	printf("Single thread cost %.2f s\n",sec+(usec/1000000.0));
	fclose(out);

	//MT
	gettimeofday(&start,0);

	pthread_t thread[16];
	par[1].head=0;
	par[1].tail=num-1;
	sem_init(&final,0,0);
	for(i=1;i<16;i++)
	{
		par[i].id=i;
		sem_init(&sem[i],0,0);
		if(i<8)
			pthread_create(&thread[i],NULL,partition,&par[i].id);
		else
			pthread_create(&thread[i],NULL,bubble,&par[i].id);
	}
	sem_post(&sem[1]);
	sem_wait(&final);

	gettimeofday(&end,0);
	sec=end.tv_sec-start.tv_sec;
	usec=end.tv_usec-start.tv_usec;
	printf("Multi thread cost %.2f s\n",sec+(usec/1000000.0));

	return 0;
}