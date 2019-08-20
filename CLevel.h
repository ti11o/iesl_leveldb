#ifndef CLEVEL_H
#define CLEVEL_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdio>
#include <ctime>
#include <string.h>
#include <dirent.h>
#include <jsoncpp/json/json.h>

class CLevel{
    
    public:
	CLevel();
	std::string PutJson();
	unsigned long PutDir();
	std::string Get(std::string Key);
	void Delete(std::string key);
};


#endif
