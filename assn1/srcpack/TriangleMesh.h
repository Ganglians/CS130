/**
 *\file		TriangleMesh.h
 *
 *\brief	For reading from an obj 3D model file.
 *
 *\author	Xianming Chen\n
 *		Computer Science Department\n
 *		University of Utah
 *
 *\date		11 Jul 2004\n
 *		Copyright (c) 2004, University of Utah
 */


#ifndef _TRIANGLEMESH_H
#define _TRIANGLEMESH_H

#include "BBox.h"
#include <vector>


namespace columbia
{
  struct TriangleFace
  {
      TriangleFace(int I, int J, int K, int Ti=-1, int Tj=-1, int Tk=-1, int Ni=-1, int Nj=-1, int Nk=-1) : 
	i(I), j(J), k(K), ti(Ti), tj(Tj), tk(Tk), ni(Ni), nj(Nj), nk(Nk) { }
      int i, j, k;                                               // indices of 3 vertices.
      int ni, nj, nk;                                            // indices of 3 normals.
      int ti, tj, tk;                                            // indices of 3 tex coords.
  };
  
  struct TriangleMesh
  {
      TriangleMesh(const char* obj_fname);
      TriangleMesh()       { }
      void TriangulateSphere(int depth = 1);
      
      std :: vector<Vertex> V;           // vertex (x, y, z)
      std :: vector<Vector> N;           // normal (x, y, z)
      std :: vector<Vertex> T;           // texture (s, t, w)
      std :: vector<TriangleFace> F;     // triangle (i, j, k)
      int total_triangles;
      bool has_normal;
      BBox bbox;
  };

  inline ostream& operator<< (ostream& os, TriangleMesh const& msh)
  {
    for(int i=1; i<msh.V.size(); i++)
    {
      os << "v " << msh.V[i].x << " " << msh.V[i].y << " " << msh.V[i].z << endl;
    }
    os << endl;

    for(int i=1; i<msh.N.size(); i++)
    {
      os << "vn " << msh.N[i].x << " " << msh.N[i].y << " " << msh.N[i].z << endl;
    }
    os << endl;

    for(int i=0; i<msh.F.size(); i++)
    {
      if(msh.has_normal)
	os << "f " << msh.F[i].i << "//" << msh.F[i].ni << " " << msh.F[i].j << "//" << msh.F[i].nj << " " << msh.F[i].k << "//" << msh.F[i].nk << endl;
      else
	os << "f " << msh.F[i].i << " " << msh.F[i].j <<  " " << msh.F[i].k << endl;
    }
    os << endl;
    return os;
  }
    

}//end namespace xchen


#endif
