#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>


void calc(unsigned int *A,unsigned int *C,int i,int dest,int dim)
{
	unsigned int j,k,sum;
	for(;i<dest;i++)
		for(j=0;j<dim;j++)
		{
			sum=0;
			for(k=0;k<dim;k++)
				sum+=A[i*dim+k]*A[k*dim+j];
			C[i*dim+j]=sum;
		}
	return;
}

unsigned int check(unsigned int *C,int dim)
{
	unsigned int sum=0;
	int i,j;
	for(i=0;i<dim;i++)
		for(j=0;j<dim;j++)
			sum+=C[i*dim+j];
	return sum;
}

int main()
{
	int Aid=shmget(0,2600000,IPC_CREAT|0660);
	int Cid=shmget(0,2600000,IPC_CREAT|0660);
	int i,j,k,dim,now,leng,pro;
	struct timeval start, end;
	printf("Input the matrix dimension: ");
	scanf("%d",&dim);
	unsigned int *A=(unsigned int *)shmat(Aid,NULL,0);
	unsigned int *C=(unsigned int *)shmat(Cid,NULL,0);
	for(i=0;i<dim;i++)
		for(j=0;j<dim;j++)
			A[i*dim+j]=i*dim+j;
	for(pro=1;pro<=16;pro++)
	{
		for(i=0;i<dim;i++)
			for(j=0;j<dim;j++)
				C[i*dim+j]=0;
		leng=dim/pro;
		gettimeofday(&start,0);
		for(now=0;now<pro-1;now++)
			if(fork()==0)
				break;
		if(now==pro-1)
			calc(A,C,now*leng,dim,dim);
		else
		{
			calc(A,C,now*leng,(now+1)*leng,dim);
			exit(0);
		}
		signal(SIGCHLD, SIG_IGN);
		wait(NULL);
		gettimeofday(&end,0);
		int sec=end.tv_sec-start.tv_sec;
		int usec=end.tv_usec-start.tv_usec;
		printf("Multiplying matrices using %d process\n",pro);
		printf("Elapsed time: %f ms, Checksum: %u\n",sec*1000+(usec/1000.0),check(C,dim));
	}
	shmctl(Aid,IPC_RMID,NULL);
	shmctl(Cid,IPC_RMID,NULL);
	shmdt(A);
	shmdt(C);
	return 0;
}