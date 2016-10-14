#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "point3d.h"
#include <vector>
#include <iostream>

using namespace std;

struct Vertex {
 Point3D pt;
 vector<int> adj_list;


 Vertex() : pt(), adj_list() {}
 Vertex(const Point3D & np) : pt(np) {}

 //Point-related Functions
 double get_x() {
  return pt.x;
 }

 double get_y() {
  return pt.y;
 }
 
 double get_z() {
  return pt.z;
 }

 //Edge-related Function
 void add_endPt(int n) {
  adj_list.push_back(n);

 }

 int get_endPt(int index) {
   return adj_list[index];
 }
 
 bool is_adjacent(int n) {
  for(int i = 0; i < adj_list.size(); ++i)
   if(adj_list[i] == n)
    return true;
  return false;
 }
 


 void print() {
  cout << " === Point(" << pt.x << ", " << pt.y << ", " << pt.z << ") === " << endl;
  cout << "==== Adjacency List ====" << endl;
  for(int i = 0; i < adj_list.size(); ++i)
   cout << adj_list[i] << ' ';
  cout << endl << endl;
 } 
};

#endif
