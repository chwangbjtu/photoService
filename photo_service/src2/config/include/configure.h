#ifndef __CONFIGURE_H
#define __CONFIGURE_H
#include <string>
#include <map>
#include <vector>
#include "kmutex.h"
#include "klock.h"

using namespace std;

#define PHOTO_SERVICE_SERVER_VERSION "0.1.0.01"

//configure item
class cdata
{
public:
	cdata():
		_service_port(8080),
		_service_worker_num(4),
		_http_timeout(20),
		_log_buf_size(1024*50),
	    _cutdown_interval(300)
	{
		_log_path = "./log/";
		_photo_service_server_ip = "127.0.0.1";
		_http_cache_time = "0";
	}

	unsigned short _service_port;
	int _service_worker_num;
	int _http_timeout;
	int _log_buf_size;
	int _cutdown_interval;
    //unsigned int _trans_ip;
    //unsigned short _trans_port;
	string _log_path;
	string _photo_service_server_ip;
    vector<string> _mongos;
    
    string _db_name;
    string _mongo_username;
    string _mongo_pwd;
    string _mongo_rs_name;

    string _http_cache_time;
    bool _is_write_mongo;
    bool _is_use_mongo;
    string _file_path;
};


/*
 *read configure at  10s interval and check each item.if error found,exit this
 * programme.
 * */
class configure
{
public:
	~configure();
	static configure* instance();
	/* read configure file
	 *@para[in]file:filename of configure name
	 *@return:
	 *  0:success,-1:failed
	 **/
	int start(const string& file);

	//getter 
	unsigned short get_service_port() ;

	int get_service_worker_num();

	int get_http_timeout();

	int get_queue_num();

	string get_log_path();

    //unsigned int get_trans_ip();
    
    //unsigned short get_trans_port();

	int print();

	int get_log_buf_size();

	string get_server_ip();

	string get_server_version();

    string get_mongos_info(int index);

    int get_mongos_info(vector<string>& mongos);

    string get_mongo_rs_name();

    string get_dbname();
    
    string get_collection();

    string get_mongo_username();

    string get_mongo_pwd();

	string get_http_cache_time();

	bool get_en_write_mongo();
	bool get_en_use_mongo();
	string get_file_path();
private:
	configure();
	/* check the  validity of each item in configure file
	 * @para[in]d: configure file item 
	 * @return:
	 *  0:success,-1: error item existed in configure file
	 * */
	int check_config(const cdata& data);
	
	int str2ip(const string& str,unsigned int& ip);
private:
	string _path;
	cdata _data;
	static configure* _inst;
	fsk::kshared_mutex _mutex;
};

#endif//__CONFIGURE_H

