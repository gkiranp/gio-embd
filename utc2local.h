#ifndef UTC_2_LOCAL_H
#define UTC_2_LOCAL_H

#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

class UtcToLocalTime {
private:
	std::string m_UtcTime;
	char m_TimeZone[125];
	std::string m_LocalTime;

private:
	int str2i(std::string s);
	void split(const std::string &s, char delim, vector<std::string> &elems);
	vector<std::string> str_split(const std::string &s, char delim);
	map<std::string, int> month_map(void);
	int get_month(std::string name);
	
public:
	UtcToLocalTime();
	~UtcToLocalTime();
	std::string UTC2LOCAL(std::string utc_datetime, const char* tz_info);
	std::string FormatTimeInfo(std::string raw_time);
};

#endif