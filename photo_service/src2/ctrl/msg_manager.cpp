#include "msg_manager.h"

msg_manager* msg_manager::_instance = NULL;

msg_manager::msg_manager()
{
}

msg_manager::~msg_manager()
{
}

msg_manager* msg_manager::instance()
{
	if ( _instance == NULL ) 
	{
		_instance = new msg_manager();
	}
	return _instance;
}

int msg_manager::dispatch(msg_cmd_base* cmd)
{
	int ret = 0;
	ret = _queue.post(cmd);
	return ret;
}

int msg_manager::peek(msg_cmd_base* &cmd)
{
	cmd = NULL;
	int recv_ret = 0;
	cmd = NULL;
	recv_ret = _queue.peek((void*&)cmd, 5);

	return recv_ret;
}

int msg_manager:: start()
{
	return 0;
}

int msg_manager::size()
{
	return _queue.size();
}

