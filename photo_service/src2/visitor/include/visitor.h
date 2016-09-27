#ifndef __VISITOR_H
#define __VISITOR_H

#include <vector>
#include "visitor_handler.h"
#include "tconnector.h"

using namespace std;

class visitor
{
public:
    ~visitor();

    static visitor* instance();
    int start();
private:
    visitor();
    static visitor* _instance;
    
    pthread_t _thread_connector;
    
    static void * connect_thread(void *arg);
};

#endif//__VISITOR_H


