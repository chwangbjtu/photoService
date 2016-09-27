#include<iostream>
#include<sstream>
#include <arpa/inet.h>
#include <netdb.h>
#include<sstream>	
#include "k_str.h"
#include "dbg.h"
#include "configure.h"
#include "tlogger.h"


using namespace std;
tlogger * tlogger::_inst = NULL;
tlogger * tlogger::instance ()
{
	if (_inst == NULL)
		_inst = new tlogger ();
	return _inst;
}

tlogger::tlogger ()
{
}

int tlogger::log(string info)
{
	//boost::mutex::scoped_lock lock(_mutex);
    fsk::kscope_lock<fsk::kunique_mutex> lck(_mutex);
	std::stringstream ss;
	ss<<time(NULL)<<","<<info;
	string tinfo = ss.str();

	_tlog->log("%s\n",tinfo.data());
	return 0;
}

int tlogger::flush()
{
	//boost::mutex::scoped_lock lock(_mutex);
    fsk::kscope_lock<fsk::kunique_mutex> lck(_mutex);
	_tlog->flush();
	return 0;
}

int tlogger::log(string taskid,int errno1)
{
    //boost::mutex::scoped_lock lock(_mutex);
    fsk::kscope_lock<fsk::kunique_mutex> lck(_mutex);
    std::stringstream ss;
    ss<<time(NULL)<<","<<taskid<<","<<errno1;
    string tinfo = ss.str();

    _tlog->log("%s\n",tinfo.data());
    return 0;
}

int tlogger::start()
{
	int log_size_mb = 50;
    string log_path = configure::instance()->get_log_path();
	_tlog = new fsk::time_cycled_logger("photoserver_%Y%m%d_%H%M%S.log",fsk::LDAY, new fsk::writer_file(log_path));
	if (!_tlog)
	{
		cout<<"NULL POINTER _wtr or _log"<<endl;
		return -1;
	}

	_tlog->set_layout(new fsk::layout_pattern("[%t][%T][%l] %m [%L]\r\n"));
	_tlog->open();
	
	return 0;
}



