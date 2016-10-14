// Name:Juan Chavez
// Quarter, Year: Fall 2014
// Lab: 7
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#include <cmath>
#include <vector>
#include <GL/glut.h>
using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const float VIEW_LEFT = 0.0;
const float VIEW_RIGHT = WINDOW_WIDTH;
const float VIEW_BOTTOM = 0.0;
const float VIEW_TOP = WINDOW_HEIGHT;
const float VIEW_NEAR = -400;
const float VIEW_FAR = 400;

// Retrieve random float
float randFloat()
{
	return rand() / static_cast<float>(RAND_MAX);
}
float randFloat(float min, float max)
{
	return randFloat() * (max - min) + min;
}

// A simple wrapper for store 3D vectors
struct Vector3
{
	float x;
	float y;
	float z;
    
	Vector3() : x(0.0), y(0.0), z(0.0)
	{}
    
	Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
	{}
    
	Vector3(const Vector3 & v)
    : x(v.x), y(v.y), z(v.z)
	{}
    
	Vector3 operator+(const Vector3 & rhs) const
	{ return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	Vector3 operator-(const Vector3 & rhs) const
	{ return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	Vector3 operator*(float rhs) const
	{ return Vector3(x * rhs, y * rhs, z * rhs); }
	Vector3 operator/(float rhs) const
	{ return Vector3(x / rhs, y / rhs, z / rhs); }
	Vector3 operator+=(const Vector3 & rhs)
	{ x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Vector3 operator-=(const Vector3 & rhs)
	{ x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	Vector3 operator*=(float rhs)
	{ x *= rhs; y *= rhs; z *= rhs; return *this; }
	Vector3 operator/=(float rhs)
	{ x /= rhs; y /= rhs; z /= rhs; return *this; }
    
	float magnitude() const
	{ return sqrt(x * x + y * y + z * z); }
	void normalize()
	{ *this /= magnitude(); }
	Vector3 normalized() const
	{ return *this / magnitude(); }
	float dot(const Vector3 & rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
	Vector3 cross(const Vector3 & rhs) const
	{
		return Vector3(y * rhs.z - z * rhs.y,
                       z * rhs.x - x * rhs.z,
                       x * rhs.y - y * rhs.x);
	}
};

// A simple wrapper to store colors
struct Color3d
{
	float r;
	float g;
	float b;
	float a;
    
	Color3d()
    : r(0.0), g(0.0), b(0.0), a(1.0)
	{}
	Color3d(float r, float g, float b, float a = 1.0)
    : r(r), g(g), b(b), a(a)
	{}
};

void renderPixel(int x, int y, Color3d& color, float sz = 1.0)
{
	glPointSize(sz);
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}


struct Sphere
{
    Vector3 center;
    float radius;
    Color3d color;
    Sphere() : radius(1.0) {}
    Sphere(const Vector3& cent, const float& R, const Color3d& col)
    {
        if (R > 0.0) {radius = R;} else {radius = 1.0;}
        center = cent;  color = col;
    }
};

vector<Sphere> spheres;

void loadSpheres()
{
    /* TODO
     Add some spheres to the "spheres" vector.*/
     int j,k,l;
     j=k=l=100;
    for(int i = 0; i < 16; i++)
    {
        Vector3 c;
        c.x = j;//randFloat(100, 400);
        c.y = k;//randFloat(100, 400);
        c.z = l;//randFloat(100, 400);
        
        j+=50;
        k+=50;
        l+=50;
        
        float r = randFloat(1, 100);
        float R = randFloat(0,1);
        float G = randFloat(0,1);
        float B = randFloat(0,1);
        float an = randFloat(0,1);
        Color3d color(R,G,B,an);
        
        Sphere t(c, r, color);	
        spheres.push_back(t);
    }
}

struct Ray
{
    Vector3 origin;
    Vector3 direction;
    Ray() {Vector3 d(0.0, 0.0, 1.0);  direction = d;}
    Ray(const Vector3& o, const Vector3& dir)
    {
        origin = o;
        Vector3 d(0.0, 0.0, 1.0);
        float mag = dir.magnitude();
        if (mag > 0.0) {d = dir;}
        direction = d;
    }
};

bool rayIntersectsSphere(const Ray& ray, const Sphere& sphere, float* t_small = NULL, float* t_large = NULL)
{
    //Ray can be written origin + direction * t where t >= 0
    //Sphere can be written as (x - center.x)^2 + (y - center.y)^2 + (z - center.z)^2 = radius^2
    //Need to determine where these equations coincide.  If
    
    if (t_small != NULL) {*t_small = -1.0;} //These default values mean these values are invalid.
    if (t_large != NULL) {*t_large = -1.0;}
    
    /*
     TODO
     The ray is written as origin + direction * t where t >= 0.
     Write a function to solve for t_small <= t_large.  These are the different values of t
     for which the ray intersects the sphere.  These t-values are solutions to a quadratic equation
     which will have 0, 1, or 2 real solutions.  If there are 0 solutions, return false.
     If neither t_small nor t_large are >= 0, return false.  Otherwise, return true.
     */
     float vmag=ray.direction.magnitude();
     float a=vmag*vmag;//a = ||v|| ^ 2

     Vector3 tmp=ray.origin-sphere.center;
     float b=(ray.direction*2).dot(tmp);//b=2v(p-x_0)
     float c=tmp.magnitude()*tmp.magnitude()-sphere.radius*sphere.radius;//||p-x_0||^2-R^2
     float des=b*b-4*a*c;//descriminant b^2-4ac
     
     if(des < 0)//no intersections
		{return false;} 

     float it_min=(-b-(float)sqrt(double(b*b-4*a*c)))/(2*a);
     float it_max=(-b+(float)sqrt(double(b*b-4*a*c)))/(2*a);

     if(it_min > 0.0)
		*t_small=it_min;
     else
		*t_small=-1;

     if(it_max >0.0)
		*t_large=it_max;
     else
        *t_large=-1; 
	
     return (*t_small != -1)||(*t_large != -1)?true:false;
}

//Returns -1 if no sphere should appear in pixel, otherwise returns index of sphere.
int pixelOn(float pixel_x, float pixel_y)
{
    Ray current(Vector3(pixel_x, pixel_y, 0.0), Vector3(0.0, 0.0, 1.0));
    int num_spheres = (int) spheres.size();
    float min = -1.0;
    int min_i = -1;
    for (int i = 0; i < num_spheres; i++)
    {
        /*
         TODO
         Fill this in.
         The loop should make min_i the index of the first sphere intersected by current ray.
         If none exists, min_i will stay = -1.  Use the float min to keep track of smallest
         nonnegative t-value.
         */
		float t_min=-1, t_large=-1;
        bool on = rayIntersectsSphere(current,spheres[i],&t_min,&t_large); 
		//min_i=(on&&min==-1)?i:-1;
		//min=(on&&min==-1)?t_min:-1; 
        
		if(on&&t_min<min||on&&min==-1)
		{	
			min=t_min;	
            min_i=i;                
		}
		//float t_small = -1, t_large = -1;
		//bool draw = rayIntersectsSphere(current,spheres[i],&t_small,&t_large);
		
		//if((draw && min == -1)||(draw && tsmall<min)) 
		//{
			//min = tsmall;
			//min_i = i;
		//}
		//if(draw && tsmall < min)
		//{
		//	min = tsmall;
		//	min_i = i;
		//}
	 
    }
    return min_i;
}

void GLrender();

//Initialiizes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Lab 7 - Juan Chavez");
	glutDisplayFunc(GLrender);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glOrtho(VIEW_LEFT, VIEW_RIGHT, VIEW_BOTTOM, VIEW_TOP, VIEW_NEAR, VIEW_FAR);
}

int main(int argc, char** argv)
{
	GLInit(&argc, argv);
    loadSpheres();
	glutMainLoop();
	return 0;
}

void GLrender()
{
	glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < WINDOW_WIDTH; i++)
    {
        for (int j = 0; j < WINDOW_HEIGHT; j++)
        {
            int which_sphere = pixelOn(i, 800 - j);
            if (which_sphere >= 0)
            {
                renderPixel(i, 800 - j, spheres[which_sphere].color);
            }
        }
    }
    
	glFlush();	
	glutSwapBuffers();
}
