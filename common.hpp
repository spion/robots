#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>



#define BOXINFL_B 0.7

using namespace std;



double GetTickCount()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
}


double uniform_deviate (int seed)
{
	return seed * ( 1.0 / ( RAND_MAX + 1.0 ) );
}
	
double gaussian_deviate(double stdev = 1.0)
{
	double x1, x2, w, y1, y2;	  
	do {
		x1 = 2.0 * uniform_deviate(rand()) - 1.0;
		x2 = 2.0 * uniform_deviate(rand()) - 1.0;
		w = x1 * x1 + x2 * x2;
	} while ( w >= 1.0 );
	w = sqrt( (-2.0 * log( w ) ) / w );
	y1 = x1 * w;
	y2 = x2 * w;
	double d = uniform_deviate(rand());
	if (d < 0.5) return stdev*y1;
	else { return stdev*y2; }
}

double gaussian_error(double stdev)
{
	return stdev*gaussian_deviate() + 1.0;
}

double uniform_error(double stdev)
{
	return (1.0 - stdev/2.0) + (stdev * uniform_deviate(rand()));
}

void debug(const string& s)
{
#ifdef _DEBUG
	cout << s << endl;
#endif
}
class SCoord
{
	public:
	double x, y;
	SCoord(double xx, double yy):x(xx), y(yy) { }
	SCoord() { x = 0.0; y = 0.0; }
	bool operator < (const SCoord& c) const
		{ return (x < c.x) && (y < c.y); }
	bool operator > (const SCoord& c) const
		{ return (x > c.x) && (y > c.y); }
	bool operator == (const SCoord& c) const { 
		return ((*this - c).absc() < BOXINFL_B/2.0); 
	}
	friend SCoord operator - (const SCoord& c1, const SCoord& c2)
	{
		SCoord temp;
		temp.x = c1.x - c2.x;
		temp.y = c1.y - c2.y;
		return temp;
	}	

	
	const double absc() const { return sqrt(x*x + y*y); }


	friend SCoord operator + (const SCoord& c1, const SCoord& c2)
	{
		SCoord temp;
		temp.x = c1.x + c2.x;
		temp.y = c1.y + c2.y;	
		return temp;
	}

	friend SCoord operator / (const SCoord& c, double d)
	{
		return SCoord(c.x/d, c.y/d);
	}

	friend SCoord operator * (const SCoord& c, double d)
	{
		return SCoord(c.x*d, c.y*d);
	}
	friend ostream& operator << (ostream& s, const SCoord& c)
	{
		return (s << c.x << " " << c.y);
	}

};


class SBox
{
	public:
	SCoord sta, end;
	bool InBox(const SCoord& c) const
	{
		return ((sta - SCoord(BOXINFL_B,BOXINFL_B) < c) && (c < end + SCoord(BOXINFL_B,BOXINFL_B)));
	}
	SBox(double xs, double ys, double xe, double ye): sta(xs,ys), end(xe, ye) { }
	SBox(SCoord c1, SCoord c2): sta(c1), end(c2) { }
	SBox():sta(0,0), end(0, 0) { }
};

bool InBox(const SCoord& sensor, const vector<SBox>& boxes)
{
	bool inbox = false;
	//cout << "Checking " << boxes.size() << "boxes" << endl;
	for (unsigned i = 0; i < boxes.size(); ++i)
	{
		if (boxes[i].InBox(sensor))
		{
			//cout << "Sensor " << sensor << " InBox : " << boxes[i].sta << " " << boxes[i].end << endl;
			inbox = true;
			break;
		}
	}
	return inbox;
}

double getDirection(const SCoord& dest, double resolution = 36.0)
{
	int direction = round(atan2(dest.y, dest.x)/M_PI * resolution)*(180.0/resolution); 
	if (direction < 0) direction = 360 + direction;
	return direction;	
}

double getDelta(const SCoord& d1, const SCoord& d2, double resolution = 36.0)
{
	int delta = abs(getDirection(d1,resolution) - getDirection(d2,resolution));
	if (delta > 180) delta =  abs(360 - delta);
	return delta;
}



double sgn(double d) { return (d == 0) ? d : d/abs(d); }



bool sgncomp(double d1, double d2)
{
	return (d1*d2 >= 0);
}

SCoord sgnc(const SCoord& c, const double& mul)
{
	double addx = 0.0, addy = 0.0;
	if (c.x == 0) { addx = 0.01; }
	if (c.y == 0) { addy = 0.01; }
	return SCoord(c.x/(abs(c.x)+addx)*mul, c.y/(abs(c.y)+addy)*mul);
}

SCoord sgncf(const SCoord& c, const double& mul)
{
	if (abs(c.x) == abs(c.y))
		return SCoord(c.x/sqrt(2), c.y/sqrt(2));
	else
		return c;	
}

double absc(const SCoord& c)
{
	return sqrt((c.x*c.x) + (c.y*c.y));
}


SBox minibox(const SCoord& c, double sz)
{
	return SBox(c.x - sz, c.y - sz, c.x + sz, c.y + sz);
}



SCoord Intersect(const SBox& b1, const SBox& b2)
{
	
	SCoord intersection(0.0,0.0);
	//x1 = b1.sta.x
	//x2 = b1.end.x
	//x3 = b2.sta.x
	//x4 = b2.end.x
	double denom = (b1.sta.x - b1.end.x)
				 	* (b2.sta.y - b2.end.y)
				 - (b1.sta.y - b1.end.y)
				 	* (b2.sta.x - b2.end.x);
	if (abs(denom) < 0.0001) return intersection;

	double left = b1.sta.x*b1.end.y - b1.sta.y*b1.end.x;
	double right = b2.sta.x*b2.end.y - b2.sta.y*b2.end.x;
	intersection.x = left * (b2.sta.x - b2.end.x) -
					 (b1.sta.x - b1.end.x) * right;

	intersection.y = left * (b2.sta.y - b2.end.y) -
					 (b1.sta.y - b1.end.y) * right;
	intersection.x /= denom;
	intersection.y /= denom;
	return intersection;

}
