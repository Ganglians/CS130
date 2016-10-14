#include <iostream>
#include "vertex.h"
#include "point2d.h"

using namespace std;

int main() {
 Point2D tmp(10, 10);
 Vertex test(tmp);

 test.add_endPt(10);
 test.add_endPt(100);
 //test.print();
 cout << test.get_endPt(0);
 return 0;
}
