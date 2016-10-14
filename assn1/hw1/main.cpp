#include <GL/glut.h>
#include <math.h>
#include <cstdlib>
#include <cmath>
#include "point2d.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <float.h>

using namespace std;

//global variables
//used to determine the window dimensions width x height
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

//used to determine what color to make the triangles
int k = 0;
int r[5]={1,0,0,1,1};
int g[5]={0,1,0,0,1};
int b[5]={0,0,1,1,0};

//vector that will be used to store depth values
vector<double> zbuffer(WINDOW_WIDTH*WINDOW_HEIGHT, DBL_MAX);

//boolean values used to determine when user presses certain keys
bool rotation = false;
bool wireframe = false;
bool zoomin = false;
bool zoomout = false;

bool m_left = false;
bool m_right = false;
bool m_up = false;
bool m_down = false;

//variables used to represent polygons and any important constants
vector<Point2D> vertices; //store all given vertices
vector<Point2D> triFaces; //3d triangle 'faces'
vector<int> triFaceC; //coordinates

double A = 0, B = 0, C = 0, D = 0; //plane vars
double sumx=0, sumy=0, sumz=0;

//finding reference axis
//number of vertices and faces
double numV, numF;
//used to calculate axis of rotation
Point2D refPt;

//scaling factors for zoom
double zoom = 1.1;

//function prototypes

void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0);

void ddaLine(int xi, int yi, int xf, int yf);

bool equal_pixel(const Point2D &a, const Point2D &b);

//triangle rasterization functions
void fillTri(Point2D p0, Point2D p1, Point2D p2);
void fillBottomFlatTri(Point2D p0, Point2D p1, Point2D p2);
void fillTopFlatTri(Point2D p0, Point2D p1, Point2D p2);

//used to read specific polygon file
void readFile(char* filename);

//used to calculate the z value from plane representation
double calcZ(double x, double y);
void planeExn(Point2D a, Point2D b, Point2D c);

//transformation functions
void rotate();
void zoomIn();
void zoomOut();
void Left();
void Right();
void Up();
void Down();

//used to see if user pressed any keys relevant to the program
void GLKeyboardPress(unsigned char key, int x, int y);

//standard GL functions
void GL_render();
void GLInit(int* argc, char** argv);

//resets the depth buffer after transformations
void resetBuffer();


//function defenitions
// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(double x, double y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glBegin(GL_POINTS);
	glColor3f(r,g,b);//decides color of pixels

	//calculate z
	double z = calcZ(x,y);
	
	if((z <= zbuffer[x+y*800] || zbuffer[x+y*800] == DBL_MAX)&& z > 1e-30)
	{
		glVertex2f(x,y);
		zbuffer[x+y*800] = z;
	} 
	//glVertex2f(x,y);

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
	
	renderPixel(x,y, r[k], g[k], b[k]);

	for(int i=1;i<=nxt;i++)
	{
		x = x + xInc;
		y = y + yInc;
		renderPixel(x,y, r[k], g[k], b[k]);
	}
}

bool equal_pixel(const Point2D &a, const Point2D &b)
{return (a.y!=b.y)?false:true;}

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
    ddaLine(curx1, scanlineY, curx2, scanlineY);
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
    ddaLine(curx1, scanlineY, curx2, scanlineY);
  }
}

void readFile(char* filename)
{
	ifstream myfile;
	myfile.open(filename);
	Point2D temp;
	int numVertices,numTriangles;
	//vector<Point2D> vertices;

	if(myfile.is_open())
	{
		myfile >> numVertices;
		numV = numVertices;
		myfile >> numTriangles;
		numF = numTriangles;
		
		for(int i=0; i<numVertices; i++)
		{
			myfile >> temp.x;
			myfile >> temp.y;
			myfile >> temp.z;
			
			sumx+=temp.x;
			sumy+=temp.y;
			sumz+=temp.z;
			
			vertices.push_back(temp);
		}

		for(int i=0; i<numTriangles; i++)
		{
			int p0,p1,p2;
			myfile >> p0;
			myfile >> p1;
			myfile >> p2;
			
			triFaceC.push_back(p0);
			triFaceC.push_back(p1);
			triFaceC.push_back(p2);
			
			triFaces.push_back(vertices[p0]);
			triFaces.push_back(vertices[p1]);
			triFaces.push_back(vertices[p2]);
		}
	}
	else
	{
		cout<<"Unable to open file.\n";
		exit(-1);
	}
	myfile.close();
	
	refPt.x = sumx/numV;
	refPt.y = sumy/numV;
	refPt.z = sumz/numV;
}

double calcZ(double x, double y)
{return (C==0)?0:(D-A*x-B*y)/C;}

void planeExn(Point2D a, Point2D b, Point2D c)
{
	//vertices
	double v1[3], v2[3];
	v1[0] = b.x-a.x;
	v1[1] = b.y-a.y;
	v1[2] = b.z-a.z;

	v2[0] = c.x-a.x;
	v2[1] = c.y-a.y;
	v2[2] = c.z-a.z;

	A = v1[1]*v2[2] - v1[2]*v2[1];
	B = (v1[2]*v2[0] - v1[0]*v2[2]);
	C = v1[0]*v2[1] - v1[1]*v2[0];
	D = A*a.x + B*a.y + C*a.z;
}

void rotate()
{
	//Point2D refPt;
	
	double theta = M_PI/81;
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i] -= refPt;
		vertices[i].x = vertices[i].x*cos(theta)-vertices[i].z*sin(theta);
		vertices[i].z = vertices[i].x*sin(theta)+vertices[i].z*cos(theta);
		vertices[i] += refPt;
	}
	
	//reset vertices
	while(!triFaces.empty())
		triFaces.pop_back();
	
	for(int i=0; i<numF; i++)
	{
		int p0 = triFaceC[i];
		int p1 = triFaceC[i+1];
		int p2 = triFaceC[i+2];

		triFaces.push_back(vertices[p0]);
		triFaces.push_back(vertices[p1]);
		triFaces.push_back(vertices[p2]);
	}
	
	//resetBuffer();
	
}

void zoomIn()
{
	
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i] -= refPt;
		vertices[i] *= zoom; 
		vertices[i] += refPt;
	}
	
	
	//reset vertices
	while(!triFaces.empty())
		triFaces.pop_back();
	
	for(int i=0; i<numF; i++)
	{
		int p0 = triFaceC[i];
		int p1 = triFaceC[i+1];
		int p2 = triFaceC[i+2];

		triFaces.push_back(vertices[p0]);
		triFaces.push_back(vertices[p1]);
		triFaces.push_back(vertices[p2]);
	}	
}

void zoomOut()
{	
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i] -= refPt;
		vertices[i] /= zoom; 
		vertices[i] += refPt;
	}
	
	
	//reset vertices
	while(!triFaces.empty())
		triFaces.pop_back();
	
	for(int i=0; i<numF; i++)
	{
		int p0 = triFaceC[i];
		int p1 = triFaceC[i+1];
		int p2 = triFaceC[i+2];

		triFaces.push_back(vertices[p0]);
		triFaces.push_back(vertices[p1]);
		triFaces.push_back(vertices[p2]);
	}	
}

void Left()
{	
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i] -= refPt;
		vertices[i].x -= zoom; 
		vertices[i] += refPt;
	}
	
	
	//reset vertices
	while(!triFaces.empty())
		triFaces.pop_back();
	
	for(int i=0; i<numF; i++)
	{
		int p0 = triFaceC[i];
		int p1 = triFaceC[i+1];
		int p2 = triFaceC[i+2];

		triFaces.push_back(vertices[p0]);
		triFaces.push_back(vertices[p1]);
		triFaces.push_back(vertices[p2]);
	}	
}

void Right()
{	
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i] -= refPt;
		vertices[i].x += zoom; 
		vertices[i] += refPt;
	}
	
	
	//reset vertices
	while(!triFaces.empty())
		triFaces.pop_back();
	
	for(int i=0; i<numF; i++)
	{
		int p0 = triFaceC[i];
		int p1 = triFaceC[i+1];
		int p2 = triFaceC[i+2];

		triFaces.push_back(vertices[p0]);
		triFaces.push_back(vertices[p1]);
		triFaces.push_back(vertices[p2]);
	}	
}

void Up()
{	
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i] -= refPt;
		vertices[i].y += zoom; 
		vertices[i] += refPt;
	}
	
	
	//reset vertices
	while(!triFaces.empty())
		triFaces.pop_back();
	
	for(int i=0; i<numF; i++)
	{
		int p0 = triFaceC[i];
		int p1 = triFaceC[i+1];
		int p2 = triFaceC[i+2];

		triFaces.push_back(vertices[p0]);
		triFaces.push_back(vertices[p1]);
		triFaces.push_back(vertices[p2]);
	}	
}

void Down()
{	
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i] -= refPt;
		vertices[i].y -= zoom; 
		vertices[i] += refPt;
	}
	
	
	//reset vertices
	while(!triFaces.empty())
		triFaces.pop_back();
	
	for(int i=0; i<numF; i++)
	{
		int p0 = triFaceC[i];
		int p1 = triFaceC[i+1];
		int p2 = triFaceC[i+2];

		triFaces.push_back(vertices[p0]);
		triFaces.push_back(vertices[p1]);
		triFaces.push_back(vertices[p2]);
	}	
}

void GLKeyboardPress(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 't':
			wireframe = !wireframe;//toggle wireframe
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();
		break;
		
		case 'r':
			rotation = !rotation;//toggle rotation
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();	
		break;
				
		case 'i':
			zoomin = !zoomin;//toggle zoomin
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();	
		break;	
		
		case 'o':
			zoomout = !zoomout;//toggle zoomout
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();	
		break;
		
		case 'w':
			m_up = !m_up;//toggle m_up
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();			
		break;
		
		case 's':
			m_down = !m_down;//toggle m_down
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();			
		break;
				
		case 'a':
			m_left = !m_left;//toggle m_left
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();			
		break;		
				
		case 'd'://toggle m_right
			m_right = !m_right;
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();			
		break;
							
		default:
		break;
	}
}

void GL_render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//resetBuffer();

	k=0;
		if(rotation)
		{
			rotate();
			rotation = !rotation;
			resetBuffer();
		}
		
		if(zoomin)
		{
			zoomIn();
			zoomin = !zoomin;
			resetBuffer();
		}
		
		if(zoomout)
		{
			zoomOut();
			zoomout = !zoomout;
			resetBuffer();
		}
		
		if(m_left)
		{
			Left();
			m_left = !m_left;
			resetBuffer();
		}
		
		if(m_right)
		{
			Right();
			m_right = !m_right;
			resetBuffer();
		}
		
		if(m_up)
		{
			Up();
			m_up = !m_up;
			resetBuffer();
		}
		
		if(m_down)
		{
			Down();
			m_down = !m_down;
			resetBuffer();
	ls	}
	for(int i=0; i<triFaces.size(); i+=3)
	{

		
		//determine the equation of the plane for the current triangle
		planeExn(triFaces[i],triFaces[i+1],triFaces[i+2]);
		
		//user wants filled in triFaces
		if(!wireframe)
			fillTri(triFaces[i],triFaces[i+1],triFaces[i+2]);

		//user wants wireframe
		else   
		{
			ddaLine(triFaces[i].x,triFaces[i].y,triFaces[i+1].x,triFaces[i+1].y);
			ddaLine(triFaces[i].x,triFaces[i].y,triFaces[i+2].x,triFaces[i+2].y);
			ddaLine(triFaces[i+2].x,triFaces[i+2].y,triFaces[i+1].x,triFaces[i+1].y);
		}
/*
		Point2D a(700,700,200), b(200,700,700), c(700, 700, 700);
		planeExn(a,b,c);
		fillTri(a,b,c);
*/		
		k++;
		if(k > 4) k=0;
	}

	glutSwapBuffers();
}

void resetBuffer()
{
	int zsize = zbuffer.size();
	for(int i=0;i<zsize;i++)
		zbuffer[i] = DBL_MAX;
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
	glutKeyboardFunc(GLKeyboardPress);
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char** argv)
{	
	readFile(argv[1]);
	GLInit(&argc, argv);
	//readFile();
	//glutKeyboardFunc(processNormalKeys);
	//glutSpecialFunc(processSpecialKeys);
	glutMainLoop();
	
	return 0;
}
