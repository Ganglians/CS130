// Name: Steven Em
// Quarter, Year: Fall, 2013
// Assignment 01
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "point3d.h"
#include "point2d.h"
#include "vertex.h"

using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int HALF_MESH_LENGTH = 50;

//Unable to use fraction so converted to decimals
const double SRC_VERTEX_RATIO = 0.375;
const double ADJ_VERTEX_RATIO = 0.125;

int winID;
int scale_f = 5;
int subdiv_lvl = 0;
bool is_front_view = false, is_side_view = false, is_top_view = false;
char* meshfile;
vector<Point3D> mesh_face;
vector<Vertex> mesh_vertex;
vector<int> original_vertex;

// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
 glPointSize(1.0);
 glColor3f(1.0,1.0,1.0);
 glBegin(GL_POINTS);
 glVertex2f(x, y);
 glEnd();
}

void swap(Point2D &p0, Point2D &p1) {
 Point2D tmp = p0;
 p0 = p1;
 p1 = tmp;
}

void scale(Point2D &p0, Point2D &p1) {
 p0 *= scale_f;
 p1 *= scale_f;
}

void translate(Point2D &p0, Point2D &p1, int dx, int dy) {
 Point2D tmp(dx, dy);
 p0 += tmp;
 p1 += tmp;
}

void renderLine(Point2D start, Point2D end) {
 int dShiftX = (WINDOW_WIDTH/2) - (scale_f * HALF_MESH_LENGTH);
 int dShiftY = (WINDOW_HEIGHT/2) - (scale_f * (HALF_MESH_LENGTH));
 scale(start, end);
 translate(start, end, dShiftX, dShiftY);
 
 if(start.x > end.x)
  swap(start, end);

 double m = (double)(end.y - start.y)/(end.x - start.x);
 double y = (double)start.y;
 double x = (double)start.x;
 if(m >= -1 && m < 1) {
  while(x <= end.x) {
   renderPixel(x, floor(y));
   y += m;
   x++;
  }
 } else {
  if(y > end.y)
   swap(start, end);
 
  double m1 = 1/m;
  y = start.y;
  x = start.x;
  while(y <= end.y) {
   renderPixel(floor(x), y);
   y++;
   x += m1;
  }
 }
}

//Connect all 3 pts to create the face
void renderFace(Point2D p0, Point2D p1, Point2D p2) {
 renderLine(p0, p1);
 renderLine(p1, p2);
 renderLine(p2, p0);
}

//TODO FIX VIEWING. What to display when clicking for subdivision and in beginning with no clicks
//Need to hold view constant when doing subdivision and redisplaying
//Simply renders the mesh onto the OpenGL window
void renderMesh(){
 int p0, p1, p2;
 Point2D v0, v1, v2;
 for(int i = 0; i < mesh_face.size(); ++i) {
  //Extract the 3 pts for the face
  p0 = mesh_face[i].x;
  p1 = mesh_face[i].y;
  p2 = mesh_face[i].z;

  //Depending on which view is selected we ignore one of
  //the axises and then create 2D points for the window
  if(is_front_view) {
   if(i == 0)
    cout << "-- Front View --" << endl;
   //We ignore the z-axis
   v0 = Point2D(mesh_vertex[p0].get_x(), mesh_vertex[p0].get_y());
   v1 = Point2D(mesh_vertex[p1].get_x(), mesh_vertex[p1].get_y());
   v2 = Point2D(mesh_vertex[p2].get_x(), mesh_vertex[p2].get_y());
  } else if(is_side_view) {
   //We ignore the x-axis    
   if(i == 0)
    cout << "-- Side View --" << endl;
   v0 = Point2D(mesh_vertex[p0].get_z(), mesh_vertex[p0].get_y());
   v1 = Point2D(mesh_vertex[p1].get_z(), mesh_vertex[p1].get_y());
   v2 = Point2D(mesh_vertex[p2].get_z(), mesh_vertex[p2].get_y());
  } else {
   //We ignore the y-axis
   if(i == 0)
    cout << "-- Top View --" << endl;
   v0 = Point2D(mesh_vertex[p0].get_x(), mesh_vertex[p0].get_z());
   v1 = Point2D(mesh_vertex[p1].get_x(), mesh_vertex[p1].get_z());
   v2 = Point2D(mesh_vertex[p2].get_x(), mesh_vertex[p2].get_z());
  }

  //scale_f
  renderFace(v0, v1, v2);
 }

 //Set the bool conditions back to false
 is_side_view = is_front_view = is_top_view = false;
}

void GL_render() {
 glClear(GL_COLOR_BUFFER_BIT);
 renderMesh();
 glutSwapBuffers();
}

//Reads the command line input file in order to
//initalize the data storage structures (mesh_vertex & mesh_face)
void readMesh(char* filename) {
 meshfile = filename;
 ifstream mesh_file(filename);
 if(mesh_file.is_open()) {
  Vertex tmpV;
  Point3D tmpP;
  int num_vertices, num_faces, p0, p1, p2;
  mesh_file >> num_vertices >> num_faces;
  mesh_vertex.clear();
  mesh_face.clear();
 
  //Populate mesh_vertex with vertices given from the mesh
  for(int i = 0; i < num_vertices; ++i) {
   mesh_file >> p0 >> p1 >> p2;
   tmpP = Point3D(p0, p1, p2);
   tmpV = Vertex(tmpP);
   mesh_vertex.push_back(tmpV);
  }
 
  //Populate mesh_vertex's adj_list with adjacent vertices &
  //mesh_face with points used to create a face (index # for mesh_vertex)
  for(int i = 0; i < num_faces; ++i) {
   mesh_file >> p0 >> p1 >> p2;
   
   tmpP = Point3D(p0, p1, p2);
   mesh_face.push_back(tmpP);
   
   if(!mesh_vertex[p0].is_adjacent(p1)) {
    mesh_vertex[p0].add_endPt(p1);
    mesh_vertex[p1].add_endPt(p0);
   }
   if(!mesh_vertex[p0].is_adjacent(p2)) {
    mesh_vertex[p0].add_endPt(p2);
    mesh_vertex[p2].add_endPt(p0);
   }
   if(!mesh_vertex[p1].is_adjacent(p2)) {
    mesh_vertex[p1].add_endPt(p2);
    mesh_vertex[p2].add_endPt(p1);
   }
  }
 }  
}

bool is_equal(Point3D l, Point3D r) {
 return ((l.x == r.x) && (l.y == r.y) && (l.z == r.z));
}

int find_index(vector<Vertex> v, Point3D target) {
 for(int i = 0; i < v.size(); ++i)
  if(is_equal(v[i].pt, target)) return i;	 
 return -1;
}

bool is_dub_edge(int p0, int p1) {
 if(p0 > p1) {
  vector<int> adj_list = mesh_vertex[p1].adj_list;	 
  for(int i = 0; i < adj_list.size(); ++i) {
   if(adj_list[i] == p0) return true;
  }
 }
 return false;	
}	

Point3D sum_adj(int p0, int p1) {
 Point3D sum;	
 vector<Point3D> adj_pts;
 vector<int> adj_list_p0(mesh_vertex[p0].adj_list);
 vector<int> adj_list_p1(mesh_vertex[p1].adj_list);		

 //Determines if a point is adjacent to both vertices
 //If true then that point will be added to the vector adj_pts
 for(int i = 0; i < adj_list_p0.size(); ++i) {
  int adj_pt = adj_list_p0[i];
  if(mesh_vertex[p1].is_adjacent(adj_pt)) 
   adj_pts.push_back(mesh_vertex[adj_pt].pt);
 }

 //Determines the sum of all the adjacent points
 for(int i = 0; i < adj_pts.size(); ++i)
  sum += adj_pts[i];	

 return sum;
}

int findMidVertex(vector<Vertex> v, int start, int end) {
 vector<int> adj_list;
 for(int i = 0; i < v.size(); ++i) {
  if(v[i].adj_list.size() == 2) {
   adj_list = v[i].adj_list;
   if(adj_list[0] == start || adj_list[1] == start) 	   
    if(adj_list[0] == end || adj_list[1] == end) 
     return i;	    
  }
 }
 
 cout << "Error: Unable to find midpoint." << endl;
 return -1;
}

//Updates the index of the vertices from the original mesh
void updateSrcIndex(vector<Vertex> &v) {
 if(!original_vertex.empty())
  original_vertex.clear();

 for(int i = 0; i < mesh_vertex.size(); ++i) {
  Point3D origin_pt = mesh_vertex[i].pt;
  int new_index = find_index(v, origin_pt);
  original_vertex.push_back(new_index);
 }
}

//Creates new faces after the vertices have been subdivided
void newMeshFace(vector<Point3D> &f, vector<Vertex> &v) {
 updateSrcIndex(v);

 int pt0, pt1, pt2, pt01, pt02, pt12;
 for(int i = 0; i < mesh_face.size(); ++i) {
  pt0 = original_vertex[mesh_face[i].x];
  pt1 = original_vertex[mesh_face[i].y];
  pt2 = original_vertex[mesh_face[i].z];
  pt01 = findMidVertex(v ,pt0, pt1);
  pt02 = findMidVertex(v, pt0, pt2);
  pt12 = findMidVertex(v, pt1, pt2);

  //New faces
  f.push_back(Point3D(pt0, pt01, pt02));
  f.push_back(Point3D(pt1, pt01, pt12));
  f.push_back(Point3D(pt2, pt02, pt12));
  f.push_back(Point3D(pt01, pt02, pt12));
 }
}

void connectNewVertices(vector<Vertex> &v) {
 int adj_pt0, adj_pt1;
 for(int i = 0; i < v.size(); ++i) {
 if(v[i].adj_list.size() == 2) {
   adj_pt0 = v[i].adj_list[0];
   adj_pt1 = v[i].adj_list[1];
   for(int j = 0; j < v[adj_pt0].adj_list.size(); ++j) 
    if(v[adj_pt0].adj_list[j] != i)
     v[i].adj_list.push_back(v[adj_pt0].adj_list[j]);

   for(int k = 0; k < v[adj_pt1].adj_list.size(); ++k)
    if(v[adj_pt1].adj_list[k] != i)
     v[i].adj_list.push_back(v[adj_pt1].adj_list[k]);
  }   
 }
}

void subdivide_topology() {
 vector<Vertex> subdiv_vertex;
 int index0, index1, index2;
 for(int i = 0; i < mesh_vertex.size(); ++i) {
  vector<int> adj_list = mesh_vertex[i].adj_list;
  Point3D start_edge = mesh_vertex[i].pt; 

  for(int j = 0; j < adj_list.size(); ++j) {
   int adj_pt_index = adj_list[j];
   if(is_dub_edge(i, j))
    continue;

   Point3D end_edge = mesh_vertex[adj_pt_index].pt;
   Point3D adj_pts = sum_adj(i, adj_pt_index);
   Vertex subdPt(((start_edge + end_edge) * SRC_VERTEX_RATIO) 
	    + (adj_pts * ADJ_VERTEX_RATIO));
   
   if(subdiv_vertex.empty()) {
    index0 = 0, index1 = 1, index2 = 2;
    subdiv_vertex.push_back(start_edge);
    subdiv_vertex[index0].add_endPt(index1);

    subdiv_vertex.push_back(subdPt);
    subdiv_vertex[index1].add_endPt(index0);
    subdiv_vertex[index1].add_endPt(index2);

    subdiv_vertex.push_back(end_edge);
    subdiv_vertex[index2].add_endPt(index1);
   } else {
    index0 = find_index(subdiv_vertex, start_edge);
    index2 = find_index(subdiv_vertex, end_edge);
    subdiv_vertex.push_back(subdPt);
    index1 = subdiv_vertex.size()-1;

    if(index0 == -1) {
     subdiv_vertex.push_back(start_edge);	    
     index0 = subdiv_vertex.size()-1;
    }

    if(index2 == -1) {
     subdiv_vertex.push_back(end_edge);
     index2 = subdiv_vertex.size()-1;
    }   

    subdiv_vertex[index0].add_endPt(index1);
    subdiv_vertex[index2].add_endPt(index1);	
    subdiv_vertex[index1].add_endPt(index0);
    subdiv_vertex[index1].add_endPt(index2);
   } 
  }
 }
/*
 cout << "Size of Subdiv_Vertex:" << subdiv_vertex.size() << endl;
 for(int i = 0; i < subdiv_vertex.size(); ++i) {
  subdiv_vertex[i].print();
 }
*/
 
 vector<Point3D> subdiv_face;
 newMeshFace(subdiv_face, subdiv_vertex);
 connectNewVertices(subdiv_vertex);

 mesh_face = subdiv_face;
 mesh_vertex = subdiv_vertex; 
}

double getBeta(int adjacentVertices){
 const double ADJACENT3 = 0.1875; //3/16 = 0.1875
 if(adjacentVertices == 3)
  return ADJACENT3;	 
 else
  return 3 / (8 * adjacentVertices);
}

Point3D getAvgOfAdj(vector<int> &v) {
 int total = v.size();	
 Point3D sum;
 for(int i = 0; i < total; ++i)
  sum += mesh_vertex[v[i]].pt;
 return sum / total;
}

void subdivide_refinement() {
 Point3D adjustedPt;
 for(int i = 0; i < original_vertex.size(); ++i) {
  cout << "INDEX[" << i << "]";
  cout << original_vertex[i] << endl;

  int index = original_vertex[i];
  int num_adjacent = mesh_vertex[index].adj_list.size();
  Point3D original_pt = mesh_vertex[index].pt;
  Point3D avg_adjacent = getAvgOfAdj(mesh_vertex[index].adj_list);
  double beta = getBeta(num_adjacent);
  adjustedPt = (original_pt * (1 - (num_adjacent * beta))) + (avg_adjacent * beta);
  mesh_vertex[index].pt = adjustedPt;
  
  cout << "AdjustedPt--" << endl;
  adjustedPt.print();
  cout << endl;
 }	 
}

void loop_subdivision() {
 if(subdiv_lvl < 1) { //Change to 3
  subdivide_topology();
  subdivide_refinement();
  ++subdiv_lvl;
 } else {
  readMesh(meshfile);
  subdiv_lvl = 0;
 }	
 glutPostRedisplay();
}

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
 glutInit(argc, argv);
 glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
 glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

 winID = glutCreateWindow("CS 130 (Subdivision Assignment)- Steven Em");
 readMesh(argv[1]);

 //The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
 glMatrixMode(GL_PROJECTION_MATRIX);
 glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
 glutDisplayFunc(GL_render);
}

void print_mesh(vector<Point3D> v) {
 for(int i = 0; i < v.size(); ++i) 
  cout  << "Mesh[" << i << "]: " << v[i].x << " " << v[i].y << " " << v[i].z << endl;
}

//Determines which view to display the user based on which key is pressed
void view_mode(unsigned char key, int x, int y) {
 switch (key) {
  case 49: //ASCII number 1
   is_front_view = true;
   break;
  case 50: //ASCII number 2
   is_side_view = true;
   break;
  case 51: //ASCII number 3
   is_top_view = true;
   break;
  case 27: //Escape key
   glutDestroyWindow(winID);
   exit(0);
   break;
  default:
   break;
 }
 glutPostRedisplay();
}

//Implements loop subdivision on the mesh when the mouse is clicked
void processMouse(int button, int state, int x, int y) {
 if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
  //Loop Subdivision
  loop_subdivision();
 }
}

int main(int argc, char** argv)
{	
 //Execute OpenGL graphics window
 GLInit(&argc, argv);
 glutKeyboardFunc(view_mode);
 glutMouseFunc(processMouse);
 glutMainLoop();

 return 0;
}


