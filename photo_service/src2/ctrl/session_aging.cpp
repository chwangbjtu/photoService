#include "session_aging.h"
#include "dbg.h"
#include "session_mgr.h"


session_aging::session_aging():_cnt(1)
{
}

session_aging::~session_aging() 
{
}


int session_aging::run(const time_t now)
{
	_cnt++;
	if ( _cnt % 120 != 0)
	{
		return 0;
	}

	_cnt = 1;

    session_mgr::instance()->aging();

	return 0;
}



