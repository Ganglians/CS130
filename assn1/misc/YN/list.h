#ifndef __LIST_H__
#define __LIST_H__

#include <iostream>
#include "point3d.h"

using namespace std;

class list 
{
	public:
		list ();
		list (int a);
		~list ();
		
		void set(int a);
		//int getIndex();
		
		void insertAdj(int a);
		//void deleteAdj(int ptIndex);

		void printAdj();
		bool isPtInList(int a);

		//void getAdjList(vector<Point3D> &list);

		int pt1;
		vector<int> adjPts;

		//int findPt(Point3D a);
		

};

// Construction
list::list () 
{
	
}

list::list (int a) 
{
	pt1=a;
}

// Destructor
list::~list () 
{
	
}

void list::set(int a) 
{
	pt1=a;
}

void list::insertAdj(int a)
{
	if(!isPtInList(a))
		adjPts.push_back(a);
}
/*
void list::deleteAdj(int ptIndex)
{
	if(isPtInList(ptIndex))
	{
		adjPts.erase(adjPts.begin() + findPt(ptIndex));
	}	
	//else
	//	cout<<"point: "<<a.x<<","<<a.y<<","<<a.z<<" not in adjacentList"<<endl;
}*/

void list::printAdj()
{
	if(adjPts.size()>0)
	{
		for(int i=0; i<adjPts.size(); i++)
			cout<<adjPts[i]<<endl;
	}
	else
		cout<<"Empty Adj List"<<endl;
}
/*
int vertex::findPt(int pt)
{
	int index=0;
	for(int i=0; i<adjPts.size(); i++)
	{
		if(adjPts[i] == pt)
			index=i;
	}
	
	return index;
}*/

bool list::isPtInList(int a)
{
	bool res=false;

	for(int i=0; i<adjPts.size(); i++)
	{
		if(adjPts[i] == a)
			res=true;
	}
	/*if(index==-1)
		cout<<"point not in adjacentList"<<endl;*/
	
	return res;
}

#endif
