// file name: cdt.h
//	including all the data structure used in 
//	constrained delaunay triangultaion(CDT).
//
// author: Zhihua Wang (zhihua@cs.nyu.edu)
// date: Dec 12, 2002
/*************************************************************/

//#define	OUTPUT
//#define	DEBUG
//#define	DETECT_COLLINEAR_DEGENERACY
//#define	COMMENT_ON

#define	NOT_SUPPORT_SAME_X
#undef	NOT_SUPPORT_SAME_X
// *************************************************************
//	predefinitions
// *************************************************************
#include	<math.h>

/* THERION
#include	"geom2d.h"
#include	"myMacro.h"
THERION */

// *************************************************************
//	io utility
// *************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define	BLANK	' '
#define	TAB	'\t'

#define	noff_keyword "NOFF"

#define	LINE_LENGTH	80
#define	NO_ERR	1
#define	ERR_IMPORT_FILE	-1
#define	ERR_CANNOT_OPEN_FILE -2
#define	ERR_NOT_VALID_FORMAT -3
#define	ERR_WRONG_VERTEX_NUMBER -4
#define	ERR_WRONG_FACE_NUMBER -4


void	reportString(char* str)
{
	fprintf(stderr,"%s\n",str);
}

void	reportErrorString(char*	str)
{
	fprintf(stderr,"Error: %s\n",str);
	exit(-1);
}

void	reportError(char*	str,int line,char* file)
{
	fprintf(stderr,"Error: [%s:%d] %s\n",file,line,str);
	exit(-1);
}

void	reportWarningString(char* str)
{
	fprintf(stderr,"Warning: %s\n",str);
}

void	reportWarning(char* str,int line,char* file)
{
	fprintf(stderr,"Warning: [%s:%d] %s\n",file,line,str);
}

// read one line of file fp points to from current point
void	readToLineEnd(FILE * fp)
{
	int	c;
	c=fgetc(fp);
	while	(c!='\n')
		c=fgetc(fp);
}

int	readFirstNonspaceChar(FILE* fp)
{
	int	c;
	c=fgetc(fp);
	while	(c==BLANK || c==TAB)
		c=fgetc(fp);
	return c;
}

// omit the comment in fp
// in case the first character of line is '#'
void	omitComment(FILE* fp,char Leading_Comment_Char='#')
{
/*

	int c;
	//omit comment
	c=readFirstNonspaceChar(fp);

#ifdef	DEBUG
	printf("First Charater:%c\n",c);
#endif//	DEBUG
	
	// omit comment line
	while	(c==Leading_Comment_Char)
	{
		readToLineEnd(fp);
		c=readFirstNonspaceChar(fp);
#ifdef	DEBUG
	printf("First Charater:%c\n",c);
#endif//	DEBUG
	}
	fseek(fp,-1L,SEEK_CUR);	//put back one character
*/	
}

// *************************************************************
//	primitives
// 	data structure
// *************************************************************
#define	NON_CONSTRANED	-1	// type value of non constrained edge (see below)
#define	CONSTRAINED		1
#define	VIRTUAL			0

#define SEPOFFSET	1	// some value >0 to make offset
#define MAX_Y	HUGE_VAL
#define MIN_Y	-HUGE_VAL

class	Mesh;
class	Vertex;
class	Halfedge;
class	Edge;
class	EdgeList;
class	Slab;
class	Quad;
struct	Window;

typedef	Mesh*	MeshPointer;
typedef	Vertex*	VPointer;
typedef	Halfedge*	HePointer;
typedef	Edge*	EPointer;
typedef	EdgeList*	EListPointer;
typedef	Slab*	SPointer;
typedef	Quad*	QPointer;
typedef	Window* WPointer;

// planar graph
class	Mesh{
	HePointer	getHullHalfedge(VPointer v,int sep_index,bool lowside);
	HePointer	getHullHalfedgeNextTo(HePointer _he,VPointer v,int l,int sep_index,int r,bool lowside);
	void	findInitialCandidatePair(WPointer w,
		VPointer &p0,VPointer &q0,
		HePointer &p_he,HePointer &q_he,
		bool	lowside);
	EPointer	advancePairToBridge(WPointer w,VPointer p0,VPointer q0,HePointer p_he,HePointer q_he,bool lowside);
public:
	int v_num;	//the number of vertices read from input
	VPointer* v_array;	// the array pointing to the vertices
	double*	sep_array;	// x-value of vertical separate lines

	int e_num;	// numver of edges
	int ce_num;	// number of constrained edges read from input
	EListPointer	e_head,e_tail;

	//bounding box (mins.x, maxs.x) * (mins.y maxs,y)
	Point	maxs;	
	Point	mins;
	//bounding utility
	VPointer vlb,vlt;	//left most, btm and top vertex added manually
	VPointer vrb,vrt;
	EPointer sky_e;
	EPointer eth_e;
	//slab head
	SPointer	s_head;
//	SPointer	s_tail;

	Mesh();
	EPointer addEdge(VPointer v1,VPointer v2,int type=NON_CONSTRANED);
	void	deleteEdge(EPointer e);

	int initSlabs();
	int	mergeSlabs();
	void mergeSlabPair(SPointer	ls);
	void sortVertices();
	int import2OFFFile(char* sf);
	int	export2OFFFile(char* sf,char* f=NULL);
	EPointer findBridge(WPointer w,bool lowside);
	EPointer findNextBaseEdge(WPointer w,EPointer e);
	void buildCDTOfWindow(WPointer w);
	int	buildCDT();
// THERION
//	void	display();

	MeshPointer copy();
};

//vertex	
class Vertex{
private:
////members
	int	valence;
	int	index;	// index in x coordinate order
////functions
public:
////members
	////topology
	HePointer	out_he;	//out halfedge pointer

	////geometry
	Point	pos;

////functions
	//constructor
	Vertex(double _x,double _y,int i);
//	Vertex(const Point &p=O,int i=0);
	~Vertex();

	//field access
	int	getIndex()const	{return	index;};	//not user interface, only used by exportXXFile()
	void setIndex(int _i){index=_i;};			//not user interface, only used by exportXXFile()

	//initOutHalfedgeList();
	void addToOutHalfedgeList(HePointer he);	// not an interface for user
	void deleteFromOutHalfedgeList(HePointer he);	//not an interface for use

	HePointer findOutHalfedgeInRing(VPointer v2);
	HePointer findInHalfedgeInRing(VPointer v2);

	HePointer	addHalfedge(VPointer v2,EPointer e);// not an interface to user, only called by addEdge()

	int	getValence()const{return	valence;};

};// of Vertex

// Halfedge
class Halfedge{
public:
////members
	//topology
	VPointer	v;	// vertex pointer to front v(fv)
	//back v(bv) is pre_he->v

	EPointer	e;	// edge pointer
	HePointer inv_he;	// opposite halfedge pointer
	HePointer next_he;	//next halfedge
	HePointer pre_he;	//pre halfedge
	
	//geometry
	//normalized direction	vector
	//	double	v[Dimension];
	Vector	d;//direction from back v to front v

////functions
	Halfedge(VPointer _v=NULL,EPointer _e=NULL):v(_v),e(_e){inv_he=next_he=pre_he=NULL;d.x=0;d.y=0;};
	~Halfedge();
	//field access
	//topology query
	VPointer	getBackVertex()	const{return	inv_he->v;};
	VPointer	getFrontVertex()	const{return	v;};

};// of Halfedge

// Edge
class Edge{
public:
//members
	//topology
	//he[0] is always from left to right
	//he[1] is always from right to left
	HePointer	he[2];	//halfedge pointer pair, he[0]->v->pos.x > he[1]'s

	//scene level
	EListPointer	iter;	

	//type, can be constrained or nonconstrained
	int	type;

//functions
	Edge(VPointer v1,VPointer v2,int type=NON_CONSTRANED);
	~Edge();
	//field access

	//topology query
	//if an Edge exists, there must be 2 Halfedge 
	VPointer	getOneVertex()const	{return	he[0]->v;};	
	VPointer	getTheOtherVertex()const{return he[1]->v;};

};// of Edge

class	EdgeList{
public:
	EPointer	e;
	EListPointer next;
	
	EdgeList(EPointer _e):e(_e){e->iter=this;next=NULL;};
};

//slab
class Slab{
public:
	int	l,r;	// indices of left and right separate line
	QPointer	q_head,q_tail;	// head of list of quads
	SPointer	next;	// next slab 
//methords
	//constructor with li as left index and ri as right index
	Slab(int li,int ri);
	void pushBackQuad(QPointer q);
};

// quad
class Quad{
public:
	// highest and lowest vertices in the quad
	VPointer	top_v,btm_v;

	// top and bottom long edges of this quad (refer to long edge above)
	HePointer	top_he;// top half-edge from right to left
	HePointer	btm_he;// bottom half-edge from left to right

	// indices of left and right vertical line
	int		l,r;

	// y value corresponding to the intersection of top_v and top_he
	double		top_y;
	// y value corresponding to the intersection of btm_v and btm_he
	double		btm_y;
	// y value of four cornor
	double		left_top_y;		
	double		left_btm_y;		
	double		right_top_y;	
	double		right_btm_y;	

	QPointer		next;	// next quad in the list for this slab

//methords
	// constructor with highv as hv, lowv as lv, t as top and b as btm
	Quad(VPointer highv,VPointer lowv,HePointer t,HePointer b,int _l,int _r);
	Quad();
	void updateLongEdge(EPointer e);
	void computeCornorY(double lx,double rx);
};

struct	Window{
//	int	m;		//middle separate line index
	QPointer	lq,rq;	// left and right quad

	//-1 :left above right
	//0	: identical
	// 1: left below right
	int	upsign,downsign;	//sign indicating the intersection situation for lq,rq's top,btm halfedges
};

//given two quad which share a separate line, return their intersection window
//return null if no intersection and set lgr to true if lq is above rq
WPointer	getWindow(QPointer lq,QPointer rq,bool &lgr)
{
#define	LEFT_ABOVE_RIGHT	true
#define	RIGHT_ABOVE_LEFT	!LEFT_ABOVE_RIGHT

	WPointer	w;
#ifdef	DEBUG
	assert(lq->r==rq->l);
#endif
	//use this to avoid comparision double numbers
	if	(	lq->btm_he	==	rq->top_he->inv_he
		||	lq->right_btm_y>rq->left_top_y)
	{
		lgr=LEFT_ABOVE_RIGHT;
		return NULL;
	}

//	w->m=lq->r;

	//use this to avoid comparision double numbers
	if	(	lq->top_he->inv_he	==	rq->btm_he
		||	lq->right_top_y<rq->left_btm_y)
	{
		lgr=RIGHT_ABOVE_LEFT;
		return NULL;
	}

	w=new Window();
	//use this to avoid comparision double numbers
	if	(lq->top_he==rq->top_he)
	{
		w->upsign=0;
	}
	else
	if	(lq->right_top_y>rq->left_top_y)
	{
		w->upsign=-1;
	}
	else//lq->right_top_y<rq->left_top_y
	{
		w->upsign=1;
	}

	if	(lq->btm_he==rq->btm_he)
	{
		w->downsign=0;
	}
	else
	if	(lq->right_btm_y>rq->left_btm_y)
	{
		w->downsign=-1;
	}
	else//lq->right_btm_y<rq->left_btm_y
	{
		w->downsign=1;
	}

	w->lq=lq;
	w->rq=rq;

	return w;
}
//primitives in computational geometry
/******************************************************************
Given ratio of (xq-x1)/(xq-x2), assume xq!=x2
Return:
never be 1
=0	:xq eq x1
<0  : xq in middle of x1,x2
0< <1: x1 is in middle of xq,x2
>1	:x2 is in middle of xq,x1
******************************************************************/
/*
//#EXACT
double	getLambda(double xq,double x1,doube x2)
{
	assert(xq-x2);
	return((xq-x1)/(xq-x2));
}
*/
/******************************************************************
is xq in between x1,x2, assume xq not eq x1 or x2
******************************************************************/
bool	inBetween(double xq,double x1,double x2)
{
	//assert(xq-x1);
	//assert(xq-x2);
	return((xq-x1)*(xq-x2)<0);
}

/******************************************************************
Given two vertices on a line,
get the y-value of vertex on line whose x-vaule is given too.

Using line equation (y-y1)/(x-x1)=(y2-y1)/(x2-x1)
      y*(x2-x1)=(x-x1)*(y2-y1)+y1*(x2-x1)=(x-x1)*y2+y1*(x2-x)
******************************************************************/
//#EXACT
double	yOfLineGivenX(double x1,double y1,double x2,double y2,double x)	
{
	//assert(x2-x1);
  if (x2 == x1)
    return (y1 + y2) / 2.0;
  else
	  return ((x-x1)*y2+y1*(x2-x))/(x2-x1);	
}

/******************************************************************
 Point on the side of radial line: (x,y)'s position according to 
 radial line (ox,oy) to (x1,y1)
 return value: 
   -1 left
    0 on line
    1 right
******************************************************************/
//#EXACT	this function requires exact computation
int	pointToLine(double x,double y,double ox,double oy,double x1,double y1)
{
	// translation, o(x1,y1) as the new Oriental point
	x=x-ox;	y=y-oy;
	x1=x1-ox;	y1=y1-oy;

	//rotation, op1 as the positive x direction
	// x'=cos(a)*x+sin(a)*y *sqrt(x1^2+y1^2)
	// y'=-sin(a)*x+cos(a)*y *sqrt(x1^2+y1^2)
	//-->
	//	x'=x1*x+y1*y
	//	y'=-y1*x+x1*y

	double D=y1*x-x1*y;
   	if (D != 0)
          return (D > 0) ? 1 : -1;
        else
          return 0;
}

/*
  The side of (px,py) to the circumcircle of three points
  (ax,ay), (bx,by) and (cx,cy)
  return value:
  -1 outside
   0 on the circle
   1 inside
*/
//#EXACT	this function requires exact computation
int pointToCircle(double px,double py,
		double ax,double ay,
		double bx,double by,
		double cx,double cy)
{
   bx = bx- ax;
   by = by- ay;
   double bw = bx*bx + by*by;

   cx = cx- ax;
   cy = cy- ay;
   double cw = cx*cx + cy*cy; 
     
   double d1 = cy*bw - by*cw;
   double d2 = bx*cw - cx*bw;
   double d3 = by*cx - bx*cy;

   px = px - ax;
   py = py - ay;
   double pw = px*px + py*py;

   double D = px*d1 + py*d2 + pw*d3;

   if (D != 0)
      return (D > 0) ? 1 : -1;
   else
      return 0;
} 

/* 
  primitive test on whether q(xq,yq) falls in the angle from 
 op1((xo,yo)->(x1,y1)) to op2((xo,yo)->(x2,y2))
  return value: 
  -2 collinear to op2		here COLLINEAR stands for ONLY half of the line
  -1 collinear to op1
  0	outside
  1	yes, inside
*/
//#EXACT	this function requires exact computation
int	pointInAngle(double xo,double yo,
			double	xq,double yq,
			double	x1,double y1,
			double	x2,double y2)
{
	double xx,yy;
	// translation, o(xo,yo) as the new Oriental point
	xq=xq-xo;	yq=yq-yo;
	x1=x1-xo;	y1=y1-yo;
	x2=x2-xo;	y2=y2-yo;

	// whether (xq,yq) is collinear with (x1,y1) and on the same side of (0,0)
	if	(	pointToLine(xq,yq,0,0,x1,y1)==0
		&&	!	inBetween(0,xq,x1)	)
		return	-1;
	if	(	pointToLine(xq,yq,0,0,x2,y2)==0
		&&	!	inBetween(0,xq,x2)	)
		return	-2;

	//rotation, op1 as the positive x direction
	// x'=cos(a)*x+sin(a)*y 
	// x'*sqrt(x1^2+y1^2)=x1*x+y1*y
	// y'=-sin(a)*x+cos(a)*y 
	// y'*sqrt(x1^2+y1^2)=-y1*x+x1*y
	xx=xq;	yy=yq;
	xq=x1*xx+y1*yy;
	yq=-y1*xx+x1*yy;

	xx=x2;	yy=y2;
	x2=x1*xx+y1*yy;
	y2=-y1*xx+x1*yy;

	double sign=xq*y2-x2*yq;
	// here we assume no collinear
	if(x2>=0)
	{
		if(y2>0)
		{
			// (x2,y2) is in 1st quadrant
			return(sign>0 && xq>0 && yq>0);
		}
		else
		{
			//4th quadrant
			return !(sign<0 && xq>0 && yq<0);
		}
	}
	else
	{
		if(y2>=0)
		{
			//2nd 
			return( yq>0 &&(xq>=0 || xq<0 && sign>0));
		}
		else
		{
			//3rd 
			return( yq>=0 || yq<0 && xq<0 && sign>0);
		}
	}
}

// *************************************************************
//	sort algorithm used to sort vertices by x value
// *************************************************************
//***************** quick sort functions begin **********************
// for partition
void	swap(VPointer* array,int i,int j)
{
	VPointer tmpv;

	tmpv=array[i];
	array[i]=array[j];
	array[j]=tmpv;
	//modify index
	/* commented 08/02, so that array[i]->index remains the sort information
	array[i]->index=i;
	array[j]->index=j;
	*/
}

// for quick sort
int	partition(VPointer* array,int p,int r)
{
	double x;
	int	i;
	
	x=array[r]->pos.x;
	i=p-1;
	for (int j=p;j<r;j++)
	{
		if(array[j]->pos.x==x)
		{
			// vertices sharing same x values
			reportWarning("Not supporting vertiecs sharing same x value now!",__LINE__,__FILE__);
#ifdef	NOT_SUPPORT_SAME_X
			exit(-1);
#endif
		}
		else
		if(array[j]->pos.x<x)
		{
			i++;
			//	exchange array[i] and array[j]
			swap(array,i,j);
		}
	}
	swap(array,i+1,r);
	return i+1;
}

// quick sort
// input :from is the index where sort begins
// count is the index of where sort ends
// usually quicksort(array,0,n-1)
void quicksort(VPointer* array,int from,int to)
{
	int middle;
	if(from<to)
	{
		middle=partition(array,from,to);
#ifdef DEBUG
		if
		(middle>=0)
		{
#endif
			quicksort(array,from,middle-1);
			quicksort(array,middle+1,to);
#ifdef DEBUG
		}
#endif
	}	
}
