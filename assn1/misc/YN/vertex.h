#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <iostream>
#include "point3d.h"

using namespace std;

class vertex 
{
	public:
		vertex ();
		vertex (Point3D a);
		~vertex ();
		
		void insertPoints(Point3D a);
		Point3D getPoint();
		
		void insertAdj(Point3D a);
		void deleteAdj();
		double getNumAdj();

		void printAdj();
		bool isPtInList(Point3D a);

		void getAdjList(vector<Point3D> &list);
		vector<Point3D> adjPts;

	private: 
		Point3D pt1;

		int findPt(Point3D a);
		

};

// Construction
vertex::vertex () 
{
	
}

vertex::vertex (Point3D a) 
{
	pt1=a;
}

// Destructor
vertex::~vertex () 
{
	
}

void vertex::insertPoints(Point3D a) 
{
	pt1=a;
}

Point3D vertex::getPoint() 
{
	return pt1;
}

void vertex::insertAdj(Point3D a)
{
	if(!isPtInList(a))
		adjPts.push_back(a);
}

void vertex::deleteAdj()
{
	adjPts.clear();
	/*while(adjPts.size()>0)
	{
		adjPts.erase(adjPts.begin());
	}*/
}

double vertex::getNumAdj()
{
	return (double)adjPts.size();
}

void vertex::printAdj()
{
	if(adjPts.size()>0)
	{
		for(int i=0; i<adjPts.size(); i++)
			cout<<adjPts[i].x<<","<<adjPts[i].y<<","<<adjPts[i].z<<endl;
	}
	else
		cout<<"Empty Adj List"<<endl;
}

bool vertex::isPtInList(Point3D a)
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

int vertex::findPt(Point3D a)
{
	int index=0;

	for(int i=0; i<adjPts.size(); i++)
	{
		if(adjPts[i] == a)
			index=i;
	}
	/*if(index==-1)
		cout<<"point not in adjacentList"<<endl;*/
	
	return index;
}

void vertex::getAdjList(vector<Point3D> &list)
{
	while(list.size()>0)
	{
		list.erase(list.begin());
	}
	for(int i=0; i<adjPts.size(); i++)
		list.push_back(adjPts[i]);
}

#endif
