#include <iostream>
#include <string.h>
#include <string>
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include "./shapelib/shapelib-1.3.0/shapefil.h"
#include "parser_shp.h"
#include "utc2local.h"

#define DISCARD_CITY_ENTRY1 "uninhabited"

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

using namespace std;

/**
 * Struct: User defined inputs
 */
typedef struct _user_input_parameters {
	std::string utc_datetime;
	double lat;
	double lon;
}st_InputParams;

/**
 * Utility: prints usage of this application
 */
void usage(std::string app_name) {
	if(!app_name.empty()) {
		std::cout << app_name << " File_Input.csv [File_To_Output]" << std::endl;
		std::cout << "where," << std::endl << "  - File_Input.csv : input a CSV file with path to read" << std::endl;
		std::cout << "  - File_To_Output : if provided output will be saved to this file";
		std::cout << "otherwise output will be displayed in stdout";
	}
}

/**
 * Utility: to read any file line by line
 */
std::vector<std::string> read_csv_file(std::string file_name) {
	std::vector<std::string> readBuf;
	if(!file_name.empty()) {
		std::ifstream csv_file;
		csv_file.open(file_name.c_str());
		std::string rd_line;
		while (std::getline(csv_file, rd_line))	{
			if(!rd_line.empty()) {
				readBuf.push_back(rd_line);
			}
		}
		if(csv_file.is_open()) {
			csv_file.close();
		}
	}
	return readBuf;
}

/**
 * Utility: convert std string to integer
 */
int str2i(const std::string &s) {
	int x = 0;
	stringstream convert(s);
	convert>>x;
	return x;
}

/**
 * Utility: convert std string to double
 */
double str2d(const std::string &s) {
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		return 0.0;
	return x;
}

/**
 * Utility: to split std string by delimiters
 */

vector<std::string> str_split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
    return elems;
}

/**
 * The main application starts from here
 */

int main(int argc, char* argv[])
{
	/* ! initialized ! */
	std::string InputFile = "";
	std::string OutputFile = "";
	std::ofstream OutStream;
	if(argc < 2) {
		std::cout << "Error: too few arguments." << std::endl;
		usage(str2str::to_string(argv[0]));
		return -1;
	}else if(argc > 3) {
		std::cout << "Error: too many arguments." << std::endl;
		usage(str2str::to_string(argv[0]));
		return -1;
	}else if(argc == 3) {
		InputFile = str2str::to_string(argv[1]);
		OutputFile = str2str::to_string(argv[2]);
		OutStream.open(OutputFile.c_str());
	}else if(argc == 2) {
		InputFile = str2str::to_string(argv[1]);
		OutputFile = "";
	}

	/* ! required process data declarations ! */
	std::vector <st_CityMap> matching_cities;
	std::vector <std::string> csv_entries;
	std::string csv_line = "";
	std::string processed_line = "";
	std::vector <std::string> elements;
	st_CityMap city_element;
	st_InputParams usr_input;
	ParserShpMap *world_map = new ParserShpMap(); //this might take a while to construct whole world map
	MapperShp *mapper = new MapperShp();
	UtcToLocalTime *utc2local = new UtcToLocalTime();
	const char *timezone;
	std::string tz_str = "";
	double dist_min = 0.0;
	double dist_max = 0.0;

	/* ! read CSV file into list !*/
	csv_entries = read_csv_file(InputFile);

	/* ! parse all the entries in file one by one and output ! */
	for(std::vector<std::string>::iterator it = csv_entries.begin(); it != csv_entries.end(); ++it) {
		matching_cities.clear();
		elements.clear();
		processed_line = "";
		tz_str = "";
		timezone = NULL;
		csv_line = *it;
		if(!csv_line.empty()) {
			/** this shall gets us {UTC-datetime}{latitude}{longitude}
			 *  if not, this entry shall not be parsed
			 **/
			elements = str_split(csv_line, ',');
			if((!elements.empty()) && (elements.size() == 3)) {
				usr_input.utc_datetime = elements.at(0);
				usr_input.lat = str2d(elements.at(1));
				usr_input.lon = str2d(elements.at(2));
				matching_cities = world_map->GetCitiesRange(usr_input.lat, usr_input.lon);
				if(!matching_cities.empty()) {
					for(std::vector <st_CityMap>::iterator itm = matching_cities.begin();
						itm != matching_cities.end() ; ++itm)
					{
						city_element = *itm;
						if(city_element.city != DISCARD_CITY_ENTRY1) {
							dist_min = mapper->distance(city_element.latS, city_element.lonW, usr_input.lat, usr_input.lon);
							dist_max = mapper->distance(usr_input.lat, usr_input.lon, city_element.latN, city_element.lonE);
							if((dist_min <= city_element.dist) && (dist_max <= city_element.dist)) {
								/* this is a valid city entry */
								processed_line = city_element.city;
								break;
							}
						}
					}
					/* ! define timezone info ! */
					if(!processed_line.empty()) {
						tz_str = "TZ=" + processed_line;
						timezone = tz_str.c_str();
					}
					/* display on stdout or write into file */
					if(!OutputFile.empty()) {
						OutStream << csv_line << ","
								  << processed_line << ","
								  << utc2local->FormatTimeInfo(utc2local->UTC2LOCAL(usr_input.utc_datetime, timezone))
								  << "\n";
					}else {
						std::cout << csv_line << ","
								  << processed_line << ","
								  << utc2local->FormatTimeInfo(utc2local->UTC2LOCAL(usr_input.utc_datetime, timezone))
								  << std::endl;
					}
				}
			}
		}
	}

	/* ! it is always safe to destroy the objects and close files opened after usage ! */
	if((!OutputFile.empty()) && (OutStream.is_open())){
		OutStream.close();
	}
	if(world_map) {
		delete world_map;
		world_map = NULL;
	}
	if(mapper) {
		delete mapper;
		mapper = NULL;
	}
	if(utc2local) {
		delete utc2local;
		utc2local = NULL;
	}

	return 0; //safely return
}
