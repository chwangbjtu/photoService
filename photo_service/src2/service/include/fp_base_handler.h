#ifndef __FP_BASE_HANDLER_H
#define __FP_BASE_HANDLER_H
#include <string>
#include <map>

#include "http_request.h"

using namespace std;
class fp_base_handler
{
public:
	enum
	{
		max_resp_len = 2048
	};

	enum
	{
		return_ok,
		req_invalid
	};
	
	fp_base_handler();
	
	//virtual int process(const multimap<string,string>& req,unsigned int ip,string& resp){return -1;}
	virtual int process(const fs::http_request& hr,unsigned int ip,unsigned int tid,string& vid,int& width,int& height, string& resp){return -1;}
	
	virtual ~fp_base_handler(){};
protected:

	int pack(string& msg,string& resp);

	const std::string& find_user_agent(const fs::http_request& hr);
	const unsigned int find_xforwardedfor(const fs::http_request& hr);

    int is_digit(const string & info);

	string _return_succ;
};

#endif //__FP_BASE_HANDLER_H

