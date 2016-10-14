/**
 *\file		RGB.h
 *
 *\brief        RGB class wrapping pixel color-triple: red, green, and blue.
 *
 *              Each rgb component is of a float value in [<b><var>0.0, 1.0</var></b>].
 *
 */


#ifndef _RGB_H
#define _RGB_H

#include <iostream>
using namespace std;


namespace columbia
{
  class RGB
  {
      float rgb[3];
      
  public:
      RGB(float r=0., float g=0., float b=0.)  { rgb[0] = r; rgb[1] = g; rgb[2] = b; }

      float& operator[](int idx)               { return *(rgb+idx); }   //non const method: return lvalue
      float operator[](int idx) const          { return *(rgb+idx); }   //const method: return rvalue

      RGB& operator*=(RGB const& rhs)          { for(int i=0; i<3; i++) rgb[i] *= rhs[i]; return *this; }
      RGB operator*(RGB const&rhs) const       { RGB ret(*this); return ret *= rhs; }

      RGB& operator*=(float scale)             { for(int i=0; i<3; i++) rgb[i] *= scale; return *this; }
      RGB operator*(float scale) const         { RGB ret(*this); return ret *= scale; }

      RGB& operator+=(RGB const& rhs)          { for(int i=0; i<3; i++) rgb[i] += rhs[i]; return *this; }
      RGB operator+(RGB const& rhs) const      { RGB ret(*this); return ret+=rhs; }

      bool operator==(RGB const& rhs) const    { return equal(rgb, rgb+3, rhs.rgb); }
      
  };

  inline ostream& operator << (ostream& os, RGB const& rgb)
  {
    return os << "(" << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << ",)";
  }
  

}//end namespace columbia


#endif
