/**
 *\file		main.cpp
 *
 *\brief	
 *
 *\author	Xianming Chen
 *
 */

#include "Canvas.h"
#include "color.h"
#include "xmath.h"
#include "TriangleMesh.h"

#include <iostream>
#include <sstream>
#include <iomanip>


using namespace columbia;

/* 
 * Given the material color, light color, and the normal and lighting direction, return the diffuse color.
 * If the normal == 0, then no light computation is done, and the material_col is simply returned.
 */
inline RGB compute_lighting(RGB const& material_col, RGB const& light_col, Vector const& light_dir, Vector* normal=0)
{
  if(normal)
  {
    float pro = normal->x * light_dir.x + normal->y * light_dir.y + normal->z * light_dir.z;
    if(pro < 0)
      return Black;

    return 
      (material_col * light_col) * 
      ( (pro / (sqrt(normal->x * normal->x + normal->y * normal->y + normal->z * normal->z) *
		sqrt(light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z) )) );
  }
  return material_col;
}


int main(int argc, char** argv) 
{
  bool draw_wireframe = false;
  if(argc > 2)
    draw_wireframe = (argv[2][0]=='w');
  float scale_factor = 0.15;
  if(argc > 3)
    scale_factor *= argv[3][0] - '0';
  
  try 
  {
    Canvas canvas;
    canvas.filename = "t.ppm";

    TriangleMesh msh(argv[1]);

    bool has_normal = msh.has_normal; 

    Vector translate = - msh.bbox.center;
    
    float scale_x = canvas.hReso / (msh.bbox.max.x - msh.bbox.min.x) ;
    float scale_y = canvas.vReso / (msh.bbox.max.y - msh.bbox.min.y) ;
    float scale = std :: min (scale_x, scale_y) * scale_factor;
    float z_range = msh.bbox.max.z - msh.bbox.min.z;
    
    RGB material_col = Firebrick;
    RGB light_col1 = White;
    RGB light_col2 = White * .0;
    
    Vector light_dir1(.2, 0, 1); //cos(pi/6), 0,sin(pi/6));
    //    Vector light_dir2(-1, 0, -.3); //cos(pi/6), 0,sin(pi/6));
    
    for(int f = 0; f < msh.total_triangles; ++f)
    {
      int i = msh.F[f].i, j = msh.F[f].j, k = msh.F[f].k;
      int ni = msh.F[f].ni, nj = msh.F[f].nj, nk = msh.F[f].nk;
      
      RGB col1, col2, col3;

      if(has_normal)
      {
	col1 = compute_lighting(material_col, light_col1, light_dir1, &msh.N[ni]);
	col2 = compute_lighting(material_col, light_col1, light_dir1, &msh.N[nj]);
	col3 = compute_lighting(material_col, light_col1, light_dir1, &msh.N[nk]);
      }
      else // depth cue
      {
	col1 = material_col * ( (msh.V[i].z - msh.bbox.min.z) / z_range );
	col2 = material_col * ( (msh.V[j].z - msh.bbox.min.z) / z_range );
	col3 = material_col * ( (msh.V[k].z - msh.bbox.min.z) / z_range );
      }
      if(draw_wireframe)
	canvas.WireTriangle( (msh.V[i] + translate) * scale, col1, 
			      (msh.V[j] + translate) * scale, col2, 
			      (msh.V[k] + translate) * scale, col3);
      else
      {
	canvas.SolidTriangle( (msh.V[i] + translate) * scale, col1, 
			      (msh.V[j] + translate) * scale, col2, 
			      (msh.V[k] + translate) * scale, col3);
      }
    }
    
    canvas.WritePPM();
  }
  catch(char const* msg) 
  { 
    std :: cerr << msg << std :: endl; 
  }
  
  
  return 0;
}
  
