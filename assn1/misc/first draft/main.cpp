#include <stdio.h>
#include <iostream>
#include <math.h>
#include <cmath>
#include <fstream>
#include <vector>
#include <GL/glut.h>
#include "point3d.h"
#include "vertex.h"
#include "face.h"

using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;

//global variables
int numOfFaces = 0;
int numOfVertex = 0;
int view = 0;
vector<Point3D> vertices;
vector<vertex> vert;
vector<face> faces;
vector<Point3D> listPts;
vector<Point3D> newListPts;

//function prototypes
void ddaLine(int xi, int yi, int xf, int yf);
void readFile();
void sideView();
void frontView();
void topView();
void processSpecialKeys(int key, int x, int y);
//void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0);



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

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow("CS 130 - <Juan Chavez>");
	glutDisplayFunc(GL_render);
	//glutMouseFunc(GLMousePress);

	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
}

// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glBegin(GL_POINTS);
	glVertex2f(x,y);
	glEnd();
}

//dda algorithm for renderling lines
void ddaLine(int xi, int yi, int xf, int yf)
{
	int nxt;
	float x, y, xInc, yInc, dx, dy;

	dx = xf - xi;
	dy = yf - yi;
	
	if(abs(dx)>abs(dy))
	{
		nxt=abs(dx);
	}
	else
	{
		nxt=abs(dy);
	}

	xInc = dx / nxt;
	yInc = dy / nxt;
	
	x = xi;
	y = yi;
	
	renderPixel(x,y);

	for(int i=1;i<=nxt;i++)
	{
		x = x + xInc;
		y = y + yInc;
		renderPixel(x,y);
	}
}

void readFile()
{
	Point3D fPt;
	
	vertex fVertex;

	ifstream inFile("./modelview/samplecylinder.txt");
 	if (inFile.is_open()) {
		inFile >> numOfVertex;
		inFile >> numOfFaces;
		
		//store vertex points
		for(int i=0; i<numOfVertex; i++)
		{	
			inFile >> fPt.x; 
			inFile >> fPt.y;
			inFile >> fPt.z;
			fVertex.insertPoints(fPt);
			vertices.push_back(fPt);
			vert.push_back(fVertex);
		}

		//store face points
		for(int i=0; i<numOfFaces; i++)
		{	
			inFile >> fPt.x; 
			inFile >> fPt.y;
			inFile >> fPt.z;
			listPts.push_back(fPt);
			face tmpFace(vertices[fPt.x],vertices[fPt.y],vertices[fPt.z]);
			faces.push_back(tmpFace);
		}
		inFile.close();
	}
	else
		cout<<"Unable to Open File";
	
}

void sideView()
{
	for(int j=0; j<faces.size(); j++)
	{
		ddaLine(faces[j].pt1.z,faces[j].pt1.y,faces[j].pt2.z,faces[j].pt2.y);
		ddaLine(faces[j].pt2.z,faces[j].pt2.y,faces[j].pt3.z,faces[j].pt3.y);
		ddaLine(faces[j].pt3.z,faces[j].pt3.y,faces[j].pt1.z,faces[j].pt1.y);
	}
}

void frontView()
{
	for(int j=0; j<faces.size(); j++)
	{
		ddaLine(faces[j].pt1.x,faces[j].pt1.y,faces[j].pt2.x,faces[j].pt2.y);
		ddaLine(faces[j].pt2.x,faces[j].pt2.y,faces[j].pt3.x,faces[j].pt3.y);
		ddaLine(faces[j].pt3.x,faces[j].pt3.y,faces[j].pt1.x,faces[j].pt1.y);
	}	
}

void topView()
{
	for(int j=0; j<faces.size(); j++)
	{
		ddaLine(faces[j].pt1.x,faces[j].pt1.z,faces[j].pt2.x,faces[j].pt2.z);
		ddaLine(faces[j].pt2.x,faces[j].pt2.z,faces[j].pt3.x,faces[j].pt3.z);
		ddaLine(faces[j].pt3.x,faces[j].pt3.z,faces[j].pt1.x,faces[j].pt1.z);
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

void fillTri(Point2D p0, Point2D p1, Point2D p2)
{
	if(p0.y > p1.y)
	{swap(p0, p1);}
	
	if(p0.y > p2.y)
	{swap(p0, p2);}
	
	if(p1.y > p2.y)
	{swap(p1, p2);}
	
	if(equal_pixel(p1, p2))
	{
		fillBottomFlatTri(p0, p1, p2);
	}
	
	if(equal_pixel(p0, p1))
	{
		fillTopFlatTri(p0, p1, p2);		
	}
	
	else
	{
		Point2D p3;
		p3.x = (double)(p0.x + ((double)(p1.y - p0.y) / (double)(p2.y - p0.y)) * (p2.x - p0.x));
		p3.y = p1.y;

		fillBottomFlatTri(p0, p1, p3);
		fillTopFlatTri(p1, p3, p2);		
	}
}

void fillBottomFlatTri(Point2D p0, Point2D p1, Point2D p2)
{
  double invslope1 = (p1.x - p0.x) / (p1.y - p0.y); //1/m
  double invslope2 = (p2.x - p0.x) / (p2.y - p0.y);

  double curx1 = p0.x;
  double curx2 = p0.x;

  for (double scanlineY = p0.y; scanlineY <= p1.y; scanlineY++)
  {
    drawLine(curx1, scanlineY, curx2, scanlineY);
    curx1 += invslope1;
    curx2 += invslope2;
  }
}

void fillTopFlatTri(Point2D p0, Point2D p1, Point2D p2)
{
  float invslope1 = (p2.x - p0.x) / (p2.y - p0.y);
  float invslope2 = (p2.x - p1.x) / (p2.y - p1.y);

  float curx1 = p2.x;
  float curx2 = p2.x;

  for (int scanlineY = p2.y; scanlineY > p0.y; scanlineY--)
  {
    curx1 -= invslope1;
    curx2 -= invslope2;
    drawLine(curx1, scanlineY, curx2, scanlineY);
  }
}

int main(int argc, char** argv)
{	
	GLInit(&argc, argv);
	readFile();
	//glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutMainLoop();
	
	return 0;
}
