#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void Free(char** a,int size)
{
	for(int i=0;i<size;i++)
	{
		free(a[i]);
		a[i]=NULL;
	}
}

int main()
{
	int n[2],nonBlock,fd,file,words;
	char str[500],*cmd[2][5],*s,pro[20];
	const char *cut=" ";
	while(1)
	{
		int pipefd[2];
		printf(">");
		nonBlock=0;file=0;words=0;
		for(int j=0;j<2;j++)
		{
			for(int i=0;i<5;i++)
				cmd[j][i]=(char*)malloc(sizeof(char)*20);
			n[j]=0;
		}

		fgets(str,sizeof(str),stdin);
		str[strlen(str)-1]='\0';
		s=strtok(str,cut);
		
		for(int i=0;s!=NULL;i++,n[words]++)
		{
			if(*s=='|')
			{
				pipe(pipefd);
				words++;
				n[words]--;
				i=-1;
			}
			if(*s=='>')
			{
				n[words]--;
				s=strtok(NULL,cut);
				fd=open(s,O_WRONLY|O_CREAT|O_TRUNC,0664);
				file=1;
			}
			else if(*s!='&')
				strcpy(cmd[words][i],s);
			else
			{
				n[words]--;
				nonBlock=1;
			}
			s=strtok(NULL,cut);
		}

		if(nonBlock)
			if(fork()==0)
			{
				if(fork()==0)
				{
					if(file)
					{
						if(words)
						{
							if(fork()==0)
							{
								if(file)
								{
									dup2(fd,1);
									close(fd);
								}
								Free(cmd[0]+n[0],5-n[0]);
								if(words)
								{
									Free(cmd[1],5);
									close(pipefd[0]);
									dup2(pipefd[1],1);
									close(pipefd[1]);
								}
								execvp(cmd[0][0],cmd[0]);
							}
							else
							{
								wait(NULL);
								Free(cmd[0],5);
								if(words)
								{
									if(fork()==0)
									{
										Free(cmd[1]+n[1],5-n[1]);
										close(pipefd[1]);
										dup2(pipefd[0],0);
										close(pipefd[0]);
										execvp(cmd[1][0],cmd[1]);
									}
									else
									{
										Free(cmd[1],5);
										close(pipefd[1]);
										close(pipefd[0]);
										wait(NULL);
									}
								}
							}
						}
						else
						{
							dup2(fd,1);
							close(fd);
						}
					}
					Free(cmd[0]+n[0],5-n[0]);
					execvp(cmd[0][0],cmd[0]);
				}
				else
				{
					Free(cmd[0],5);
					exit(0);
				}
			}
			else
			{
				Free(cmd[0],5);
				wait(NULL);
			}
		else
			if(fork()==0)
			{
				if(file)
				{
					dup2(fd,1);
					close(fd);
				}
				Free(cmd[0]+n[0],5-n[0]);
				if(words)
				{
					Free(cmd[1],5);
					close(pipefd[0]);
					dup2(pipefd[1],1);
					close(pipefd[1]);
				}
				execvp(cmd[0][0],cmd[0]);
			}
			else
			{
				wait(NULL);
				Free(cmd[0],5);
				if(words)
				{
					if(fork()==0)
					{
						Free(cmd[1]+n[1],5-n[1]);
						close(pipefd[1]);
						dup2(pipefd[0],0);
						close(pipefd[0]);
						execvp(cmd[1][0],cmd[1]);
					}
					else
					{
						Free(cmd[1],5);
						close(pipefd[1]);
						close(pipefd[0]);
						wait(NULL);
					}
				}
			}
	}
	return 0;
}