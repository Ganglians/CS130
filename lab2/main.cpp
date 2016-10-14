// Name: Juan Chavez 
// Quarter, Year: Fall 2015
// Lab: 2
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include "point2d.h"
#include <vector>


//dynamic programming - make a table
using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

//function prototypes
void ddaLine(int xi, int yi, int xf, int yf);
void midCircle(int c1, int c2, int r);
void GLMousePress(int button, int state, int x, int y);
Point2D ScreenToWindowCoordinates(const Point2D &p);
void drawLine(Point2D p1, Point2D p2);
void Bezier(Point2D &p,double t);

//global vars
vector<Point2D> pts;
double clicks = 0;

int factorial(int n)
{
    if (n<=1)
        return(1);
    else
        n=n*factorial(n-1);
    return n;
}

float bernstein_coeff(float n,float k)
{
    float bern;
    bern = factorial(n) / (factorial(k)*factorial(n-k));
    return bern;
}

// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glBegin(GL_POINTS);
	glVertex2f(x,y);
	glEnd();
}

//Output function to OpenGL Buffer
void GL_render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    int i = 0;
    int n = pts.size();
    /*if(pts.size() >= 2)
    {
    for(; i < n; i++)
    {
		drawLine(pts[i], pts[i + 1]);
		//renderPixel(pts[i].x, pts[i].y);
	}
	}*/
	
	//Point2D p1 = pts[0];
		
	if(pts.size() >= 2)
	{
	//draw control lines
	//for(int j = 0; j < (n-1); ++j)
		//drawLine(pts[j], pts[j+1]);		
	Point2D p1;
	p1.x = pts[i].x;
	p1.y = pts[i].y;

    for(double t = 0.0;t <= 1.0; t += 0.02)
    {
		Point2D p2;
		Bezier(p2, t);
        drawLine(p1, p2);
        
        //set the next p1 equal to p2 for next iteration
        p1.x = p2.x;
        p1.y = p2.y;
    }
    
	}	
    glutSwapBuffers();
}

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	
	// ...
	// Complete this function
	// ...
	glutCreateWindow("CS 130 - <Juan Chavez>");
	glutDisplayFunc(GL_render);
	glutMouseFunc(GLMousePress);

	// The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
	// For the purposes of this lab, this is set to the number of pixels
	// in each dimension.
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
}

void ddaLine(int xi, int yi, int xf, int yf)
{
	
	float dx, dy, m;
	dx = xf - xi;
	dy = yf - yi;
	
	int smallerx, largerx, smallery, largery;
	if(xi > xf)
	{
		smallerx = xf;
		largerx = xi;
	}
	else
	{
		smallerx = xi;
		largerx = xf;
	}
	
	if(yi > yf)
	{
		smallery = yf;
		largery = yi;
	}
	else
	{
		smallery = yi;
		largery = yf;
	}	

	//vertical line case
	if(dx == 0)
	{
		
		for(int i = smallery; i <= largery; ++i)
		{
			renderPixel(xi, i);
		}
		
	}
	
	//horizontal line case
	if(dy == 0)
	{
		
		for(int i = smallerx; i <= largerx; ++i)
		{
			renderPixel(i, yi);
		}
			
	}
	
	m = dy/dx;
	if(m < 0)//if slope is negative, have to start from the larger value of y first
	{
		int tmp = smallery;
		smallery = largery;
		largery = tmp;
	}

	//case when m is (0,1]
	if(abs(m) > 0 && abs(m) <= 1)
	{
		int y = smallery;
		for(int x = smallerx; x <= largerx; ++x)
		{
			renderPixel(x, y);
			y = (int)(y + m);
		}
	}
	
	if(abs(m) > 1)
	{
		int x = smallerx;
		for(int y = smallery; y <= largery; ++y)
		{
			renderPixel(x,y);
			x = (int)(x + 1/m);
		}
	}
}

void midCircle(int c1, int c2 ,int r) //center coordinates and radius
{
	int h = 1 - r;
	int x = 0;
	int y = r;
	
	renderPixel(c1 + x, c2 + y);//initial point
	
	while(y > x)
	{
		if (h < 0)//select E
		{
			h = h + 2*x +3;
		}
		
		else//select SE
		{
			h = h + 2*(x-y) + 5;
			y = y -1;
		}
		x = x + 1;
		renderPixel(c1 + x, c2 + y);
		renderPixel(c1 - x, c2 + y);
		renderPixel(c1 + x, c2 - y);
		renderPixel(c1 - x, c2 - y);
		
		renderPixel(c1 + y, c2 + x);
		renderPixel(c1 - y, c2 + x);
		renderPixel(c1 + y, c2 - x);
		renderPixel(c1 - y, c2 - x);
	}
}


void drawLine(Point2D p1, Point2D p2) 
{
	glBegin(GL_LINES);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd();
    glFlush();
}


void Bezier(Point2D &p,double t) 
{
    p.x = 0; p.y = 0;   
    for (int i = 0; i<clicks; i++)
    {
        p.x = p.x + bernstein_coeff((float)(clicks - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (clicks - 1 - i)) * pts[i].x;
        p.y = p.y + bernstein_coeff((float)(clicks - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (clicks - 1 - i)) * pts[i].y;
    }
}

//monitor mouse presses and store the point in window coordinates
void GLMousePress(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		pts.push_back(ScreenToWindowCoordinates(Point2D(x,y)));
		++clicks;
		glutPostRedisplay();
	}
}

//convert coordinates to screen coordinates;
Point2D ScreenToWindowCoordinates(const Point2D &p)
{
	GLdouble model[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	Point2D converted;
	GLdouble temp;
	
	gluUnProject(p.x, viewport[3] - p.y, 0, model, projection, viewport, &converted.x, &converted.y, &temp);
	return converted;
}



int main(int argc, char** argv)
{	
	GLInit(&argc, argv);
	glutMainLoop();

	return 0;
}
