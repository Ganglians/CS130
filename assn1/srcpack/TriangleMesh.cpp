/**
 *\file		TriangleMesh.cpp
 *
 *\brief	
 *
 *\author	Xianming Chen
 *
 *\date		11 Jul 2004
 */


#include "TriangleMesh.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <string>

using namespace std;


namespace columbia
{

  static void skip_line(istream& is)
  {
    char next;
    while( is >> noskipws >> next && next != '\n' );
  }
  
  static bool skip_comment_line(istream& is, char comment_tag)
  {
    char next;
    while( is >> skipws >> next ) 
    {
      is.putback(next);
      if(next == comment_tag) skip_line(is);
      else
	return true;
    }
    return false;
  }


  TriangleMesh :: TriangleMesh(const char* obj_fname) : has_normal(false)
  {
    ifstream is(obj_fname);

    if(is)
    {
      V.push_back( Vertex() );                           // the 0'th vertex is ignored.
      N.push_back( Vector() );                           // the 0'th normal is ignored.
      T.push_back( Vector() );                           // the 0'th texture is ignored.

      bool no_normal = true;
      bool no_tex = true;

      while( skip_comment_line(is, '#') ) 
      {
	string ele_id;
	float x, y, z;    

	if( ! (is >> ele_id) ) break;

	if(ele_id == "v")
	{
	  is >> x >> y >> z;
	  V.push_back( Vertex(x, y, z) );
	}
	else if(ele_id == "vt")
	{
	  no_tex = false;
	  is >> x >> y >> z;
	  is.clear();                           // is z(i.e. w) is not available, have to clear error flag.
	  T.push_back( Vector(x, y, z) );
	}
	else if(ele_id == "vn")
	{
	  no_normal = false;
	  is >> x >> y >> z;
	  if(! is.good())                      // in case it is -1#IND00
	  {
	    x = y = z = 0.0;
	    is.clear();
	    skip_line(is);
	  }
	  N.push_back( Vector(x, y, z) );
	}
	else if(ele_id == "f")
	{
	  int vi[10];                               // vertex indices.
	  int ni[10] = { -1, -1, -1, -1, };         // normal indices.
	  int ti[10] = { -1, -1, -1, -1, };         // tex indices.
	  int i = 0;
	  for(char c; i<10; ++i)
	  {
	    if(no_tex && no_normal) is >> vi[i];
	    else if(no_tex)         is >> vi[i] >> c >> c >> ni[i];
	    else if(no_normal)      is >> vi[i] >> c >> ti[i];
	    else                    is >> vi[i] >> c >> ti[i] >> c >>  ni[i];

	    if( ! is.good() )	    break;
	  }
	  for(int k=0; k<=i-3; k++)
	  {
	    F.push_back( TriangleFace(vi[0], vi[k+1], vi[k+2], ti[0], ti[k+1], ti[k+2], ni[0], ni[k+1], ni[k+2]) );
	  }
	  is.clear();
	}
	else 
	  skip_line(is);
      }

      has_normal = (N.size() > 1);
      total_triangles = F.size();
      is.close();
      cout << "total vertices " << V.size() - 1 << endl;
      cout << "total normals " << N.size() - 1 << endl;
      cout << "total triangles " << F.size() << endl;
    }
    else 
      cerr << "Can't open input file " << obj_fname << endl;

    bbox = BBox(*this);
    cout << "bounding box is " << bbox << endl;
  }

  void TriangleMesh :: TriangulateSphere(int depth)
  {
    const float X = .525731112119133606;
    const float Z = .850650808352039932;
    float vdata[12][3] = {    
      {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
      {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
      {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
    };
    int tindices[20][3] = { 
      {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
      {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
      {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
      {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

    V.push_back(Vertex());
    for(int i=0; i<12; i++)
    {
      V.push_back( Vertex(vdata[i][0], vdata[i][1], vdata[i][2]) );
    }
  
    for(int i=0; i<20; i++)
    {
      F.push_back( TriangleFace(tindices[i][0]+1, tindices[i][1]+1, tindices[i][2]+1,  tindices[i][0]+1, tindices[i][1]+1, tindices[i][2]+1) );
    }
    for(int d=1; d<depth; d++)  
    {
      int faces = F.size();
      for(int f=0; f < faces; ++f)
      {
	int i = F[0].i, j = F[0].j, k = F[0].k;
	
	Vertex v1 = V[i];
	Vertex v2 = V[j];
	Vertex v3 = V[k];

	Vertex v12 = ((v1 + v2) / 2).GetUnit();
	Vertex v13 = ((v1 + v3) / 2).GetUnit();
	Vertex v23 = ((v3 + v2) / 2).GetUnit();

	int ij = V.size();
	V.push_back(v12);
	
	int ik = V.size();
	V.push_back(v13);
	
	int jk = V.size();
	V.push_back(v23);
	
	F.erase( F.begin() );
	
	F.push_back( TriangleFace(i, ik, ij, i, ik, ij) );
	F.push_back( TriangleFace(j, jk, ij, j, jk, ij) );
	F.push_back( TriangleFace(k, ik, jk, k, ik, jk) );
	F.push_back( TriangleFace(jk, ik,ij, jk, ik,ij) );
      }
    }
    N = V;
    
    has_normal = (N.size() > 1);
    total_triangles = F.size();
    bbox = BBox(*this);
    cout << (*this) << endl;
  }
  


};




