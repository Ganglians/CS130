// CS130 Fall 2013: Computer Graphics
// point2d.h
//
// This file does not need to be modified
/////////////////////////////////////////
#ifndef __POINT2D_H__
#define __POINT2D_H__

struct Point2D
{
	double x;
	double y;
	double z;

	Point2D() : x(0.0), y(0.0), z(0.0) {}
	Point2D(const double & nx, const double & ny, const double & nz) : x(nx), y(ny), z(nz) {}
	Point2D(const double & nx, const double & ny) : x(nx), y(ny) {}
	
	Point2D operator+(const Point2D & rhs) const { return Point2D(x + rhs.x, y + rhs.y, z+rhs.z); }
	Point2D operator-(const Point2D & rhs) const { return Point2D(x - rhs.x, y - rhs.y, z-rhs.z); }
	Point2D operator*(double val) const { return Point2D(x * val, y * val, z * val); }
	Point2D operator/(double val) const { return Point2D(x / val, y / val, z / val); }
	Point2D operator+=(const Point2D & rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Point2D operator-=(const Point2D & rhs) { x -= rhs.x; y -= rhs.y; z -=rhs.z; return *this; }
	Point2D operator*=(double val) { x *= val; y *= val, z *= val; return *this; }
	Point2D operator/=(double val) { x /= val; y /= val; z /=val; return *this; }

	Point2D GLscreenToWindowCoordinates(const Point2D &p);
};

#endif

	Point2D GLscreenToWindowCoordinates(const Point2D &p)
	{
		GLdouble model[16];
		glGetDoublev(GL_MODELVIEW_MATRIX,model);
		GLdouble projection[16];
		glGetDoublev(GL_PROJECTION_MATRIX,projection);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT,viewport);
		Point2D converted;
		GLdouble temp;
		gluUnProject(p.x, viewport[3]-p.y,0,model,projection,viewport,&converted.x, &converted.y, &temp);
		return converted;
	}

