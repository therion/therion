/* geom2d.h */
//#define	CLOCKWISE	true
//#define	COUNTER_CLOCKWISE	!CLOCKWISE
const	bool	Clockwise_Order=true;
const	bool	Counter_Clockwise_Order=!Clockwise_Order;

const	int	Dimension=2;

struct	Point2D{
	double	x;
	double	y;
};

typedef	Point2D	Point;
typedef	Point	Vector;

const	Point	O={.0,.0};
const	Point	I={1,1};
const	Vector	X={1,.0};
const	Vector	Y={.0,1};
