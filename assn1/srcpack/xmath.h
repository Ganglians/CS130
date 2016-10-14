/**
 *\file		xmath.h
 *
 *\brief	some commonly used math routines.
 *
 *\author	Xianming Chen
 */


#ifndef _XMATH_H
#define _XMATH_H



namespace columbia
{
  const double pi = 3.1416;
  

  template<typename T>
  T interpolate(double t, T const& P1, T const& P2)
  {
    return P1*(1-t) + P2*t;
  }

  inline int round(double a)
  {
    if(a>=0) 
      return int(a + .49999);
    else
      return int(a - .49999);
  }
  

}//end namespace columbia


#endif
