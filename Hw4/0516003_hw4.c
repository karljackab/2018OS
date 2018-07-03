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
	int level;
}job;

int *seq1,*seq2;
job job_list[16];
int jobHead,jobTail,iter=0;
sem_t disp,mutex,final,thd;

void resetSeq(int n)
{
	int i;
	for(i=0;i<n;i++)
		*(seq2+i)=*(seq1+i);
}

void swap(int *a,int *b)
{
	int k=*a;
	*a=*b;
	*b=k;
	return;
}

void bubble(int id)
{
	int i,j;
	for(i=job_list[id].head;i<=job_list[id].tail;i++)
		for(j=i+1;j<=job_list[id].tail;j++)
			if(*(seq2+j)<*(seq2+i))
				swap(seq2+j,seq2+i);
	sem_post(&final);
}

void partition(int id)
{
	int head=job_list[id].head,tail=job_list[id].tail,i;
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
	sem_wait(&mutex);
	jobTail++;
	job_list[jobTail].head=job_list[id].head;
	job_list[jobTail].tail=tail;
	job_list[jobTail].level=job_list[id].level+1;
	jobTail++;
	job_list[jobTail].head=tail+1;
	job_list[jobTail].tail=job_list[id].tail;
	job_list[jobTail].level=job_list[id].level+1;
	sem_post(&disp);
	sem_post(&disp);
	sem_post(&mutex);
}

void *run()
{
	while(1)
	{
		int id;
		sem_wait(&thd);
		sem_wait(&disp);
		sem_wait(&mutex);
		id=jobHead;
		jobHead++;
		sem_post(&mutex);
		if(job_list[id].level<4)
			partition(id);
		else
			bubble(id);
		sem_post(&thd);
		if(iter==8&&jobHead>15)
			break;
	}
}

int main()
{
	int num,i,j,k,sec,usec;
	char name[100];
	struct timeval start,end;
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
		fscanf(in,"%d",seq1+i);


	pthread_t thread[16];
	
	sem_init(&disp,0,0);
	sem_init(&mutex,0,1);
	sem_init(&final,0,0);
	sem_init(&thd,0,0);
	for(i=0;i<8;i++)
		pthread_create(&thread[i],NULL,run,NULL);

	for(iter=1;iter<=8;iter++)
	{
		resetSeq(num);
		jobHead=1;jobTail=1;

		gettimeofday(&start,0);
		job_list[1].id=1;
		job_list[1].head=0;
		job_list[1].tail=num-1;
		job_list[1].level=1;
		sem_post(&thd);
		sem_post(&disp);

		for(i=0;i<8;i++)
			sem_wait(&final);
		gettimeofday(&end,0);

		char name[15]="output_x.txt";
		name[7]='0'+iter;
		name[12]='\0';
		out=fopen(name,"w");
		for(i=0;i<num;i++)
			fprintf(out,"%d ",*(seq2+i));

		sec=end.tv_sec-start.tv_sec;
		usec=end.tv_usec-start.tv_usec;
		printf("%d threads cost %.2f s\n",iter,sec+(usec/1000000.0));
	}
	return 0;
}