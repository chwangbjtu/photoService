#ifndef __UTIL_H
#define __UTIL_H

#include <iostream>

using namespace std;

class util
{
public:
	~util(){}
	
	static util* instance();

	string get_error_resp();

    string get_ok_resp();

	int escape2str(const char * str, int len, string &res);

	int ip2str(const unsigned int & ip,string& ipstr);
	int str2ip(const string& str,unsigned int& ip);
	
private:
	util();
	static util* _inst;
	char * HEX_STR;
    string _err_resp;
    string _ok_resp;
};

#endif//__UTIL_H



