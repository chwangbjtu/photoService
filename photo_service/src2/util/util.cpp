#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include "dbg.h"
#include "json/json.h"
#include "util.h"

using namespace std;

util* util::_inst = NULL;
util* util::instance()
{
	if ( _inst == NULL)
		_inst = new util();
	return _inst;
}

util::util()
{
    /*
    Json::Value tmp_value;
    tmp_value["error"] = "1";
    _err_resp = tmp_value.toStyledString();


    Json::Value ok_value;
    ok_value["error"] = "0";
    _ok_resp = ok_value.toStyledString();
    */

    _err_resp = "{\"error\":\"1\"}";
    _ok_resp = "{\"error\":\"0\"}";
}


string util::get_error_resp()
{
    return _err_resp;
}

string util::get_ok_resp()
{
    return _ok_resp;
}

int util::ip2str(const unsigned int & ip,string& ipstr)
{
	struct in_addr ip_addr;
	memset(&ip_addr,0,sizeof(in_addr));
	ip_addr.s_addr = ip;
	char ipbuf[INET_ADDRSTRLEN]= {0};
	if ( inet_ntop(AF_INET,(struct in_addr*)&ip_addr,ipbuf,INET_ADDRSTRLEN)!= NULL) 
	{
		ipstr = string(ipbuf,strlen(ipbuf));
		return 0;
	}
	return -1;
}

int util::str2ip(const string& str,unsigned int& ip)
{
	in_addr ipaddr;
	memset(&ipaddr,0,sizeof(ipaddr));
	if ( inet_pton(AF_INET,str.c_str(),(struct in_addr*)&ipaddr) > 0) 
	{
		ip = ntohl(ipaddr.s_addr);
		return 0;
	}
	return -1;
}



/*
*	decode escape string (like %AB%0D%....) to strings
*@param str	input string to decode
*@param len	length of the str
*@res	out, decode result
*return:
*	0, always success
*/
int util::escape2str(const char * str, int len, string &res)
{
	int i=0;
	int pos = 0;
	char buf[65535] = {0};
	while(i < len)
	{
		char c = *(str+i);
		if(c != '%')
		{
			*(buf+pos) = c;
			pos++;
			i++;
		}
		else
		{
			if(i < len-2)
			{
				i++;
				c = *(str+i);

				if(c>='0' && c<='9')
					*(buf+pos) |= ((c-'0')<<4);
				else if(c>='a' && c<='f')
					*(buf+pos) |= ((c-'a'+10)<<4);
				else if(c>='A' && c<='F')
					*(buf+pos) |= ((c-'A'+10)<<4);
				else
					break;

				i++;
				c = *(str+i);
				if(c>='0' && c<='9')
					*(buf+pos) |= ((c-'0'));
				else if(c>='a' && c<='f')
					*(buf+pos) |= ((c-'a'+10));
				else if(c>='A' && c<='F')
					*(buf+pos) |= ((c-'A'+10));
				else
					break;

				pos++;
				i++;
			}
			else
				break;
		}
	}

	res.assign(buf, pos);
	return 0;
}

