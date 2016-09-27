#ifndef __MSG_MANAGER_H
#define __MSG_MANAGER_H

#include <pthread.h>  
#include <vector>
#include <map>
#include "cmdq.h"
#include "msg_cmd_base.h"

using namespace std;
class msg_manager
{
public:
	~msg_manager();

	static msg_manager* instance();
	int dispatch(msg_cmd_base* cmd );
	int peek(msg_cmd_base* &cmd);
	int start();
	int size();
private:
	msg_manager();
	static msg_manager* _instance;
	//map<string,msg_cmd_maze *> _queue;
	pthread_mutex_t _mutex ; 
	fsk::cmdq			_queue;
	
};

#endif//__MSG_MANAGER_H

