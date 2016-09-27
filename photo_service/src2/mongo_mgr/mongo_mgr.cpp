//#include <magic.h>
//#include "client/gridfs.h"
#include <iostream>
#include "dbg.h"
#include "configure.h"
#include "mongo_mgr.h"
using namespace std;
mongo_mgr::mongo_mgr()
{
}
mongo_mgr* mongo_mgr::_inst = NULL;
mongo_mgr* mongo_mgr::instance()
{
    if ( _inst == NULL)
        _inst = new mongo_mgr();
    return _inst;
}
int mongo_mgr::start(int thread_num)
{
    _files_name = configure::instance()->get_dbname() + ".fs.files";
    mongo::client::GlobalInstance * pinstance = new mongo::client::GlobalInstance();
    if (!pinstance->initialized()) 
    {
        //DBG_ERROR("failed to initialize the client driver: %s",instance.status().data());
        cout<<"failed to initialize the client driver: "<<pinstance->status()<<endl;
        return -1;
    }
    int res = -1;
    for ( int i=0;i<thread_num;i++)
    {
		res = -1;
		connection_struct * pconn_struct = new connection_struct();
		vector<string> mongo_rs;
	    configure::instance()->get_mongos_info(mongo_rs);
		for ( int j=0;j<mongo_rs.size();j++)
		{
        	mongo::DBClientConnection* pconn = NULL;
	        mongo::GridFS * pfs = NULL;
    	    if ( connect_mongo(mongo_rs[j],pconn,pfs) && pconn != NULL && pfs != NULL)
        	{
            	res = 0;
				pconn_struct->_lconn.push_back(pconn);
				pconn_struct->_fs.push_back(pfs);
        	}
	        pconn = NULL;
    	    pfs = NULL;
			res = -1;
        	if ( connect_mongo(mongo_rs[j],pconn,pfs,true) && pconn != NULL && pfs != NULL)
        	{
	            res = 0;
				pconn_struct->_lconn_slave.push_back(pconn);
                pconn_struct->_fs_slave.push_back(pfs);
        	}
		}
			pconn_struct->_lconn_index = 0;
			pconn_struct->_lconn_slave_index = 0;
			pconn_struct->_fs_index = 0;
			pconn_struct->_fs_slave_index = 0;
			_conn_mgr.push_back(pconn_struct);
    }
    _myt = magic_open(MAGIC_CONTINUE|MAGIC_ERROR/*|MAGIC_DEBUG*/|MAGIC_MIME_TYPE);
    magic_load(_myt,NULL);
    if ( _conn_mgr.size() < thread_num)
    {
        res = -1;
    }
    return res;
}

int mongo_mgr::store_deal_pic(unsigned int tid,string& oid,string& picdata,int w,int h)
{
    mongo::DBClientConnection* mongo_ins = NULL;
    mongo::GridFS * pfs = NULL;
    get_connector(tid,mongo_ins);
    get_fs(tid,pfs);
    if ( mongo_ins == NULL || pfs == NULL)
    {
        return -1;
    }
    string mime_type;
    {
        fsk::kunique_lock<fsk::kshared_mutex> lock(_mutex);
        const void * pv = (const void*)picdata.data();
        int pic_len = picdata.size();
        if (pic_len > 16)
            pic_len = 16;
        mime_type = magic_buffer(_myt,pv,pic_len);
        if ( mime_type.find("image") == string::npos)
        {
            return -1;
        }
        //magic_close(myt);
    }
    try
    {
        string filename;
        mongo::BSONObj fileobj = pfs->storeFile(picdata.data(),picdata.size(),filename,mime_type);
        string _id = fileobj.getField("_id").OID().toString();
    
        mongo_ins->update(_files_name, mongo::Query(BSON("_id"<<mongo::OID(_id))), BSON("$set"<<BSON("oid"<<oid<<"height"<<h<<"width"<<w)), false, false);
    }
    catch(mongo::DBException& e)
    {
		mongo::DBClientConnection* mongo_ins = NULL;
        mongo::GridFS * pfs = NULL;
        get_connector(tid,mongo_ins,true,false);
        get_fs(tid,pfs,true,false);
        return -1;
    }
    return 0;
}

int mongo_mgr::store_pic(unsigned int tid,string& oid,string& picdata,int w,int h)
{
    //fsk::kunique_lock<fsk::kshared_mutex> lock(_mutex);
    mongo::DBClientConnection* mongo_ins = NULL;
    mongo::GridFS * pfs = NULL;
    get_connector(tid,mongo_ins);
    get_fs(tid,pfs);
    if ( mongo_ins == NULL || pfs == NULL)
    {
        return -1;
    }
    string mime_type;
    {
        fsk::kunique_lock<fsk::kshared_mutex> lock(_mutex);
        const void * pv = (const void*)picdata.data();
        int pic_len = picdata.size();
        if (pic_len > 16)
            pic_len = 16;
        //mime_type = magic_buffer(myt,(const void *)(picdata.data()),picdata.size());
        mime_type = magic_buffer(_myt,pv,pic_len);
        if ( mime_type.find("image") == string::npos)
        {
            return -1;
        }
        //magic_close(myt);
    }
    try
    {
        //fsk::kunique_lock<fsk::kshared_mutex> lock(_mutex);
        string filename;
        mongo::BSONObj fileobj = pfs->storeFile(picdata.data(),picdata.size(),filename,mime_type);
        oid = fileobj.getStringField("md5");
        string _id = fileobj.getField("_id").OID().toString();
    
        //test.fs.files
        mongo_ins->update(_files_name, mongo::Query(BSON("_id"<<mongo::OID(_id))), BSON("$set"<<BSON("oid"<<oid<<"height"<<0<<"width"<<0)), false, false);
    }
    catch(mongo::DBException& e)
    {
    	mongo::DBClientConnection* mongo_ins = NULL;
    	mongo::GridFS * pfs = NULL;
    	get_connector(tid,mongo_ins,true,false);
    	get_fs(tid,pfs,true,false);
        /*
        if ( _mongo_ins->isFailed())
        {
            connect_mongo();
        }
        */
        return -1;
    }
    return 0;
}
int mongo_mgr::get_pic_info(unsigned int tid,std::string& oid,int& width,int& height,std::string& content_type,vector<unsigned char>& pic)
{
    //fsk::kunique_lock<fsk::kshared_mutex> lock(_mutex);
    mongo::BSONObjBuilder builder;
    if (oid.size() == 0)
    {
        return -1;
    }
    builder<<"oid"<<oid;
    builder << "width"<<width;
    builder << "height"<<height;
    mongo::BSONObj bid = builder.obj();
    int res = get_pic(tid,bid,content_type,pic);
    return res;
}

int mongo_mgr::get_pic_info(unsigned int tid,std::string& oid,int& width,int& height,std::string& content_type,std::string& pic)
{
    //fsk::kunique_lock<fsk::kshared_mutex> lock(_mutex);
    mongo::BSONObjBuilder builder;
    if (oid.size() == 0)
    {
        return -1;
    }
    builder<<"oid"<<oid;
    builder << "width"<<width;
    builder << "height"<<height;
    mongo::BSONObj bid = builder.obj();
    int res = get_pic(tid,bid,content_type,pic);
    return res;
}
bool mongo_mgr::findfile(unsigned int tid,std::string oid,int width,int height)
{
    //fsk::kunique_lock<fsk::kshared_mutex> lock(_mutex);
    bool res = false;
    
    if (oid.size() == 0)
    {
        return res;
    }
	int step = 0;
    try
    {
        mongo::BSONObjBuilder builder;
        builder<<"oid"<<oid;
        builder << "width"<<width;
        builder << "height"<<height;
        mongo::BSONObj bid = builder.obj();
    
        string info = bid.toString();
        mongo::GridFS * pfs = NULL;
        get_fs(tid,pfs,false,true);//newconn = false,slaveok==true
        mongo::GridFile gf = pfs->findFile(bid);
        //mongo::GridFile gf = pfs->findFile(BSON("oid"<<oid<<"height"<<height<<"width"<<width));
        if (gf.exists() )
        {
            res = true;
        }
        else
        {
            mongo::GridFS * pfs = NULL;
            get_fs(tid,pfs,false,false);
			step = 1;
            mongo::GridFile gf = pfs->findFile(bid);
            if (gf.exists() )
            {
                res = true;
            }
        }
    }
    catch(mongo::DBException& e)
    {
    	mongo::DBClientConnection* mongo_ins = NULL;
    	mongo::GridFS * pfs = NULL;
		if ( step == 0)
		{
    		get_connector(tid,mongo_ins,true,true);
    		get_fs(tid,pfs,true,true);
		}
		else
		{
    		get_connector(tid,mongo_ins,true,false);
    		get_fs(tid,pfs,true,false);
		}
        res = false;
    }
    return res;
}
int mongo_mgr::get_pic(unsigned int tid, mongo::BSONObj& bid,std::string& content_type,vector<unsigned char>& pic)
{
    int step = 0;
    try
    {
        mongo::GridFS * pfs = NULL;
        get_fs(tid,pfs,false,true);//nweconn==false,slave==true
        mongo::GridFile gf = pfs->findFile(bid);
    
        if (gf.exists() )
        {
            unsigned long long size = 0;
            size = gf.getContentLength();
            
            int chunknum = gf.getNumChunks();
            for(int i=0;i<chunknum;i++)
            {
                mongo::GridFSChunk chunk = gf.getChunk(i);
                int clen = chunk.len();
                //pic.append(chunk.data(clen),clen);
                pic.insert(pic.end(),(chunk.data(clen)),(chunk.data(clen)+clen));
            }
            
            content_type = gf.getContentType();
        }
        else// not find pic
        {
            mongo::GridFS * pfs = NULL;
            get_fs(tid,pfs,false,false);
			int step = 1;
            mongo::GridFile gf = pfs->findFile(bid);
        
            if (gf.exists() )
            {
                unsigned long long size = 0;
                size = gf.getContentLength();
                
                int chunknum = gf.getNumChunks();
                for(int i=0;i<chunknum;i++)
                {
                    mongo::GridFSChunk chunk = gf.getChunk(i);
                    int clen = chunk.len();
                    //pic.append(chunk.data(clen),clen);
                    pic.insert(pic.end(),(chunk.data(clen)),(chunk.data(clen)+clen));
                }
                content_type = gf.getContentType();
            }
            else
            {
                return -1;
            }
        }
    }
    catch(mongo::DBException& e)    
    {   
    	mongo::DBClientConnection* mongo_ins = NULL;
    	mongo::GridFS * pfs = NULL;
		if ( step == 0)
        {
            get_connector(tid,mongo_ins,true,true);
            get_fs(tid,pfs,true,true);
        }
        else
        {
            get_connector(tid,mongo_ins,true,false);
            get_fs(tid,pfs,true,false);
        }
        return -1;    
    }  
    return 0;
}
int mongo_mgr::get_pic(unsigned int tid, mongo::BSONObj& bid,std::string& content_type,std::string& pic)
{
	int step = 0;
    try
    {
        mongo::GridFS * pfs = NULL;
        get_fs(tid,pfs,false,true);//nweconn==false,slave==true
        mongo::GridFile gf = pfs->findFile(bid);
    
        if (gf.exists() )
        {
            unsigned long long size = 0;
            size = gf.getContentLength();
            
            int chunknum = gf.getNumChunks();
            for(int i=0;i<chunknum;i++)
            {
                mongo::GridFSChunk chunk = gf.getChunk(i);
                int clen = chunk.len();
                pic.append(chunk.data(clen),clen);
            }
            
            content_type = gf.getContentType();
        }
        else// not find pic
        {
            mongo::GridFS * pfs = NULL;
            get_fs(tid,pfs,false,false);
			int step = 1;
            mongo::GridFile gf = pfs->findFile(bid);
        
            if (gf.exists() )
            {
                unsigned long long size = 0;
                size = gf.getContentLength();
                
                int chunknum = gf.getNumChunks();
                for(int i=0;i<chunknum;i++)
                {
                    mongo::GridFSChunk chunk = gf.getChunk(i);
                    int clen = chunk.len();
                    pic.append(chunk.data(clen),clen);
                }
                content_type = gf.getContentType();
            }
            else
            {
                return -1;
            }
        }
    }
    catch(mongo::DBException& e)    
    {   
    	mongo::DBClientConnection* mongo_ins = NULL;
    	mongo::GridFS * pfs = NULL;
		if ( step == 0)
        {
            get_connector(tid,mongo_ins,true,true);
            get_fs(tid,pfs,true,true);
        }
        else
        {
            get_connector(tid,mongo_ins,true,false);
            get_fs(tid,pfs,true,false);
        }
        return -1;    
    }  
    return 0;
}
bool mongo_mgr::connect_mongo(string& host,mongo::DBClientConnection*& pconn,mongo::GridFS * &pfs,bool slaveok)
{
    std::string errmsg;
   
    //mongodb://siv:siv@125.88.157.197:27017/?authMechanism=SCRAM-SHA-1&authSource=zyx_test_3&replicaSet=replset&readPreference=secondaryPreferred
    stringstream ss;
    ss<<"mongodb://";
    ss<<configure::instance()->get_mongo_username()<<":"<<configure::instance()->get_mongo_pwd();
    ss<<"@"<<host;
    ss<<"/?authMechanism=SCRAM-SHA-1&authSource="<<configure::instance()->get_dbname();
    if ( slaveok)
    {
        ss<<"&readPreference=secondaryPreferred";
    }
    string cstr = ss.str();
    mongo::ConnectionString cs = mongo::ConnectionString::parse( cstr.data() , errmsg );
    if ( ! cs.isValid() ) 
    {
        DBG_ERROR("error parsing url: %s",errmsg.data());
        return false;
    }
    pconn = static_cast<mongo::DBClientConnection*>( cs.connect( errmsg ) );
    if ( ! pconn ) 
    {
        DBG_ERROR("error connecting: %s",errmsg.data());
        return false;
    }
    pfs = new mongo::GridFS(*pconn,configure::instance()->get_dbname());
    return true;
}
int mongo_mgr::get_connector(unsigned int tid,mongo::DBClientConnection*& pconn,bool newconn,bool slaveok)
{
	if ( tid < 0 || tid > _conn_mgr.size())
	{
		return -1;
	}
	if ( newconn)
	{
		if ( slaveok)
		{
			_conn_mgr[tid]->_lconn_slave_index++;
			if ( _conn_mgr[tid]->_lconn_slave_index >= _conn_mgr[tid]->_lconn_slave.size())
			{
				_conn_mgr[tid]->_lconn_slave_index = 0;
			}
			pconn = _conn_mgr[tid]->_lconn_slave[_conn_mgr[tid]->_lconn_slave_index];
		}
		else
		{
			_conn_mgr[tid]->_lconn_index++;
			if ( _conn_mgr[tid]->_lconn_index >= _conn_mgr[tid]->_lconn.size())
			{
				_conn_mgr[tid]->_lconn_index = 0;
			}
			pconn = _conn_mgr[tid]->_lconn_slave[_conn_mgr[tid]->_lconn_index];
		}
	}
	else
	{
		if ( slaveok)
        {
            pconn = _conn_mgr[tid]->_lconn_slave[_conn_mgr[tid]->_lconn_slave_index];
        }   
        else
        {   
            pconn = _conn_mgr[tid]->_lconn_slave[_conn_mgr[tid]->_lconn_index];
        }
	}
    return 0;
}
int mongo_mgr::get_fs(unsigned int tid,mongo::GridFS * &pfs,bool newconn,bool slaveok/*=false*/)
{
	if ( tid < 0 || tid > _conn_mgr.size())
	{
		return -1;
	}
	if ( newconn)
	{
		if ( slaveok)
		{
			_conn_mgr[tid]->_fs_slave_index++;
			if ( _conn_mgr[tid]->_fs_slave_index >= _conn_mgr[tid]->_fs_slave.size())
			{
				_conn_mgr[tid]->_fs_slave_index = 0;
			}
			pfs = _conn_mgr[tid]->_fs_slave[_conn_mgr[tid]->_fs_slave_index];
		}
		else
		{
			_conn_mgr[tid]->_fs_index++;
			if ( _conn_mgr[tid]->_fs_index >= _conn_mgr[tid]->_fs.size())
			{
				_conn_mgr[tid]->_fs_index = 0;
			}
			pfs = _conn_mgr[tid]->_fs_slave[_conn_mgr[tid]->_fs_index];
		}
	}
	else
	{
		if ( slaveok)
        {
            pfs = _conn_mgr[tid]->_fs_slave[_conn_mgr[tid]->_fs_slave_index];
        }   
        else
        {   
            pfs = _conn_mgr[tid]->_fs_slave[_conn_mgr[tid]->_fs_index];
        }
	}
    return 0;
}
