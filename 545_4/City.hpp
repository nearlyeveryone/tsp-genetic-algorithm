#ifndef CITY_HPP
#define CITY_HPP

#include <vector>

class City
{    
	public:
		double _longitude;
        double _latitude;
		int CityId;
		City();
        City(double longitude, double latitude, int cid): _longitude(longitude), _latitude(latitude), CityId(cid) {}
		std::vector<City*> Connections;
        double DistanceTo(City c) const;
		double DistanceFromEdge(City s, City t);
		
		
		bool operator <(const City &rhs)
		{
			if(_longitude == rhs._longitude)
			{
				return _latitude < rhs._latitude;
			} else {
				return _longitude < rhs._longitude;
			}
		}
};

#endif // CITY_HPP
