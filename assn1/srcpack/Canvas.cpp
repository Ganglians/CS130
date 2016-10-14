#include "Canvas.h"
#include "color.h"
#include "xmath.h"
#include <iostream>
#include <fstream>
#include <vector>


using namespace std;

namespace columbia
{

  void Canvas :: init() 
  { 
    int N = hReso*vReso;
    
    delete [] data; 
    data = new RGB [N]; 
    memset(data, 0, sizeof(RGB) * N);

    delete [] z_buffer; 
    z_buffer = new float [N];
    fill(z_buffer, z_buffer+N, -1.0E9);

    eye_at = hReso / 2;     // default horizontal field of view = 90.
  }

  void Canvas :: Clear(RGB const& color)
  {
    for (int row = 0; row < vReso; row++) 
      for (int col = 0; col < hReso; col++) 
      {
	(*this)[row][col] = color;
      }
  }
  

  void Canvas :: WritePPM(const char *_filename) const
  {
    Canvas* This =  (Canvas*)this;
    
    if(!_filename) 
      _filename = filename.c_str();

    ofstream file;
    file.open (_filename, ios::out | ios::binary);
    
    if(file) 
    {
      file << "P6\n";   // color rawbits format
      file << hReso << " " << vReso << endl << 100 << endl;
  
      unsigned char r, g, b;
      for (int row = 0; row < vReso; row++) 
	for (int col = 0; col < hReso; col++) 
	{
	  r = (unsigned char) (100.0*(*This)[vReso-row-1][col][0]);  
	  g = (unsigned char) (100.0*(*This)[vReso-row-1][col][1]);  
	  b = (unsigned char) (100.0*(*This)[vReso-row-1][col][2]);  

	  if(r>100.0 || g>100.0 || b>100.0)  throw "RGB color overflow\n"; 

	  file << r << g << b;
	}

      file << endl;
      file.close();
    }
    else 
      cerr << "Can't open output file " << _filename << endl;
  }



  void Canvas :: scanLineSegment(int x1, int y1, float z1, RGB const& col1, int x2, int y2, float z2, RGB const& col2, set<ScannedResult>* output)
  {
    assert( x2-x1 >= 1 && x2-x1 >= y2-y1 && y2-y1 >= 0);

    if(y1 == y2)                                     // this is horizontal line, or vertical line if swap_xy set.
    {
      for(int x = x1; x <= x2; ++x)                   
      {
	double t = ( double(x-x1) ) / (x2 - x1);
	writePixel(x, y1, interpolate(t, z1, z2), interpolate(t, col1, col2), output);
      }
    }
    else if(y2-y1 == x2-x1)                         // diagonal lines.
    {
      int y = y1;
      for(int x = x1; x <= x2; ++x, ++y)
      {
	double t = ( double(x-x1) ) / (x2 - x1);
	writePixel(x, y, interpolate(t, z1, z2), interpolate(t, col1, col2), output);
      }
    }
    else     
    {
      for(int x = x1; x <= x2; ++x)
      {
	double t = ( double(x-x1) ) / (x2 - x1);
	writePixel(x, round( interpolate(t, double(y1), double(y2)) ), interpolate(t, z1, z2), interpolate(t, col1, col2), output);
      }
    }
  }
  

  void Canvas :: ScanLineSegment(int x1, int y1, float z1, RGB const& col1, int x2, int y2, float z2, RGB const& col2, set<ScannedResult>* output)
  {
    if(x1==x2 && y1==y2) return;

    RGB c1(col1), c2(col2);
    
    if(x1 > x2)                                       // always scan convert from left to right.
    {
      swap(x1, x2); swap(y1, y2); swap(z1, z2);
      swap(c1,  c2);
    }
    if( (flip_y = y1 > y2) )                          // always scan convert from down to up.
    {
      y1 = -y1, y2 = -y2;
    }
    if( (swap_xy = y2-y1 > x2-x1) )                   // and always scan convert a line with <= 45 deg to x-axis.
    {
      swap(x1, y1), swap(x2, y2);
    }
    scanLineSegment(x1, y1, z1, c1, x2, y2, z2, c2, output);
  }


  void Canvas :: WireTriangle(Point const& P1, RGB const& col1, Point const& P2, RGB const& col2, Point const& P3, RGB const& col3)
  {
    Point p1 = perspectiveProjection(P1);
    Point p2 = perspectiveProjection(P2);
    Point p3 = perspectiveProjection(P3);
    ScanLineSegment(round(p1.x), round(p1.y), p1.z, col1, round(p2.x), round(p2.y), p2.z, col2);
    ScanLineSegment(round(p1.x), round(p1.y), p1.z, col1, round(p3.x), round(p3.y), p2.z, col3);
    ScanLineSegment(round(p3.x), round(p3.y), p1.z, col3, round(p2.x), round(p2.y), p2.z, col2);
  }
  
  void Canvas :: SolidTriangle(Point const& P1, RGB const& col1, Point const& P2, RGB const& col2, Point const& P3, RGB const& col3)
  {
    Point p1 = perspectiveProjection(P1);
    Point p2 = perspectiveProjection(P2);
    Point p3 = perspectiveProjection(P3);

    set<ScannedResult> scanned_pnts;     // ele is <y, <x, col> >, sorted by y val.

    //    cout << "P1 P2 P3 "  << P1 << "\t\t" << P2 << "\t\t" << P3 << endl;
    
    //    cout << "scan line 12\n\n\n";
    ScanLineSegment(round(p1.x), round(p1.y), p1.z, col1, round(p2.x), round(p2.y), p2.z, col2, &scanned_pnts);
    //    cout << "scan line 13\n\n\n";
    ScanLineSegment(round(p1.x), round(p1.y), p1.z, col1, round(p3.x), round(p3.y), p3.z, col3, &scanned_pnts);
    //    cout << "scan line 32\n\n\n";
    ScanLineSegment(round(p3.x), round(p3.y), p3.z, col3, round(p2.x), round(p2.y), p2.z, col2, &scanned_pnts);
    
    int cur_yval = vReso / 2 + 1;                    // initialize to an invalid value.
    set<ScannedResult> same_yval;                    // of the scanned result.
    
    for (set<ScannedResult>::iterator it = scanned_pnts.begin(); it != scanned_pnts.end(); ++it)
    {
      int y = it->y;
      if(y != cur_yval) 
      {
	if(same_yval.size())
	{
	  set<ScannedResult>::iterator it1 = same_yval.begin(), it2 = --same_yval.end();
 	  ScanLineSegment(it1->x, cur_yval, it1->z, it1->col,
			  it2->x, cur_yval, it2->z, it2->col);
	  same_yval.clear();
	}
	cur_yval = y;
      }
      same_yval.insert(*it);
    }
  }
}

