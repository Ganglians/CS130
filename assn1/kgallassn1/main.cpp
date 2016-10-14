// Name: Katherine Gallaher
// Quarter, Year: Fall 2014
// Lab: M 8-11
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
//INCLUDES
#include <GL/glut.h>
#include <math.h>
#include <cstdlib>
#include <cmath>
#include "point2d.h"
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

//GLOBAL VARIABLES
vector<double> zbuffer(800*800);
vector<Point2D> triangles;
double planeA=0, planeB=0, planeC=0, planeD=0;
double numFaces=0, numVertices=0;
double sumX=0, sumY=0, sumZ=0;

bool wireframe=false;
int n=0;
int r[5]={1,0,0,1,1};
int g[5]={0,1,0,0,1};
int bb[5]={0,0,1,1,1};
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

//FUNCTION DECLARATIONS	
void renderPixel(double x, double y, float r , float g , float b);
void renderTriangle(Point2D a, Point2D b, Point2D c);
void GL_render();
void GLInit(int* argc, char** argv);
void GLKeyboardPress(unsigned char key, int x, int y);
void renderLine(int x0, int y0, int x1, int y1);
void readFile(char* filename);
void planeEquation(Point2D a, Point2D b, Point2D c);
double findZ(double x, double y);

// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(double x, double y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glBegin(GL_POINTS);
	glColor3f(r,g,b);//decides color of pixels

	//calculate z
	double z = findZ(x,y);
	if(z <= zbuffer[x+y*800] || zbuffer[x+y*800] == 0)
	{
		glVertex2f(x,y);
		zbuffer[x+y*800] = z;
	} 
	//glVertex2f(x,y);

	glEnd();
}

//EVALUATES KEYBOARD PRESS BY USER
void GLKeyboardPress(unsigned char key, int x, int y)
{
	if(key=='w')
	{
		if(wireframe) 
			wireframe = false;
		else
			wireframe=true;
		glClear(GL_COLOR_BUFFER_BIT);
		glutPostRedisplay();
	}
}

//RENDERS LINE STARTING AT (X0, Y1) AND FINISHING AT (X1,Y1)
void renderLine(int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
   
    int steps;
    if (abs((float)dx) > abs((float)dy))
        steps = abs((float)dx);
    else
        steps = abs((float)dy);

    float stepx = static_cast<float>(dx) / steps;
    float stepy = static_cast<float>(dy) / steps;

    float x = x0;
    float y = y0;
    for (int i = 0; i <= steps; ++i) {
        renderPixel((int)(x + 0.5), (int)(y + 0.5));
        x += stepx;
        y += stepy;
    }
}

//RENDER A TRIANGLE THAT IS FLAT ON BOTTOM
void renderTop(Point2D b, Point2D c, Point2D d)
{
	for(double y = d.y; y<=c.y; y++)//top triangle
	{
		bool leftinfinity = false,rightinfinity = false; 
		if(!(c.x-b.x))
			rightinfinity = true;
		if(!(c.x-d.x))
			leftinfinity=true;

		double mleft = double(c.y-d.y)/double(c.x-d.x);
		double mright = double(c.y-b.y)/double(c.x-b.x);

		double bleft = d.y-(mleft* d.x);
		double bright = b.y-(mright* b.x);

		double x0 = (1/mleft)*y-(bleft/mleft);
		double x1 = (1/mright)*y-(bright/mright);

		if(leftinfinity) x0 = c.x;
		if(rightinfinity) x1 = c.x;
			
		if(x0 > x1)
			swap(x0,x1);
		for(double x=x0; x<=x1; x++)
			renderPixel(x,y,r[n],g[n],bb[n]);
	}
}
//RENDER A TRIANGLE THAT IS FLAT ON TOP
void renderBottom(Point2D a, Point2D b, Point2D d)
{
	for(double y= a.y; y<=d.y; y++)//bottom triangle
	{
		bool leftinfinity = false,rightinfinity = false; 
		if(!(b.x-a.x))
			rightinfinity = true;
		if(!(d.x-a.x))
			leftinfinity=true;

		double mleft = double(d.y-a.y)/double(d.x-a.x);
		double mright = double(b.y-a.y)/double(b.x-a.x);

		double bleft = a.y-(mleft*a.x);
		double bright = a.y-(mright*a.x);

		double x0 = (1/mleft)*y-(bleft/mleft);
		double x1 = (1/mright)*y-(bright/mright);

		if(leftinfinity) x0 = a.x;
		if(rightinfinity) x1 = a.x;
			
		if(x0>x1)
			swap(x0,x1);
		for(double x=x0; x<=x1; x++)
			renderPixel(x,y,r[n],g[n],bb[n]);
	}	
}
//RENDER A TRIANGLE WITH VERTICES AT a, b, c
void renderTriangle(Point2D a, Point2D b, Point2D c)
{//they are now arranged by y values, a is lowest, c is highest

	if(a.y > b.y)
		swap(a,b);
	if(a.y > c.y)
		swap(a,c);
	if(b.y > c.y)
		swap(b,c);

	if(a.y == b.y && b.y== c.y)//it's a line
	{
		if(a.x > b.x)
			swap(a,b);
		if(a.x > c.x)
			swap(a,c);
		if(b.x > c.x)
			swap(b,c);
		renderLine(a.x, a.y, c.x, c.y);

	}
	else if(b.y == c.y) //top is horizontal
		renderBottom(a,b,c);
	else if(a.y==b.y) //bottom is horizontal 
		renderTop(a,c,b);
	else
	{
		Point2D d;//Pnew
		d.x = a.x+(double(b.y-a.y)/double(c.y-a.y))*(c.x-a.x);
		d.y = b.y;
		renderTop(b,c,d);
		renderBottom(a,b,d);
	}

}

//SAVES INFORMATION IN THE FILE THAT CONTAINS VERTICES AND TRIANGLES
void readFile(char* filename)
{
	ifstream myfile;
	myfile.open(filename);
	Point2D temp;
	int numpoints,numtriangles;
	vector<Point2D> points;

	if(myfile.is_open())
	{
		myfile >> numpoints;
		myfile >> numtriangles;

		for(int i=0; i<numpoints; i++)
		{
			myfile >> temp.x;
			myfile >> temp.y;
			myfile >> temp.z;

			points.push_back(temp);
		}

		for(int i=0; i<numtriangles; i++)
		{
			int one,two,three;
			myfile >> one;
			myfile >> two;
			myfile >> three;

			triangles.push_back(points[one]);
			triangles.push_back(points[two]);
			triangles.push_back(points[three]);
		}
	}
	else
	{
		cout<<"there was an error opening the file."<<endl;
		exit(1);
	}
	myfile.close();
}

//FINDS THE EQUATION OF THE PLANE WITH VERTICES AT a, b,c
void planeEquation(Point2D a, Point2D b, Point2D c)
{
	double v1[3], v2[3];
	v1[0] = b.x-a.x;
	v1[1] = b.y-a.y;
	v1[2] = b.z-a.z;

	v2[0] = c.x-a.x;
	v2[1] = c.y-a.y;
	v2[2] = c.z-a.z;

	//find the coeff. for the equation of the plane, global
	planeA = v1[1]*v2[2] - v1[2]*v2[1];
	planeB = (v1[2]*v2[0] - v1[0]*v2[2]);
	planeC = v1[0]*v2[1] - v1[1]*v2[0];
	planeD = planeA*a.x + planeB*a.y + planeC*a.z;
}

//FINDS THE Z VALUE OF A POINT GIVEN x, y COORDINATES
double findZ(double x, double y)
{
	return (1/planeC)*(planeD - planeA*x - planeB*y);
}

//RENDERS PIXELS TO SCREEN
void GL_render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	n=0;
	for(int i=0; i<triangles.size(); i+=3)
	{
		//determine the equation of the plane for the current triangle
		planeEquation(triangles[i],triangles[i+1],triangles[i+2]);
		
		//user wants filled in triangles
		if(!wireframe)
			renderTriangle(triangles[i],triangles[i+1],triangles[i+2]);

		//user wants wireframe
		else   
		{
			renderLine(triangles[i].x,triangles[i].y,triangles[i+1].x,triangles[i+1].y);
			renderLine(triangles[i].x,triangles[i].y,triangles[i+2].x,triangles[i+2].y);
			renderLine(triangles[i+2].x,triangles[i+2].y,triangles[i+1].x,triangles[i+1].y);
		}
/*
		Point2D a(700,700,200), b(200,700,700), c(700, 700, 700);
		planeEquation(a,b,c);
		renderTriangle(a,b,c);
*/		
		n++;
		if(n > 4) n=0;
	}
	glutSwapBuffers();
}

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CS 130 Assignment 1  - Katherine Gallaher");
	glutDisplayFunc(GL_render);
	
	glutKeyboardFunc(GLKeyboardPress);

	// The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
	// For the purposes of this lab, this is set to the number of pixels
	// in each dimension.
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);

	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char** argv)
{	
	readFile(argv[1]);
	GLInit(&argc, argv);
	glutMainLoop();
	return 0;
}
