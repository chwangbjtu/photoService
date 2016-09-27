#include <stdlib.h>
#include "dbg.h"
#include "msg_cmd_base.h"
#include "msg_manager.h"
#include "visitor.h"

visitor* visitor::_instance = NULL;

visitor::visitor()
{   
}

visitor::~visitor()
{
}

visitor* visitor::instance()
{
    if ( _instance == NULL ) 
    {
        _instance = new visitor();
    }
    return _instance;
}

int visitor::start()
{
    if(pthread_create(&_thread_connector, NULL, connect_thread, this) != 0)
    {
        return -1;
    }
    
    return 0;
}

void* visitor::connect_thread(void *arg)
{
    tconnector<visitor_handler> * _pconnector = new tconnector<visitor_handler>;
    int err = _pconnector->start(1);
    if(err != 0)
    {
        cout<<"start connector failed."<<endl;
        exit(-1);
    }

    while(true)
    {
        msg_cmd_base * pcmd = NULL;
        int res = msg_manager::instance()->peek(pcmd);
        time_t now = time(NULL);
        if ( res == 0 && pcmd != NULL)
        {
            _pconnector->connect(pcmd->get_ip(),pcmd->get_port(),120,(void*)pcmd);
        }
        else
        {
            usleep(500000);
        }
        
    }

    pthread_exit(0);
    return 0;
}


