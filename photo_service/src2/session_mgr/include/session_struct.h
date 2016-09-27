#ifndef __SESSION_STRUCT_H
#define __SESSION_STRUCT_H

#include <iostream>

using namespace std;

class session_struct
{
public:
    session_struct()
    {
        _trans_over = 0;
        _create_time = time(NULL);
    }

public:
    int _trans_over;
    time_t _create_time;
};

#endif//__SESSION_STRUCT_H
