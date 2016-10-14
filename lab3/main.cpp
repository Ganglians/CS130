// Name: Juan Chavez 
// Quarter, Year: Fall 2015
// Lab: 3
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
struct colors
{
	float r, g, b;
};

colors c[5];
int k = 0;

int r[5]={1,0,0,1,1};
int g[5]={0,1,0,0,1};
int b[5]={0,0,1,1,0};

//misc functions
void GLMousePress(int button, int state, int x, int y);
Point2D ScreenToWindowCoordinates(const Point2D &p);
void GLKeyboardPress(unsigned char key, int x, int y);

bool double_equal(double x, double y)
{return abs(x - y) < 1.0e-12;}

bool equal_pixel(const Point2D &a, const Point2D &b)
{	
	if(a.y != b.y)
	{
		return false;
	}
	
	return true;
}


//rendering functions
void ddaLine(double xi, double yi, double xf, double yf);
void drawLine(double x_0, double y_0, double x_1, double y_1);
void midCircle(int c1, int c2, int r);
void drawLine(Point2D p1, Point2D p2);
void Bezier(Point2D &p,double t);
void drawTri(Point2D p1, Point2D p2, Point2D p3);
void fillTri(Point2D p0, Point2D p1, Point2D p2);
void fillBottomFlatTri(Point2D p0, Point2D p1, Point2D p2);
void fillTopFlatTri(Point2D p0, Point2D p1, Point2D p2);

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
void renderPixel(double x, double y, float r = 1.0, float g = 1.0, float b = 1.0) //rgb colors
{
	glBegin(GL_POINTS);
	glColor3f(r, g, b);
	glVertex2f(x,y);
	glEnd();
}

//Output function to OpenGL Buffer
void GL_render()
{   	
	int s = pts.size();
	
	if((s % 3) == 0 && pts.size() != 0)
	{
		k = 0;
		int i = 0;
		while(i < s)
		{
			fillTri(pts[i], pts[i + 1], pts[i + 2]);
			i += 3;
			k++;
			if(k > 4)
				k = 0;
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
	glutKeyboardFunc(GLKeyboardPress);
	
	// The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
	// For the purposes of this lab, this is set to the number of pixels
	// in each dimension.
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void ddaLine(double xi, double yi, double xf, double yf)
{
	
	double dx, dy, m;
	dx = xf - xi;
	dy = yf - yi;
	
	double smallerx, largerx, smallery, largery;
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
		
		for(double i = smallery; i <= largery; ++i)
		{
			renderPixel(xi, i, r[k], g[k], b[k]);//***
		}
		
	}
	
	//horizontal line case
	if(dy == 0)
	{
		
		for(double i = smallerx; i <= largerx; ++i)
		{
			renderPixel(i, yi, r[k], g[k], b[k]);//***
		}
			
	}
	
	m = dy/dx;
	if(m < 0)//if slope is negative, have to start from the larger value of y first
	{
		double tmp = smallery;
		smallery = largery;
		largery = tmp;
	}

	//case when m is (0,1]
	if(abs(m) > 0 && abs(m) <= 1)
	{
		double y = smallery;
		for(double x = smallerx; x <= largerx; ++x)
		{
			renderPixel(x, y, r[k], g[k], b[k]);//***
			y = (double)(y + m);
		}
	}
	
	if(abs(m) > 1)
	{
		double x = smallerx;
		for(double y = smallery; y <= largery; ++y)
		{
			renderPixel(x,y, r[k], g[k], b[k]);//***
			x = (double)(x + 1/m);
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
	/*glBegin(GL_LINES);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd();
    glFlush();*/
   ddaLine(p1.x, p1.y, p2.x, p2.y);
}

void drawLine(double x_0, double y_0, double x_1, double y_1) 
{
	/*glBegin(GL_LINES);
    glVertex2f(x_0, y_0);
    glVertex2f(x_1, y_1);
    glEnd();
    glFlush();*/
    ddaLine(x_0, y_0, x_1, y_1);
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

void drawTri(Point2D p1, Point2D p2, Point2D p3)
{
	drawLine(p1, p2);
	drawLine(p2, p3);
	drawLine(p1, p3);
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

void GLKeyboardPress(unsigned char key, int x, int y)
{ 
	if (key == '0') 
	{	
		pts.resize(0);
		glClear(GL_COLOR_BUFFER_BIT);
		glutPostRedisplay();
	} 
}


int main(int argc, char** argv)
{	
	GLInit(&argc, argv);
	glutMainLoop();

	return 0;
}
