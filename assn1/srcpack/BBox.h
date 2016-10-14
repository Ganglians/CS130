/**
 *\file		BBox.h
 *
 *\brief	An axis aligned bounding box.
 *
 *\author	Xianming Chen
 *
 *\date		12 Jul 2004
 */


#ifndef _BBOX_H
#define _BBOX_H

#include "Point.h"


namespace columbia
{
  struct TriangleMesh ;
  
  struct BBox
  {
      Point min, max, center;
      BBox() { };
      
      BBox(TriangleMesh const&);
  } ;

  inline ostream& operator<<(ostream& os, BBox const& bbox)
  {
    return os << bbox.min << endl << bbox.max << endl;
  }
  
}//end namespace xchen


#endif
