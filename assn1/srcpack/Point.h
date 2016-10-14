/**
 *\file		Point.h
 *
 *\brief	Point is a float triple, also type defined as Vertex and Vector.
 *
 *\author	Xianming Chen
 *
 *\date		11 Jul 2004\n
 */


#ifndef _POINT_H
#define _POINT_H

#include <iostream>

using namespace std;

namespace columbia
{
  struct Point 
  {
      Point(float X=.0, float Y=.0, float Z=.0) : x(X), y(Y), z(Z) { }
      float x, y, z;

      Point GetUnit() const { return Point(x/GetNorm(), y/GetNorm(), z/GetNorm()); }
      

      float operator*(Point const& rhs) const 
      {
	float dot_pro = 0.0;
	dot_pro += x * rhs.x;
	dot_pro += y * rhs.y;
	dot_pro += z * rhs.z;
	return dot_pro;
      }
      float GetNorm() const 
      { 
	float nm = 0.0;
	nm += x * x;
	nm += y * y;
	nm += z * z;
	return sqrt(nm);
      }

      Point& operator+=(Point const& rhs) { x += rhs.x, y += rhs.y, z += rhs.z; return *this; }
      Point& operator-=(Point const& rhs) { x -= rhs.x, y -= rhs.y, z -= rhs.z; return *this; }
      Point operator-() { return Point(-x, -y, -z); }
      Point operator+(Point const& rhs) const { Point tmp(*this); tmp += rhs; return tmp; }
      Point operator-(Point const& rhs) const { Point tmp(*this); tmp -= rhs; return tmp; }
      Point& operator*=(float scale) { x *= scale, y *= scale, z *= scale; return *this; }
      Point operator*(float scale) const { Point tmp(*this); tmp *= scale; return tmp; }
      Point& operator/=(float scale) { x /= scale, y /= scale, z /= scale; return *this; }
      Point operator/(float scale) const { Point tmp(*this); tmp /= scale; return tmp; }
  };
  
  typedef Point Vertex;
  typedef Point Vector;

  inline ostream& operator<<(ostream& os, Point const& P)
  {
    return os << P.x << ", " << P.y << ", " << P.z;
  }
  
}//end namespace xchen


#endif
