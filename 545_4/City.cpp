#include "City.hpp"
#include <cmath>

using namespace std;

double City::DistanceTo(City c) const
{
    return sqrt(pow(c._latitude - this->_latitude,2) + pow(c._longitude - this->_longitude,2));
}

double City::DistanceFromEdge(City s, City t)
{
	double a, b, d;
	//http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
	
	if((abs(s._longitude - this->_longitude) + abs(t._longitude - this->_longitude) == abs(s._longitude - t._longitude))
			&& (abs(s._latitude - this->_latitude) + abs(t._latitude - this->_latitude) == abs(s._latitude - t._latitude)))
	{
		a = t._latitude - s._latitude;
		b = t._longitude - s._longitude;
		d = abs(a*(s._longitude-this->_longitude)-b*(s._latitude-this->_latitude))/sqrt(a*a+b*b);
	} else {
		d = min(this->DistanceTo(s), this->DistanceTo(t));
	}
	
	return d;
	
}