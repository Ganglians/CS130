/**
 *\file		Canvas.h
 *
 *\brief	The Canvas class is a discrete simulation of the real life canvas. 
 *
 *              The canvas is cutted into grid structure, with <b><var>vReso</var></b> grids along
 *              vertical (<b><var>Row</var></b>) direction and <b><var>hReso</var></b> horizontal grids along 
 *              horizontal(<b><var>Col</var></b>) direction. Each rectangular grid is called a pixel, or picture
 *              element, and is specified by its lower-left <b><var>(row, col)</var></b> position. 
 *              The coordinate system has the lower-left corner of the canvas as its origin, 
 *              with <b><var>Row</var></b> going up and <b><var>Col</var></b> going right. The lower-left corner 
 *              pixel has a coordinate of <b><var>(0,0)</var></b>, and the upper-right one of <b><var>
 *              (hReso - 1, vReso - 1)</var></b>. 
 *
 *              A <b><var>(x, y)</var></b> pair means <b><var>col = x+hReso/2, and row = y+vReso/2</var></b>.
 *
 *              To each piexel <b><var>(row, col)</var></b> on the Canvas, a RGB color value is associated,
 *              which can be accessed via index operator [<b><var>row</var></b>][<b><var>col</var></b>].
 *
 *              WritePPM() writes the Canvas to an image file of PPM format.
 *
 * <H4>11 Jul 2004:</H4> Point structure and perspectiveProjection(Point const&) added. Now the Canvas is actually a virutal camera
 *              in the simplest setting: the Canvas plane is centered at z=0 with x-size of hReso, and y-size of vReso, 
 *              Eye at eye_at (>0) and looking along positive z direction.  eye_at defaults to hReso / 2.
 *
 *              Now the triangle primitives are updated so the given points are in 3D space. The other primitives are
 *              not updated though, and actually can be deleted from Canvas class, since for 3D graphics, everything 
 *              is usually tesselated by triangles.
 *
 *\author	Xianming Chen
 *
 */


#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "RGB.h"
#include "xmath.h"
#include "Point.h"
#include "color.h"

#include <string>
#include <cassert>
#include <set>

using namespace std;

namespace columbia
{
  
  struct ScannedResult
  {
      ScannedResult(int X, int Y, float Z, RGB const& Col) : x(X), y(Y), z(Z), col(Col) { }
      bool operator<(ScannedResult const& rhs) const { return (y<rhs.y) || ( (y==rhs.y) && (x<rhs.x) ); }
      int x, y;
      float z;
      RGB col;
  };
  
  class Canvas
  { 
  public:
      explicit Canvas(int resoH = 512, int resoV = 512) : hReso(resoH), vReso(resoV), filename("tmp.ppm"), data(0), z_buffer(0)  { init(); }
      ~Canvas() { delete [] data; delete [] z_buffer; }

      void Clear(RGB const&);
      
      RGB* operator[] (int r)                    { assert(r>=0 && r<vReso); return data + r*hReso; }
      RGB& Pixel(int row, int col);              //! Use this instead of the [] operator if both row and col number checking is needed.
      float& Z(int row, int col);    

      /**
       * Scan-convert a line segment given two end points. The scan conversion stop right before the second end point.
       * If output = 0, Draw a line segment between two end points, (x1,y1) and (x2,y2).  Each pixel color is interpolated from two end colors.
       * Else only return scanned result to output.
       *
       * Also interpolate between given z-valus, and write to canvas only after passing z-buffer algorithm.
       */
      void ScanLineSegment(int x1, int y1, float z1, RGB const& col1, int x2, int y2, float z2, RGB const& col2, set<ScannedResult>* output=0);
      

      void WireTriangle(Point const& P1, RGB const& col1, Point const& P2, RGB const& col2, Point const& P3, RGB const& col3);
      void SolidTriangle(Point const& P1, RGB const& col1, Point const& P2, RGB const& col2, Point const& P3, RGB const& col3);
      
      void WritePPM(const char* = 0) const;

      int hReso, vReso;                          //! Resolutions (pixel #)at horizontal and vertical directions.
      std::string filename;                      //! write as PPM image file name.


  private:
      RGB* data;
      float* z_buffer;                           // z-buffer value.
      
      bool swap_xy, flip_y;                      // swap x&y if abs(slope) > 1.0, flip y if y<0.

      void init();

      void scanLineSegment(int x1, int y1, float z1, RGB const& col1, int x2, int y2, float z2, RGB const& col2, set<ScannedResult>* output=0);
      
      void writePixel(int X, int Y, float cur_z, RGB const& col, set<ScannedResult>* output); 

      double eye_at;
      Point perspectiveProjection(Point const&); 
  };
  
  inline void Canvas :: writePixel(int X, int Y, float cur_z, RGB const& col, set<ScannedResult>* output)
  {
    if(swap_xy) swap(X,Y);
    if(flip_y)  Y = -Y; 

    if(output)
    {
      output->insert( ScannedResult(X, Y, cur_z, col) );
    }
    else if( (X >= -hReso/2 && X < hReso/2) &&
	     (Y >= -vReso/2 && Y < vReso/2) )
    {
      if( cur_z > Z(Y + vReso/2, X + hReso/2) )
      {
	if(col == Red)
	{
// 	  cout << "old z " << Z(Y + vReso/2, X + hReso/2) << endl;
// 	  cout << "cur z " << cur_z << endl;
// 	  cout << "old color " << Pixel(Y + vReso/2, X + hReso/2) << " replaced\n";
	}

	Z(Y + vReso/2, X + hReso/2) = cur_z;
	Pixel(Y + vReso/2, X + hReso/2) = col;
      }
      else if(col == Red)
      {
// 	cout << "old z " << Z(Y + vReso/2, X + hReso/2) << endl;
// 	cout << "cur z " << cur_z << endl;
// 	cout << "old color " << Pixel(Y + vReso/2, X + hReso/2) << " no replaced\n";
      }
      
    }
  }
  

  inline RGB& Canvas :: Pixel(int row, int col)
  {
    if(row >= vReso) { cout << "row = " << row << endl; throw "row number overflow\n"; }    
    if(row < 0)      { cout << "1row = " << row << endl; throw "row number underflow\n"; }
    if(col >= hReso) { cout << "col = " << col << endl; throw "col number overrflow\n"; }
    if(col < 0)      { cout << "col = " << col << endl; throw "col number underflow\n"; }
    return *(data + row * hReso + col);
  }
  
  inline float& Canvas :: Z(int row, int col)
  {
    if(row >= vReso) { cout << "row = " << row << endl; throw "row number overflow\n"; }    
    if(row < 0)      { cout << "1row = " << row << endl; throw "row number underflow\n"; }
    if(col >= hReso) { cout << "col = " << col << endl; throw "col number overrflow\n"; }
    if(col < 0)      { cout << "col = " << col << endl; throw "col number underflow\n"; }
    return *(z_buffer + row * hReso + col);
  }

  inline Point Canvas :: perspectiveProjection(Point const& P)
  {
    double scale = eye_at / (eye_at - P.z);
    return Point(P.x * scale, P.y * scale, P.z);
  }


}

#endif
