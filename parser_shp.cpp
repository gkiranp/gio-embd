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
#include "parser_shp.h"

using namespace std;

namespace str2str {
    template < typename T > std::string to_string( const T& n ){
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
    template < typename T > std::string to_string( const T* n ){
		std::string s(n);
		return s;
	}
}

MapperShp::MapperShp() {}
MapperShp::~MapperShp() {}

double MapperShp::deg2rad (double degree) {
		return ((double)((M_PI*degree)/180.0));
}

double MapperShp::rad2deg (double radians) {
	return ((double)(radians * (180.0 / M_PI)));
}

double MapperShp::distance(double latA, double lonA, double latB, double lonB) {
	double radius = 6378.137;
	double rLatA = deg2rad(latA);
	double rLatB = deg2rad(latB);
	double rHalfDeltaLat = deg2rad((latB - latA) / 2.0);
	double rHalfDeltaLon = deg2rad((lonB - lonA) / 2.0);
	return (2 * radius * asin(sqrt(pow(sin(rHalfDeltaLat), 2.0) + cos(rLatA) * cos(rLatB) * pow(sin(rHalfDeltaLon), 2.0))));
}

double MapperShp::destination_lat(double lat, double lon, double bearing, double distance) {
    double radius = 6378.137;
    double rLat = deg2rad(lat);
    double rLon = deg2rad(lon);
    double rBearing = deg2rad(bearing);
    double rAngDist = (double)(distance/radius);
    double rLatB = asin(sin(rLat) * cos(rAngDist) + cos(rLat) * sin(rAngDist) * cos(rBearing));
    return rad2deg(rLatB);
}

double MapperShp::destination_lon(double lat, double lon, double bearing, double distance) {
    double radius = 6378.137;
    double rLat = deg2rad(lat);
    double rLon = deg2rad(lon);
    double rBearing = deg2rad(bearing);
    double rAngDist = (double)(distance/radius);
    double rLatB = asin(sin(rLat) * cos(rAngDist) + cos(rLat) * sin(rAngDist) * cos(rBearing));
    double rLonB = rLon + atan2(sin(rBearing) * sin(rAngDist) * cos(rLat), cos(rAngDist) - sin(rLat) * sin(rLatB));
    return rad2deg(rLonB);
}

double MapperShp::bound_N_lat(double lat, double lon, double distance) {
	return destination_lat(lat, lon, 0, distance);
}

double MapperShp::bound_E_lon(double lat, double lon, double distance) {
	return destination_lon(lat, lon, 90, distance);
}

double MapperShp::bound_S_lat(double lat, double lon, double distance) {
	return destination_lat(lat, lon, 180, distance);
}

double MapperShp::bound_W_lon(double lat, double lon, double distance) {
	return destination_lon(lat, lon, 270, distance);
}

ParserShpMap::ParserShpMap()
{
	m_InitSts = InitializeParser();
}

ParserShpMap::~ParserShpMap()
{
	if(m_ShpObj) {
		SHPDestroyObject(m_ShpObj);
	}
}

eUserStatus ParserShpMap::InitializeParser()
{
	eUserStatus sts = eStsFailed;
	int index = 0;
	const char *field_name;
	st_CityMap CityElement;
	double dist = 0.0;
	MapperShp *mShp = new MapperShp();

	m_ShpObj = NULL;
	m_CityMap.clear();
	m_HdlShapes = SHPOpen(WORLD_SHP_FILE_SHP, "rb");
	m_HdlDatabase = DBFOpen(WORLD_SHP_FILE_DBF, "rb");
	if(m_HdlShapes && m_HdlDatabase)
	{
		SHPGetInfo(m_HdlShapes, &m_ShapesCount, NULL, NULL, NULL);
		m_DBCount = DBFGetRecordCount(m_HdlDatabase);
		if((m_DBCount == m_ShapesCount) && (m_DBCount > 0) && (m_ShapesCount > 0)) {
			/* let's build an offline map */
			for(index = 0; index < m_DBCount; index++) {
				m_ShpObj = SHPReadObject(m_HdlShapes, index);
				field_name = DBFReadStringAttribute(m_HdlDatabase, index, 0);
				if((field_name) && (m_ShpObj)) {
					dist = mShp->distance(	m_ShpObj->dfXMin,
											m_ShpObj->dfYMin,
											m_ShpObj->dfXMax,
											m_ShpObj->dfYMax);
					CityElement.dist = dist;
					CityElement.latN = m_ShpObj->dfYMax;
					CityElement.lonE = m_ShpObj->dfXMax;
					CityElement.latS = m_ShpObj->dfYMin;
					CityElement.lonW = m_ShpObj->dfXMin;
					CityElement.city = str2str::to_string(field_name);
#if PRINT_MAP_ON_CONSOLE
					std::cout << CityElement.city << ","
							  << CityElement.latN << ","
							  << CityElement.lonE << ","
							  << CityElement.latS << ","
							  << CityElement.lonW << ","
							  << dist << std::endl;
#endif
					m_CityMap.push_back(CityElement);
				}
			}
			if(m_ShpObj) {
				SHPDestroyObject(m_ShpObj);
				m_ShpObj = NULL;
			}
			sts = eStsSuccess;
		}
	}
	return sts;
}

std::vector< st_CityMap > ParserShpMap::GetCitiesRange(double iLat, double iLon)
{
	std::vector< st_CityMap > cities;
	if(m_InitSts == eStsSuccess) {
		st_CityMap city_element;
		for (std::list< st_CityMap >::iterator it = m_CityMap.begin(); it != m_CityMap.end(); ++it) {
			city_element = *it;
			if((city_element.latS <= iLat) &&
			   (city_element.latN >= iLat) &&
			   (city_element.lonW <= iLon) &&
			   (city_element.lonE >= iLon)) {
				cities.push_back(city_element);
			}
		}
	}
	return cities;
}
