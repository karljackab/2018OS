#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <map>
#include <list>
#include <ctime>
using namespace std;

int main()
{
	fstream fin;
	int target;
	double faltRatio;
	int missCnt,hitCnt;
	int queue[1025];
	double start,total=0;

	// FIFO
	cout<<"FIFO---"<<endl;
	cout<<"size\tmiss\thit\tpage fault ratio"<<endl;
	bool occ[10000]={0};
	for(int i=128;i<=1024;i<<=1)
	{
		faltRatio=0;	missCnt=0;	hitCnt=0;
		int len=0,now=0;
		fin.open("trace.txt",ios::in);
		memset(occ,0,sizeof(bool)*10000);
		start=clock();
		while(fin>>target)
		{
			if(occ[target])
				hitCnt++;
			else if(len<i)
			{
				missCnt++;
				occ[target]=true;
				queue[len]=target;
				len++;
			}
			else
			{
				missCnt++;
				occ[queue[now]]=false;
				queue[now]=target;
				occ[target]=true;
				now=(now+1)%i;
			}
		}
		total+=(clock()-start)/(double)(CLOCKS_PER_SEC);
		faltRatio=(double)missCnt/(missCnt+hitCnt);
		cout<<fixed<<setprecision(9);
		cout<<i<<"\t"<<missCnt<<"\t"<<hitCnt<<"\t"<<faltRatio<<endl;
		fin.close();
	}

	//LRU
	cout<<"LRU---"<<endl;
	cout<<"size\tmiss\thit\tpage fault ratio"<<endl;
	map<int,list<int>::iterator> LRUhash;
	list<int> LRUlist;
	map<int,list<int>::iterator>::iterator it;
	for(int i=128;i<=1024;i<<=1)
	{
		faltRatio=0;	missCnt=0;	hitCnt=0;
		int len=0,now=0;
		fin.open("trace.txt",ios::in);
		start=clock();
		while(fin>>target)
		{
			it=LRUhash.find(target);
			if(it!=LRUhash.end())
			{
				hitCnt++;
				LRUlist.erase(it->second);
				LRUlist.push_front(target);
				LRUhash[target]=LRUlist.begin();
			}
			else if(len<i)
			{
				missCnt++;
				LRUlist.push_front(target);
				LRUhash[target]=LRUlist.begin();
				len++;
			}
			else
			{
				missCnt++;
				LRUhash.erase(LRUlist.back());
				LRUlist.pop_back();
				LRUlist.push_front(target);
				LRUhash[target]=LRUlist.begin();
			}
		}
		faltRatio=(double)missCnt/(missCnt+hitCnt);
		total+=(clock()-start)/(double)(CLOCKS_PER_SEC);
		cout<<fixed<<setprecision(9);
		cout<<i<<"\t"<<missCnt<<"\t"<<hitCnt<<"\t"<<faltRatio<<endl;
		LRUhash.erase(LRUhash.begin(),LRUhash.end());
		LRUlist.clear();
		fin.close();
	}
	cout<<"FIFO + LRU cost "<<total<<" s"<<endl;
	return 0;
}