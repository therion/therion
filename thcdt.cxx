/**
 * @file thcdt.cxx
 */
  
#include "thcdt.h"  
#include "thcdt/geom2d.h"
#include "thcdt/myMacro.h"
#include "thcdt/cdt.h"
#include "thcdt/cdt.cpp"

// vytvori sa zoznam vrcholov (kazdy vrchol bude mat ID
// predchadzajuceho a ID nasledujuceho) a shotov
// pri vrcholoch treba davat pozor na Xovu suradnicu
// aby vsetky boli odlisne

// spravime triangulaciu

// najdeme vsetky trojuholniky - najprv odprasime shoty,
// ktore lezia vonku
// potom zo shotov vytvorime trojuholniky (spravne orientovane)

bool operator < (const struct thcdtnode & n1, 
    const struct thcdtnode & n2) {
  return (n1.x < n2.x);
}

bool operator < (const struct thcdtleg & l1, 
    const struct thcdtleg & l2) {
  if (l1.from < l2.from)
    return true;
  if ((l1.from == l2.from) && (l1.to < l2.to))
    return true;
  return false;
}


void thcdtriangle::clear() {
  this->nnodes = 0;
  this->node_set.clear();
  this->leg_set.clear();
}


thcdtriangle::thcdtriangle() {
  this->clear();
}


long thcdtriangle::insert_node(double nx, double ny) {
  thcdtnode tmp;
  tmp.x = nx;
  tmp.y = ny;
  std::set<thcdtnode>::iterator it;
  it = this->node_set.find(tmp);
  while (it != this->node_set.end()) {
    tmp.x += 0.00001;
    it = this->node_set.find(tmp);
  }
  tmp.id = this->nnodes++;
  this->node_set.insert(tmp);
  return tmp.id;
}


void thcdtriangle::insert_leg(long from, long to) {
  long t;
  thcdtleg tmp;
  if (from > to) {
   t = from;
   from = to;
   to = t;
  }
  tmp.from = from;
  tmp.to = to;
  tmp.constrain = true;
  this->leg_set.insert(tmp);
}


void thcdtriangle::triangulate() {
  Mesh mesh;
  int n, tmp;

  // IMPORT SECTION
  mesh.v_num = this->node_set.size();
  mesh.v_array = new VPointer [mesh.v_num];
  std::set<thcdtnode>::iterator nit;
  bool firstnode = true;
  for(n = 0, nit = this->node_set.begin(); nit != this->node_set.end(); nit++, n++) {
    if (firstnode) {
      mesh.mins.x = mesh.maxs.x = nit->x;
      mesh.mins.y = mesh.maxs.y = nit->y;
      firstnode = false;
    } else {
      mesh.maxs.x=MAX(mesh.maxs.x,nit->x);
      mesh.maxs.y=MAX(mesh.maxs.y,nit->y);
      mesh.mins.x=MIN(mesh.mins.x,nit->x);
      mesh.mins.y=MIN(mesh.mins.y,nit->y);
    }
    mesh.v_array[n] = new Vertex(nit->x, nit->y, nit->id);  
  }
  
  mesh.sortVertices();
  
  int * reorder = new int [mesh.v_num],
      * backorder = new int [mesh.v_num];
  for (int n = 0; n < mesh.v_num; n++) {
    reorder[mesh.v_array[n]->getIndex()] = n;
    backorder[n] = mesh.v_array[n]->getIndex();
  }
  for (int n = 0; n < mesh.v_num; n++) {
    mesh.v_array[n]->setIndex(n);
  }
  
  //std::set<thcdtleg>::iterator lit;
  //for(lit = this->leg_set.begin(); lit != this->leg_set.end(); lit++) {
  //  mesh.addEdge(mesh.v_array[reorder[lit->from]], mesh.v_array[reorder[lit->to]], CONSTRAINED);
  //}
  
  // TRIANGULATION IT SELF
  // mesh.export2OFFFile("core","core");  
  mesh.buildCDT();
  
  // EXPORT SECTION
  EListPointer el;
  el = mesh.e_head;
  thcdtleg tmpleg;
  while (el != NULL) {
    tmpleg.from = backorder[el->e->he[0]->getBackVertex()->getIndex()];
    tmpleg.to = backorder[el->e->he[0]->getFrontVertex()->getIndex()];
    tmpleg.constrain = false;
    if (tmpleg.from > tmpleg.to) {
      tmp = tmpleg.to;
      tmpleg.to = tmpleg.from;
      tmpleg.from = tmp;
    }
    this->leg_set.insert(tmpleg);
    el=el->next;
  }

  for(n = 0; n < mesh.v_num; n++) {
    delete mesh.v_array[n];
  }  
  delete [] mesh.v_array;
  delete [] backorder;  
  delete [] reorder;  
  
}

void thcdttri::sort() {
  long tmp;
#define thcdttrisort(x,y) {tmp = x; x = y; y = tmp;}
  if ((this->n2 < this->n1) && (this->n2 < this->n3)) 
    thcdttrisort(this->n1,this->n2)
  else if ((this->n3 < this->n1) && (this->n3 < this->n2)) 
    thcdttrisort(this->n1,this->n3)
  if (this->n2 > this->n3)  
    thcdttrisort(this->n2,this->n3)
}


bool operator < (const struct thcdttri & t1, 
    const struct thcdttri & t2) {
  if (t1.n1 < t2.n1)
    return true;
  if ((t1.n1 == t2.n1) && (t1.n2 < t2.n2))
    return true;
  if ((t1.n1 == t2.n1) && (t1.n2 == t2.n2) && (t1.n3 < t2.n3))
    return true;
  return false;
}

thcdtpolygon::thcdtpolygon() {
  this->clear();
}


void thcdtpolygon::clear() {
  this->triangles.clear();
  this->cdt.clear();
  this->first_id = -1;
  this->last_id = -1;
}


void thcdtpolygon::end_contour() {
  if (this->last_id > -1) {
    this->cdt.insert_leg(this->last_id, this->first_id);
  }
  this->last_id = -1;
  this->first_id = -1;
}


long thcdtpolygon::insert_vertex(double vx, double vy) {
  long new_id = this->cdt.insert_node(vx, vy);
  if (this->last_id < 0) {
    this->first_id = new_id;
    this->last_id = new_id;
  } else {
    this->cdt.insert_leg(this->last_id, new_id);
    this->last_id = new_id;
  }
  return new_id;
}


void thcdtpolygon::triangulate() {
  this->cdt.triangulate();
  this->triangles.clear();
  thcdtleg tmpleg;
  thcdttri tmptri;
  std::set<thcdtleg>::iterator cl, nl, tl;
  cl = this->cdt.leg_set.begin(); 
  while (cl != this->cdt.leg_set.end()) {
    tl = cl;
    tl++;
    nl = tl;
    while ((nl != this->cdt.leg_set.end()) && (nl->from == cl->from)) {
      tmpleg.from = cl->to;
      tmpleg.to = nl->to;
      tl = this->cdt.leg_set.find(tmpleg);
      if (tl != this->cdt.leg_set.end()) {
        tmptri.n1 = cl->from;
        tmptri.n2 = cl->to;
        tmptri.n3 = nl->to;
        tmptri.sort();
        this->triangles.insert(tmptri);
      }
      nl++;
    }
    cl++;
  }
}


thcdtpolygon thcdtpoly;


