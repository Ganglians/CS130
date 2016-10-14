#include <iostream>
#include <fstream>
#include <vector>
#include <GL/glut.h>
#include "point3d.h"
#include "vertex.h"
#include "face.h"
#include "list.h"

using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

int numOfFaces = 0;
int numOfVertex = 0;
int view = 0;

vector<Point3D> vertices;
vector<vertex> vert;
vector<face> faces;
vector<Point3D> listPts;
vector<Point3D> newListPts;

void readFile()
{
	Point3D tempPt;
	
	vertex tempVert;

	ifstream iFile("./samplemeshes/sampletri.txt");
 	if (iFile.is_open()) {
		iFile >> numOfVertex;
		iFile >> numOfFaces;
		
		//store vertex points
		for(int i=0; i<numOfVertex; i++)
		{	
			iFile >> tempPt.x; 
			iFile >> tempPt.y;
			iFile >> tempPt.z;
			tempVert.insertPoints(tempPt);
			vertices.push_back(tempPt);
			vert.push_back(tempVert);
		}

		//store face points
		for(int i=0; i<numOfFaces; i++)
		{	
			iFile >> tempPt.x; 
			iFile >> tempPt.y;
			iFile >> tempPt.z;
			listPts.push_back(tempPt);
			face tempFace(vertices[tempPt.x],vertices[tempPt.y],vertices[tempPt.z]);
			faces.push_back(tempFace);
		}
		iFile.close();
	}
	else
		cout<<"Unable to Open File";
	
}

//gets the index of the vector in 
int getIndexOfPt(Point3D pt)
{
	for(int i=0; i<vert.size(); i++)
	{
		if(vert[i].getPoint()==pt)
			return i;
	}
	cout<<"could not find point in vertex array"<<endl;
	return -1;
}

//gets the Adjacency List
void calculateAdjList()
{
	Point3D pts;
	int pt1,pt2,pt3;
	
	for(int i=0; i<vert.size(); i++)
	{
		vert[i].deleteAdj();
	}
	
	for(int i=0; i<listPts.size(); i++)
	{
		pts = listPts[i];
		pt1 = listPts[i].x;
		pt2 = listPts[i].y;
		pt3 = listPts[i].z;

		vert[pt1].insertAdj(vertices[pt2]);
		vert[pt1].insertAdj(vertices[pt3]);
		vert[pt2].insertAdj(vertices[pt1]);
		vert[pt2].insertAdj(vertices[pt3]);
		vert[pt3].insertAdj(vertices[pt1]);
		vert[pt3].insertAdj(vertices[pt2]);
	}
	/*
	for(int i=0; i<vert.size(); i++)
	{
		cout<<vert[i].adjPts.size()<<endl;
	}*/
}

//find the other points from an edge
void findOtherTwo(Point3D a, Point3D b, Point3D &c, Point3D &d)
{
	Point3D tempC,tempD;
	bool flagC=false;
	bool flagD=false;
	
	for(int i=0; i<faces.size(); i++)
	{
		if(faces[i].hasEdge(a,b) && !flagC)
		{
			c=faces[i].getThirdPoint(a,b);
			flagC=true;
		}
		else if(faces[i].hasEdge(a,b) && !flagD)
		{
			d=faces[i].getThirdPoint(a,b);
			flagD=true;
		}
	}
}

//Calculate the odd points
Point3D calculateOdd(Point3D a, Point3D b, Point3D c, Point3D d) 
{
	a*=0.375;//a*=0.375;
	b*=0.375;//b*=0.375;
	c*=0.125;
	d*=0.125;

	return a+b+c+d; // 3/8a + 3/8b + 1/8c + 1/8d
}

double calculateBeta(int n)
{
	double beta=0.0;

	if(n>3)
		beta = 0.375*(double)n;
	else if(n==3)
		beta = 0.1875;
	else
		cout<<"error calculating beta."<<endl;

	return beta;
}

Point3D calculateEven(Point3D pt)
{
	vector<Point3D> adjList;
	double beta,mulPt;
	int index=getIndexOfPt(pt);
	Point3D currPt,sum,tmpSum;

	vert[index].getAdjList(adjList); //get adjacency list for currVertex
	beta = calculateBeta(adjList.size());
	//coutf<<"index: "<<index<<endl;
	//cout<<"n: "<<(double)adjList.size()<<endl;
	//cout<<"beta: "<<beta<<endl;
	
	for(int i=0;i<adjList.size();i++)
	{
		tmpSum=adjList[i];
		tmpSum*=beta;
		sum+=tmpSum;
	}
	//cout<<"beta: "<<beta<<endl;
	mulPt= 1.0-((double)adjList.size()*beta);
	currPt=pt;
	currPt*=mulPt;
	currPt+=sum;
	
	return currPt;
}

// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glBegin(GL_POINTS);
	//glColor3f(0.0f,0.0f,1.0f); //blue color
	glVertex2i(x,y);
    	glEnd();
}

void ddaLine(int x1, int y1, int x2, int y2)
{
	float x,y,xInc,yInc,dx,dy;
	int next;
	
	dx = x2 - x1;
	dy = y2 - y1;
	
	if(abs(dx)>abs(dy))
		next=abs(dx);
	else
		next=abs(dy);

	xInc = dx / next;
	yInc = dy / next;
	
	x = x1;
	y = y1;
	
	renderPixel(x,y);

	for(int i=1;i<=next;i++)
	{
		x = x + xInc;
		y = y + yInc;
		renderPixel(x,y);
	}
}

//Makes a subdivision given a mesh
void subDivide()
{
	//vector<Point3D> newAdjList;
	//vector<Point3D> newEven;
	vector<face> newFaces;
	//vector<vertex> newVertices;
	Point3D a1,b1,c1,d1;
	Point3D a2,b2,c2,d2;
	Point3D a3,b3,c3,d3;

	vertex v1,v2,v3;
	int p1,p2,p3;

	//Calculate the odd & even points for each face
	for(int i=0; i<faces.size(); i++)
	{		
		a1=faces[i].pt1;
		b1=faces[i].pt2;
		findOtherTwo(a1,b1,c1,d1);
		
		Point3D oddPt1(calculateOdd(a1,b1,c1,d1));	//calculate new odd of edge a-b
		if(getIndexOfPt(oddPt1)==-1)			//if it's not in the vector of 
		{
			vertex v1(oddPt1);
			vert.push_back(v1);
		}

		a2=faces[i].pt1;
		b2=faces[i].pt3;
		findOtherTwo(a2,b2,c2,d2);

		Point3D oddPt2(calculateOdd(a2,b2,c2,d2));
		if(getIndexOfPt(oddPt2)==-1)
		{
			vertex v2(oddPt2);
			vert.push_back(v2);
		}

		a3=faces[i].pt2;
		b3=faces[i].pt3;
		findOtherTwo(a3,b3,c3,d3);

		Point3D oddPt3(calculateOdd(a3,b3,c3,d3));
		if(getIndexOfPt(oddPt3)==-1)
		{
			vertex v3(oddPt3);
			vert.push_back(v3);
		}

		Point3D listPt1(getIndexOfPt(faces[i].pt1),getIndexOfPt(oddPt1),getIndexOfPt(oddPt2));
		newListPts.push_back(listPt1);

		Point3D listPt2(getIndexOfPt(faces[i].pt2),getIndexOfPt(oddPt1),getIndexOfPt(oddPt3));
		newListPts.push_back(listPt2);

		Point3D listPt3(getIndexOfPt(faces[i].pt3),getIndexOfPt(oddPt2),getIndexOfPt(oddPt3));
		newListPts.push_back(listPt3);

		Point3D listPt4(getIndexOfPt(oddPt1),getIndexOfPt(oddPt2),getIndexOfPt(oddPt3));
		newListPts.push_back(listPt4);
		
		face newFace1(faces[i].pt1,oddPt1,oddPt2);
		face newFace2(faces[i].pt2,oddPt1,oddPt3);
		face newFace3(faces[i].pt3,oddPt2,oddPt3);
		face newFace4(oddPt1,oddPt2,oddPt3);
		
		newFaces.push_back(newFace1);
		newFaces.push_back(newFace2);
		newFaces.push_back(newFace3);
		newFaces.push_back(newFace4);
		cout<<vert.size()<<endl;	
	}
	
	for(int i=0;i<newListPts.size();i++)
		cout<<newListPts[i].x<<","<<newListPts[i].y<<","<<newListPts[i].z<<endl;

	for(int i=0;i<vert.size();i++)
		cout<<i<<": "<<vert[i].getPoint().x<<","<<vert[i].getPoint().y<<","<<vert[i].getPoint().z<<endl;
	

	//delete all faces
	while(faces.size()>0)
	{
		faces.erase(faces.begin());
	}

	//update faces vector
	for(int i=0; i<newFaces.size(); i++)
	{
		faces.push_back(newFaces[i]);			
	}		
}

void sideView()
{
	for(int j=0; j<faces.size(); j++)
	{
		ddaLine(8*faces[j].pt1.z,8*faces[j].pt1.y,8*faces[j].pt2.z,8*faces[j].pt2.y);
		ddaLine(8*faces[j].pt2.z,8*faces[j].pt2.y,8*faces[j].pt3.z,8*faces[j].pt3.y);
		ddaLine(8*faces[j].pt3.z,8*faces[j].pt3.y,8*faces[j].pt1.z,8*faces[j].pt1.y);
	}
}

void frontView()
{
	for(int j=0; j<faces.size(); j++)
	{
		ddaLine(8*faces[j].pt1.x,8*faces[j].pt1.y,8*faces[j].pt2.x,8*faces[j].pt2.y);
		ddaLine(8*faces[j].pt2.x,8*faces[j].pt2.y,8*faces[j].pt3.x,8*faces[j].pt3.y);
		ddaLine(8*faces[j].pt3.x,8*faces[j].pt3.y,8*faces[j].pt1.x,8*faces[j].pt1.y);
	}	
}

void topView()
{
	for(int j=0; j<faces.size(); j++)
	{
		ddaLine(8*faces[j].pt1.x,8*faces[j].pt1.z,8*faces[j].pt2.x,8*faces[j].pt2.z);
		ddaLine(8*faces[j].pt2.x,8*faces[j].pt2.z,8*faces[j].pt3.x,8*faces[j].pt3.z);
		ddaLine(8*faces[j].pt3.x,8*faces[j].pt3.z,8*faces[j].pt1.x,8*faces[j].pt1.z);
	}
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
	if (key == 32)
	{
		calculateAdjList();
		subDivide();
		glutPostRedisplay();
	}
}

void processSpecialKeys(int key, int x, int y) {

	switch(key) {
		case GLUT_KEY_UP :
				view=0;
				//topView();				
				break;
		case GLUT_KEY_DOWN :
				view=1;
				//frontView();
				break;
		case GLUT_KEY_RIGHT :
				view=2;
				//sideView();				
				break;
		case GLUT_KEY_LEFT :
				view=2;
				//sideView();				
				break;
	}
	glutPostRedisplay();
}

void GL_render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	switch(view)
	{
		case 0:
			topView();
			break;
		case 1:
			frontView();
			break;
		case 2:
			sideView();
			break;
		default:
			frontView();
			break;
	}	
	glutSwapBuffers();
}



int main(int argc, char** argv)
{	
	GLInit(&argc, argv);
	readFile();
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutMainLoop();
	
	return 0;
}
