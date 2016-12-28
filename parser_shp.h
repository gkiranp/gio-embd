#ifndef PARSER_SHP_H
#define PARSER_SHP_H

#include <iostream>
#include <string.h>
#include <string>
#include <math.h>
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <list>
#include <vector>
#include "./shapelib/shapelib-1.3.0/shapefil.h"

#define WORLD_SHP_FILE_SHP "./tz_world_mp/world/tz_world_mp.shp"
#define WORLD_SHP_FILE_DBF "./tz_world_mp/world/tz_world_mp.dbf"
#define PRINT_MAP_ON_CONSOLE 0

using namespace std;

typedef enum {
	eStsFailed = 0,
	eStsSuccess = 1
}eUserStatus;

typedef struct _user_map_shapes_lookup {
	std::string city;
	double latN;
	double lonE;
	double latS;
	double lonW;
	double dist;
}st_CityMap;

class MapperShp
{
private:
	double deg2rad (double degree);
	double rad2deg (double radians);
	double destination_lat(double lat, double lon, double bearing, double distance);
	double destination_lon(double lat, double lon, double bearing, double distance);

public:
	MapperShp();
	~MapperShp();
	double bound_N_lat(double lat, double lon, double distance);
	double bound_E_lon(double lat, double lon, double distance);
	double bound_S_lat(double lat, double lon, double distance);
	double bound_W_lon(double lat, double lon, double distance);
	double distance(double latA, double lonA, double latB, double lonB);
};

class ParserShpMap
{
private:
	SHPHandle 		m_HdlShapes;
	DBFHandle 		m_HdlDatabase;
	SHPObject 		*m_ShpObj;
	DBFFieldType 	m_DbObj;
	char 			m_DBCity[50];
	eUserStatus 	m_InitSts;
	int 			m_ShapesCount;
	int 			m_DBCount;
	std::list< st_CityMap > m_CityMap;

private:
	eUserStatus InitializeParser();

public:
	ParserShpMap();
	~ParserShpMap();
	std::vector< st_CityMap > GetCitiesRange(double iLat, double iLon);
};

#endif
