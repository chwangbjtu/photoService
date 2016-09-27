#ifndef __SESSION_MGR_H
#define __SESSION_MGR_H

#include <string>
#include <map>
#include "kmutex.h"

#include "session_struct.h"

using namespace std;

class session_mgr
{
public:
	~session_mgr(){}
	static session_mgr* instance();
public:
    int get_resp(string& task_id);
    int put_resp_status(string& task_id,int status);
	int aging();
private:
	session_mgr();
	static session_mgr* _inst;

    multimap<string,session_struct*> _task;
    //map<string,int> _task;
    fsk::kunique_mutex _mutex;
};

#endif//__SESSION_MGR_H



