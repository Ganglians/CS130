#ifndef __FACE_H__
#define __FACE_H__

#include <iostream>
#include "point3d.h"
#include "vertex.h"

using namespace std;

class face
{
	public:
		face();
		face(Point3D a, Point3D b, Point3D c);
		~face();
		
		void insertPoints(Point3D a, Point3D b, Point3D c);
		//void getPoints(Point3D &a, Point3D &b, Point3D &c);
		void printPoints();

		bool hasEdge(Point3D a, Point3D b);

		Point3D getThirdPoint(Point3D a, Point3D b);

		Point3D pt1;
		Point3D pt2;
		Point3D pt3;
};

face::face()
{
	
}

face::face(Point3D a, Point3D b, Point3D c)
{
	pt1=a;
	pt2=b;
	pt3=c;
}

face::~face()
{
	
}

void face::insertPoints(Point3D a, Point3D b, Point3D c) 
{
	pt1=a;
	pt2=b;
	pt3=c;
}
/*
void face::getPoints(Point3D &a, Point3D &b, Point3D &c)
{
	a.x=pt1.x;
	a.y=pt1.y;
	a.z=pt1.z;
	b.x=pt2.x;
	b.y=pt2.y;
	b.z=pt2.z;
	c.x=pt3.x;
	c.y=pt3.y;
	c.z=pt3.z;
}
*/
void face::printPoints()
{
	cout<<pt1.x<<","<<pt1.y<<","<<pt1.z<<endl;
	cout<<pt2.x<<","<<pt2.y<<","<<pt2.z<<endl;
	cout<<pt3.x<<","<<pt3.y<<","<<pt3.z<<endl;
}

bool face::hasEdge(Point3D a, Point3D b)
{
	bool contains=false;
	
	if(a == pt1 && b == pt2 || a == pt2 && b == pt1)
		contains=true;
	else if(a == pt1 && b == pt3 || a == pt3 && b == pt1)
		contains=true;
	else if(a == pt2 && b == pt3 || a == pt3 && b == pt2)
		contains=true;
	/*else
	{
		contains=false;
		cout<<"dnc"<<endl;
	}*/
	return contains;
}

Point3D face::getThirdPoint(Point3D a, Point3D b)
{
	Point3D c;
	if(a == pt1 && b == pt2 || a == pt2 && b == pt1)
	{
		c = pt3;
	}
	else if(a == pt1 && b == pt3 || a == pt3 && b == pt1)
	{
		c = pt2;
	}
	else if(a == pt2 && b == pt3 || a == pt3 && b == pt2)
	{
		c = pt1;
	}
	return c;
}

#endif
