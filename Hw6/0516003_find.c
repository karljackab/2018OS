#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned long long int t_inode,req[4]={0};
char t_name[200];
double t_smin,t_smax;
#define two20 1048576

int check(char name[200],int inode,double size)
{
	if(req[0]&&inode!=t_inode)
		return 0;
	if(req[1]&&strcmp(name,t_name))
		return 0;
	if(req[2]&&size<t_smin)
		return 0;
	if(req[3]&&size>t_smax)
		return 0;
	return 1;
}

void go(char current[200])
{
	DIR *dir;
	struct dirent *ptr;
	struct stat buf;
	char path[200];
	int i;
    dir=opendir(current);
    while((ptr=readdir(dir))!=NULL)
    {
    	sprintf(path,"%s/%s",current,ptr->d_name);
    	stat(path,&buf);

    	if(check(ptr->d_name,buf.st_ino,(double)buf.st_size/two20))
    		printf("%s %ld %f MB\n",path,buf.st_ino,(double)buf.st_size/two20);

        if(!strcmp(ptr->d_name,".")||!strcmp(ptr->d_name,".."))
            continue;

        if(S_ISDIR(buf.st_mode)) // Directory
        	go(path);
    }
    closedir(dir);
}

int main(int argc,char* argv[])
{
    int i;
    for(i=2;i<argc;i+=2)
    {
    	if(strcmp(argv[i],"-inode")==0)
    	{
    		req[0]=1;
    		t_inode=atoi(argv[i+1]);
    	}
    	else if(strcmp(argv[i],"-name")==0)
    	{
    		req[1]=1;
    		strcpy(t_name,argv[i+1]);
    	}
    	else if(strcmp(argv[i],"-size_min")==0)
    	{
    		req[2]=1;
    		t_smin=atof(argv[i+1]);
    	}
    	else if(strcmp(argv[i],"-size_max")==0)
    	{
    		req[3]=1;
    		t_smax=atof(argv[i+1]);
    	}
    }

    go(argv[1]);

    return 0;
}