/**
 *\file		BBox.cpp
 *
 *\brief	
 *
 *\author	Xianming Chen\n
 *		Computer Science Department\n
 *		University of Utah
 *
 *\date		12 Jul 2004\n
 *		Copyright (c) 2004, University of Utah
 */


#include "BBox.h"
#include "TriangleMesh.h"


namespace columbia
{
  BBox :: BBox(TriangleMesh const& msh)
  {
    float minx, miny, minz, maxx, maxy, maxz;
    minx = maxx = msh.V[1].x;
    miny = maxy = msh.V[1].y;
    minz = maxz = msh.V[1].z;
    
    for(int v = 2; v < msh.V.size(); v++)
    {
      float x = msh.V[v].x;
      float y = msh.V[v].y;
      float z = msh.V[v].z;

      if(x < minx) minx = x;
      else if(x > maxx) maxx = x;

      if(y < miny) miny = y;
      else if(y > maxy) maxy = y;

      if(z < minz) minz = z;
      else if(z > maxz) maxz = z;
    }
    min = Point(minx, miny, minz);
    max = Point(maxx, maxy, maxz);
    center = (min + max) / 2;
  }
  


}//end namespace xchen
