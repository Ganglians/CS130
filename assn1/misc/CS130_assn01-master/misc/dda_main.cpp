// Name: Steven Em
// Quarter, Year: Fall, 2013
// Lab: lab 01
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include "point2d.h"

using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glPointSize(5.0);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
}

void swap(Point2D &click_1, Point2D &click_2) {
  Point2D tmp = click_1;
  click_1 = click_2;
  click_2 = tmp;
}

void renderLine(Point2D start, Point2D end) {
	if(start.x > end.x) 
	 swap(start, end);	

	double m = (double)(end.y - start.y)/(end.x - start.x);
	double y = (double)start.y;
	double x = (double)start.x;
	if(m >= -1 && m < 1) {
	 while(x <= end.x) {
	  renderPixel(x, floor(y));
	  x++;
	  y += m;
	 }
	} else {
	 if(y > end.y)
	  swap(start, end);

	 y = (double)start.y;
	 x = (double)start.x;
	 double m1 = 1/m;
	 while(y <= end.y) {
	 cout <<"true" << endl;
	  renderPixel(floor(x), y);
	  y++;
	  x += m1;
	 }
	}
}

void GL_render() {
	glClear(GL_COLOR_BUFFER_BIT);
	Point2D V0(100, 0);
	Point2D V1(100, 500);
	renderLine(V1, V0);
	glutSwapBuffers();
}

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow("CS 130 - Steven Em");

	// The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
	// For the purposes of this lab, this is set to the number of pixels
	// in each dimension.
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	glutDisplayFunc(GL_render);
}

int main(int argc, char** argv)
{	
	GLInit(&argc, argv);
	glutMainLoop();

	return 0;
}


