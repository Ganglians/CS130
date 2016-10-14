// Name: Juan Chavez 
// Quarter, Year: Fall 2015
// Lab: 1
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include <cmath>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

//function prototypes
void ddaLine(int xi, int yi, int xf, int yf);
void midCircle(int c1, int c2, int r);

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
	//renderPixel(0,0);
	//renderPixel(100, 200);
	
	//line calls
	ddaLine(400, 400, 400, 600);
	ddaLine(400, 400, 600, 600);
	ddaLine(400, 400, 600, 400);
	ddaLine(400, 400, 600, 200);
	ddaLine(400, 400, 400, 200);
	ddaLine(400, 400, 200, 200);
	ddaLine(400, 400, 200, 400);
	ddaLine(400, 400, 200, 600);
	
	
	midCircle(400, 400, 50);
	midCircle(500, 500, 20);
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
		/*renderPixel(c2 + x, c1 + y);
		renderPixel(c2 - x, c1 + y);
		renderPixel(c2 + x, c1 - y);	
		renderPixel(c2 - x, c1 - y);*/
	}
}

int main(int argc, char** argv)
{	
	GLInit(&argc, argv);
	glutMainLoop();

	return 0;
}
