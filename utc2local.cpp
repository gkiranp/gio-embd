#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "./utc2local.h"

#define ENABLE_USAGE 0

namespace str2str
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}

using namespace std;

UtcToLocalTime::UtcToLocalTime() {
	m_UtcTime.clear();
	m_LocalTime.clear();
	memset(m_TimeZone, '\0', 125);
}

UtcToLocalTime::~UtcToLocalTime() {}

std::string UtcToLocalTime::UTC2LOCAL(std::string utc_datetime, const char* tz_info) {
	/**
	 The utc_datetime supported in format "yyyy-mm-dd HH:MM:SS" only
	*/
	struct tm   lt;
    time_t      stamp;
	std::string date_str = "";
	std::string time_str = "";
	std::string str = "";
	vector<string> elems;
	
	if((!utc_datetime.empty()) && (tz_info)) {
		putenv("TZ=UTC"); //let's set UTC time format 
		memset(m_TimeZone, 0, 125);
		strncpy(m_TimeZone, tz_info, 125);
		elems.clear();
		elems = str_split(utc_datetime, ' '); //this gets us {yyyy-mm-dd}{hh:mm:ss}
		date_str = elems.at(0); //{yyyy-mm-dd}
		time_str = elems.at(1); //{hh:mm:ss}
		elems.clear();
		elems = str_split(date_str, '-'); //this gets us {yyyy}{mm}{dd}
		str = elems.at(2); //day of month
		lt.tm_mday = str2i(str);
		str = elems.at(1); //month
		lt.tm_mon = (str2i(str) - 1);
		str = elems.at(0); //year
		lt.tm_year = (str2i(str) - 1900);
		elems.clear();
		elems = str_split(time_str, ':'); //this gets us {hh}{mm}{ss}
		str = elems.at(0); //hh
		lt.tm_hour = str2i(str);
		str = elems.at(1); //mm
		lt.tm_min  = str2i(str);
		str = elems.at(2); //ss
		lt.tm_sec  = str2i(str);
		stamp = mktime(&lt); //Unix timestamp
		putenv(m_TimeZone); //Switch to destination time zone
		str = asctime(localtime(&stamp));
		m_LocalTime = str;
	}
	return str;
}

std::string UtcToLocalTime::FormatTimeInfo(std::string raw_time) {
	/**
	 Return format will be "yyyy-mm-ddTHH:MM:SS" only
	 And raw_time shall be of type "Www Mmm dd hh:mm:ss yyyy" only
	*/
	vector<std::string> time_elems;
	std::string str = "";
	std::string formatted_str = "";
	if(!raw_time.empty()) {
		time_elems = str_split(raw_time, ' '); //this gets us {Www}{Mmm}{dd}{hh:mm:ss}{yyyy}
		formatted_str = "";
		formatted_str.append(time_elems.at(4),0,4); //yyyy
		formatted_str.append("-");
		str = time_elems.at(1); //Mmm
		str = str2str::to_string(get_month(str));
		if(str.length() == 1) {str.insert(0, "0");}
		formatted_str.append(str);
		formatted_str.append("-");
		formatted_str.append(time_elems.at(2)); //dd
		formatted_str.append("T");
		formatted_str.append(time_elems.at(3)); //hh:mm:ss
	}
	return formatted_str;
}

int UtcToLocalTime::str2i(std::string s) {
	int x = 0;
	stringstream convert(s);
	convert>>x;
	return x;
}

void UtcToLocalTime::split(const std::string &s, char delim, vector<std::string> &elems) {
	stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

vector<std::string> UtcToLocalTime::str_split(const std::string &s, char delim) {
	vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

map<std::string, int> UtcToLocalTime::month_map(void) {
	map<std::string,int> m;
	m["Jan"] = 1;
	m["Feb"] = 2;
	m["Mar"] = 3;
	m["Apr"] = 4;
	m["May"] = 5;
	m["Jun"] = 6;
	m["Jul"] = 7;
	m["Aug"] = 8;
	m["Sep"] = 9;
	m["Oct"] = 10;
	m["Nov"] = 11;
	m["Dec"] = 12;
	return m;
}

int UtcToLocalTime::get_month( std::string name ) {
	map<std::string, int> months = month_map();
    map<std::string, int>::iterator iter = months.find( name );
    if( iter != months.end() )
        return iter->second;
    return -1;
}

#if ENABLE_USAGE
int main()
{
	UtcToLocalTime *obj = new UtcToLocalTime();
	std::string str;
	//e.g No 1
	str = obj->UTC2LOCAL("2013-07-10 02:52:49", "TZ=Pacific/Auckland"); //format yyyy-mm-dd HH:MM:SS
	std::cout << "Local Time : " << str;
	std::cout << obj->FormatTimeInfo(str) << std::endl;
	//e.g No 2
	str = obj->UTC2LOCAL("2013-07-10 02:52:49", "TZ=Australia/Sydney"); //format yyyy-mm-dd HH:MM:SS
	std::cout << "Local Time : " << str;
	std::cout << obj->FormatTimeInfo(str) << std::endl;
	return 0;
}
#endif