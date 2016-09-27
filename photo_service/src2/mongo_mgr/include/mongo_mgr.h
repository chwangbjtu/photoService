#ifndef _MONGO_MGR_H
#define _MONGO_MGR_H

#include <list>
#include <magic.h>
#include "client/dbclient.h"
//#include "mongo/client/dbclient.h"
////#include "client/dbclient_rs.h"
////#include "client/gridfs.h"
//#include "client/hostandport.h"
#include "kmutex.h"
#include "klock.h"

using namespace std;

class connection_struct
{
public:
	vector<mongo::DBClientConnection*> _lconn;
	vector<mongo::DBClientConnection*> _lconn_slave;
	int _lconn_index;
	int _lconn_slave_index;

	vector<mongo::GridFS *> _fs;
	vector<mongo::GridFS *> _fs_slave;

	int _fs_index;
	int _fs_slave_index;
};

class mongo_mgr
{
public:
    ~mongo_mgr(){}
    static mongo_mgr* instance();
public:
    int start(int thread_num);

    bool findfile(unsigned int tid,std::string oid,int width,int height);

    int store_pic(unsigned int tid,string& oid,string& data,int w=0,int h=0);
    int store_deal_pic(unsigned int tid,string& oid,string& data,int w=0,int h=0);
    
    int get_pic_info(unsigned int tid,std::string& oid,int& width,int& height,std::string& content_type,std::string& pic);
    int get_pic_info(unsigned int tid,std::string& oid,int& width,int& height,std::string& content_type,vector<unsigned char>& pic);
private:
    bool connect_mongo(string& host,mongo::DBClientConnection*&pconn,mongo::GridFS *& pfs,bool slaveok=false);
    //bool connect_mongo(mongo::DBClientReplicaSet*&pconn,mongo::GridFS *& pfs,bool slaveok=false);

    int get_pic(unsigned int tid,mongo::BSONObj& bid,std::string& content_type,std::string& pic);
    int get_pic(unsigned int tid,mongo::BSONObj& bid,std::string& content_type,vector<unsigned char>& pic);

    int get_connector(unsigned int tid,mongo::DBClientConnection*& pconn,bool newconn=false,bool slaveok=false);
    //int get_connector(unsigned int tid,mongo::DBClientReplicaSet*& pconn,bool slaveok=false);
    int get_fs(unsigned int tid,mongo::GridFS * &pfs,bool newfs = false,bool slaveok=false);
private:
    mongo_mgr();
    static mongo_mgr* _inst;

    string _files_name;
    //mongo::DBClientConnection* _mongo_ins;
    //mongo::GridFS * _fs_ins;

	vector<connection_struct *> _conn_mgr;

	/*
    map<unsigned int,mongo::DBClientConnection*> _connectors;
    //map<unsigned int,mongo::DBClientReplicaSet*> _connectors;
    map<unsigned int,mongo::GridFS *> _fs;

    //map<unsigned int,mongo::DBClientReplicaSet*> _connectors_slave;
    map<unsigned int,mongo::DBClientConnection*> _connectors_slave;
    map<unsigned int,mongo::GridFS *> _fs_slave;
    list<mongo::DBClientConnection*> _lconnectors;
    //list<mongo::DBClientReplicaSet*> _lconnectors;
    list<mongo::GridFS *> _lfs;

    //list<mongo::DBClientReplicaSet*> _lconnectors_slave;
    list<mongo::DBClientConnection*> _lconnectors_slave;
    list<mongo::GridFS *> _lfs_slave;
	*/

    magic_t _myt;
    fsk::kshared_mutex  _mutex;
    /*
    //std::vector<mongo::DBClientConnection*> _online_ins;
    unsigned int _conn_num;
    */
};
#endif
