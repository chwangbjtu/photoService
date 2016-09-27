#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <sstream>

#include "json/json.h"
#include "k_configure.h"
#include "dbg.h"
#include "util.h"
#include "configure.h"

using  fs::k_configure;

configure* configure::_inst = NULL;

configure::configure()
{
}
configure::~configure() {}

configure* configure::instance()
{
	if ( _inst == NULL)
		_inst = new configure();
	return _inst;
}



int configure::start(const string& path)
{
	//read configure file
	k_configure conf;
	if ( conf.open(path.c_str()) != 0) 
	{
		DBG_ERROR("open configure file error");
		return -1;
	}

	cdata data;
	//file system
	conf.get_string_value("file","file_path",data._file_path);
	//enable
	int  write_mongo = 1;
	int  use_mongo = 1;
	conf.get_integer_value("enable","en_write_mongo",write_mongo);
	conf.get_integer_value("enable","en_use_mongo",use_mongo);
	
	if (use_mongo == 1)
	{
	    data._is_use_mongo=true;
	}
	else
	{
	    data._is_use_mongo=false;
 	}

	if (write_mongo == 1)
	{
	    data._is_write_mongo=true;
	}
	else
	{
	    data._is_write_mongo=false;
 	}
	int port = 0;
	//service
	conf.get_integer_value("service","service_port",port);
	data._service_port= port;
	conf.get_integer_value("service","service_worker_num",data._service_worker_num);
	conf.get_integer_value("service","http_timeout",data._http_timeout);
	conf.get_string_value("service","http_cache_time",data._http_cache_time);

    //trans
    /*
    string strans_ip;
    int trans_port = 0;
    conf.get_string_value("trans","trans_ip",strans_ip);
    util::instance()->str2ip(strans_ip,data._trans_ip);
    conf.get_integer_value("trans","trans_port",trans_port);
    data._trans_port = trans_port;
	*/
	//log
	conf.get_string_value("log","log_path",data._log_path);
	conf.get_string_value("log","photo_service_server_ip",data._photo_service_server_ip);
	conf.get_integer_value("log","cutdown_interval",data._cutdown_interval);

    //mongo
    string mongos;
    conf.get_string_value("mongo","mongos",mongos);
    conf.get_string_value("mongo","mongo_username",data._mongo_username);
    conf.get_string_value("mongo","mongo_pwd",data._mongo_pwd);
    conf.get_string_value("mongo","mongo_dbname",data._db_name);
    conf.get_string_value("mongo","mongo_rs_name",data._mongo_rs_name);
    Json::Reader reader;
    Json::Value value;
    if ( reader.parse(mongos,value))
    {
        int size = value.size();

        for ( int i=0;i<size;i++)
        {
            string tmp = value[i].asString();
            data._mongos.push_back(tmp);
        }
    }

    if (data._mongos.size() == 0)
    {
        cout<<"parse mongos error"<<endl;
        return 1;
    }
	
	//check the configure item 
	if ( check_config(data) != 0) 
	{
		DBG_ERROR("check_config() error");
		return -1;
	}

	
	fsk::kunique_lock<fsk::kshared_mutex> lck(_mutex);
	_path = path;
	_data = data;

	print();

	DBG_INFO("success to read configure");
	return 0;
}

int configure::check_config(const cdata& data)
{
	if(
	      data._service_port <= 0
	   || data._service_worker_num <= 0
	   || data._http_timeout <= 0
	   || data._http_cache_time.size() <= 0
	   || data._log_path.size() <= 0
	   || data._photo_service_server_ip.size() <= 0
       /*|| data._trans_ip <= 0
       || data._trans_port <= 0*/
       || data._db_name.size() <= 0
       || data._mongo_username.size() <= 0
       || data._mongo_pwd.size() <= 0
	   ) 
	{
		DBG_ERROR("configure error may be some parameters <= 0");
		return -1;
	}
	if (-1 == access(data._file_path.c_str(),0))
	{
		DBG_ERROR("configure error \"file_path\" does not exist");
		return -1;
	}
	return 0;
}

unsigned short configure::get_service_port() 
{	
	fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
	return _data._service_port;
}

int configure::get_service_worker_num() 
{	
	fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
	return _data._service_worker_num;
}

int configure::get_http_timeout()
{
	fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
	return _data._http_timeout;
}

string configure::get_http_cache_time()
{
	fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
	return _data._http_cache_time;
}

string configure::get_log_path()
{
	fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
	return _data._log_path;
}

int configure::get_log_buf_size()
{
	fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
	return _data._log_buf_size;
}

string configure::get_server_ip()
{
	fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
	return _data._photo_service_server_ip;
}
/*
unsigned int configure::get_trans_ip()
{
    fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
    return _data._trans_ip;
}

unsigned short configure::get_trans_port()
{
    fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
    return _data._trans_port;
}
*/
string configure::get_server_version()
{
	//fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
	return PHOTO_SERVICE_SERVER_VERSION;
}

string configure::get_mongos_info(int num)
{
    fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
    string str;
    if (num < _data._mongos.size())
    {
        str = _data._mongos[num];
    }

    return str;
}

int configure::get_mongos_info(vector<string>& mongos)
{
    fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
    mongos = _data._mongos;
    return 0;
}

string configure::get_mongo_username()
{
    fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
    return _data._mongo_username;
} 

string configure::get_mongo_pwd()
{
    fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
    return _data._mongo_pwd;
} 

string configure::get_dbname()
{
    fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
    return _data._db_name;
}

string configure::get_mongo_rs_name()
{
    fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
    return _data._mongo_rs_name;
}
bool configure::get_en_write_mongo()
{
    fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
    return _data._is_write_mongo;
}

bool configure::get_en_use_mongo()
{
    fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
    return _data._is_use_mongo;
}

string configure::get_file_path()
{
	fsk::kshared_lock<fsk::kshared_mutex> lck(_mutex);
	return _data._file_path;
}

int configure::print()
{
	cout<<"\n=========================configure file=========================\n";
	cout<<"service_port="<<_data._service_port<<endl;
	cout<<"service_worker_num="<<_data._service_worker_num<<endl;
	cout<<"http_timeout="<<_data._http_timeout<<endl;
	cout<<"log_path="<<_data._log_path.data()<<endl;
	cout<<"photo_service_server_ip="<<_data._photo_service_server_ip.data()<<endl;
    cout<<"mongo_db_name="<<_data._db_name<<endl;
    cout<<"mongo_user="<<_data._mongo_username<<endl;
    cout<<"mongo_pwd="<<_data._mongo_pwd<<endl;
    cout<<"mongo_rs_name="<<_data._mongo_rs_name<<endl;
	cout<<"en_write_mongo ="<<_data._is_write_mongo<<endl;
	cout<<"en_use_mongo ="<<_data._is_use_mongo<<endl;
	cout<<"file_path ="<<_data._file_path<<endl;
	cout<<"\n=========================configure file=========================\n";
	
	return 0;
}



