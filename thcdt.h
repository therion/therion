/** 
 * @file thcdt.h
 * CDT calculation class.
 */
  
 
#ifndef thcdt_h
#define thcdt_h

#include <set>

/**
 * CDT class.
 */ 
 
struct thcdtnode {
  double x, y;
  long id;
};

bool operator < (const struct thcdtnode & n1, 
    const struct thcdtnode & n2);

struct thcdtleg {
  long from, to;
  bool constrain;
};

bool operator < (const struct thcdtleg & l1, 
    const struct thcdtleg & l2);

struct thcdtriangle {
  std::set<thcdtnode> node_set;
  std::set<thcdtleg> leg_set;
  long nnodes;
  thcdtriangle();
  void clear();
  long insert_node(double nx, double ny);
  void insert_leg(long from, long to);
  void triangulate();
};


struct thcdttri {
  long n1, n2, n3;
  void sort();
};


bool operator < (const struct thcdttri & t1, 
    const struct thcdttri & t2);

struct thcdtpolygon {
  thcdtriangle cdt;
  std::set <thcdttri> triangles;
  thcdtpolygon();
  long first_id, last_id;
  void clear();
  void end_contour();
  long insert_vertex(double vx, double vy);
  void triangulate();
};


extern thcdtpolygon thcdtpoly;

#endif


