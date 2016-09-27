#include "configure.h"
#include "msg_cmd_base.h"

msg_cmd_base::msg_cmd_base(string msg)
{
	_msg = msg;

    //_ip = configure::instance()->get_trans_ip();
    //_port = configure::instance()->get_trans_port();
		
}

msg_cmd_base::~msg_cmd_base()
{
}


string msg_cmd_base::get_msg()
{
	return _msg;
}
	
void msg_cmd_base::set_msg(string msg)
{
	_msg = msg;
}
	
int msg_cmd_base::set_ip_port(unsigned int ip,unsigned short port)
{
	_ip=ip;
	_port=port;
	return 0;
}

unsigned int msg_cmd_base::get_ip()
{
	return _ip;
}
	
unsigned short msg_cmd_base::get_port()
{
	return _port;
}
	

int msg_cmd_base::action() 
{
	return 0;
}


