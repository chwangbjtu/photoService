#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include "k_str.h"
#include "http_util.h"
#include "http_response.h"
//#include "util.h"
#include "fp_base_handler.h"

using namespace std;

fp_base_handler::fp_base_handler()
{
	//_return_succ = util::instance()->get_return_succ();
}

int fp_base_handler::pack(string& msg,string& resp)
{
	fs::http_response http_resp;
	
	http_resp._resp._content = msg;
	if(http_resp.pack(resp) < 0) 
	{
		return -1;
	}
	return 0;
}

const string& fp_base_handler::find_user_agent(const fs::http_request& hr)
{
    static const string empty;
    multimap<string, string>::const_iterator user_agent_it = hr._req._map_headers.find("User-Agent");

    if(hr._req._map_headers.end() == user_agent_it)
    {
        return empty;
    }

    return user_agent_it->second;
}

const unsigned int fp_base_handler::find_xforwardedfor(const fs::http_request& hr)
{
    /*
	unsigned int ip = 0;
	
	//format : X-Forwarded-For: client1, proxy1, proxy2
	const unsigned int least_addr_num = 1;
	const int client_index = 0;
	
	static const char* x_forwarded_for = "X-FORWARDED-FOR";
	multimap<string,string>::const_iterator forwarded_iter = hr._req._map_headers.begin();
	multimap<string,string>::const_iterator forwarded_iter_end = hr._req._map_headers.end();
	for(; forwarded_iter != forwarded_iter_end; ++forwarded_iter) 
	{
		const string header_keyword = fs::str2upper( forwarded_iter->first );
		if( header_keyword.compare(x_forwarded_for) != 0 )
			continue;

		//DBG_INFO(("%s", forwarded_iter->second.c_str()));
		vector<string> proxy_addr;
		fs::split(forwarded_iter->second,',',proxy_addr);

		if (proxy_addr.size() >= least_addr_num) 
		{
			util::instance()->str2ip(proxy_addr[client_index], ip); 
		}
		break;
	}

	return ip;
    */
    return 0;
}

int fp_base_handler::is_digit(const string & str)
{
        int size = str.length();
        if ( size == 0)
        {
                return -1;
        }

        const char DIGIT_MAX = '9';
        const char DIGIT_MIN = '0';
        int i = 0;
        int ret = 0;

        while ( i < size )
        {
                if ( str[i] <= DIGIT_MAX && str[i] >= DIGIT_MIN )
                {
                        i++;
                        continue;
                }
                else 
                {
                        ret = -1;
                        break;
                }
        }

        return ret;
}


