#include "dbg.h"
#include "session_mgr.h"

session_mgr::session_mgr(){}

session_mgr* session_mgr::_inst = NULL;

session_mgr* session_mgr::instance()
{
    if ( _inst == NULL)
        _inst = new session_mgr();
    return _inst;
}

int session_mgr::get_resp(string& task_id)
{
    fsk::kscope_lock<fsk::kunique_mutex> lck(_mutex);
    multimap<string,session_struct*>::iterator iter = _task.find(task_id);
    //trans error
    int res = -2;
    
    if (iter != _task.end())
    {
        // trans over
        if ( iter->second->_trans_over == 0)
        {
            res = 0;
        }
        else if (iter->second->_trans_over == 1)
        {
            res = -2;
        }
        delete iter->second;
        _task.erase(iter++);
    }
    else//not trans over
    {
        res = -1;
    }
   

    return res;
}

int session_mgr::put_resp_status(string& task_id,int status)
{
    fsk::kscope_lock<fsk::kunique_mutex> lck(_mutex);
    session_struct * ps = NULL;
    ps = new session_struct;
    ps->_trans_over = status;
    ps->_create_time = time(NULL);
    _task.insert(make_pair(task_id,ps));
    /*
    map<string,int>::iterator iter = _task.find(task_id);
    
    if ( iter != _task.end())
    {
        //if (iter->second == 0)
        if (status == 0)
        {
            iter->second = status;
        }
    }
    else
    {
        _task.insert(make_pair(task_id,status));
    }
    */
    return 0;
}

int session_mgr::aging()
{
    fsk::kscope_lock<fsk::kunique_mutex> lck(_mutex);
    time_t now = time(NULL);
    time_t new_now = now + 60*10;//should read from config
    
    multimap<string,session_struct*>::iterator iter = _task.begin();
    for(;iter != _task.end();)
    {
        if ( iter->second->_create_time > new_now )
        {
            delete iter->second;
            _task.erase(iter++);
        }
        else
        {
            _task.erase(iter++);
        }
    }

    return 0;
}
