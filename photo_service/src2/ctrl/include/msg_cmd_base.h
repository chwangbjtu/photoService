#ifndef __MSG_CMD_BASE_H
#define __MSG_CMD_BASE_H

#include <iostream>
#include <string>
#include "cmdq.h"

using namespace std;

class msg_cmd_base
{
public:
	
typedef enum {
	msg_type_sendmsg		= 0x0001
	
} msg_type_ft;

	msg_cmd_base(string msg);
	virtual ~msg_cmd_base();
public:
	string get_msg();
	void set_msg(string msg);
	int set_ip_port(unsigned int ip,unsigned short port);
	unsigned int get_ip();
	unsigned short get_port();
	int action() ;
protected:
	unsigned int _ip;
	unsigned short _port;
	string _msg;
};

#endif //__MSG_CMD_BASE_H

