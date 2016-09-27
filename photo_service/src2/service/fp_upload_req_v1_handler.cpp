#include <sstream>
#include <iomanip>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/md5.h>
#include "json/json.h"
#include "dbg.h"
#include "mongo_mgr.h"
#include "fp_upload_req_v1_handler.h"
using namespace std;

int fp_upload_req_v1_handler::process(const fs::http_request& hr,unsigned int ip,unsigned int tid,string& obj_id,int& width,int& height,string& resp)
{
    int res = 0;
    //hr._req._content

    string info = hr._req._content;

    std::string oid;
    width = 0;
    height = 0;

    if ( hr._req._map_params.size() != 0)
    {
        return -1;
    }

    /*
    multimap<string, string>::const_iterator iter = hr._req._map_params.find("width");

    if ( iter != hr._req._map_params.end())
    {
        string swidth = iter->second;
        width = atoi(swidth.data());
    }

    iter = hr._req._map_params.find("height");
    if ( iter != hr._req._map_params.end())
    {
        string sheight = iter->second;
        height = atoi(sheight.data());
    }
    
    */
    

    unsigned char *pData = (unsigned char *)(info.data());
    unsigned char md5[17]={0};
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, pData, info.size());
    MD5_Final(md5,&c);

    stringstream ss;
    for(int i = 0; i < 16; i++)
        ss<< hex << setw(2) << setfill('0') << (int)md5[i];
    oid = ss.str();

    if ( !mongo_mgr::instance()->findfile(tid,oid,width,height))
    //if ( !db_mgr::instance()->findfile(tid,oid,width,height))
    {
        //res = mongo_mgr::instance()->store_pic(tid,obj_id,info);//obj_id is no use
        res = mongo_mgr::instance()->store_pic(tid,oid,info);
        //res = db_mgr::instance()->store_pic(tid,oid,info);
    }
    
    Json::Value root;
    Json::FastWriter lwriter;
        
    root["oid"] = oid;
    if ( res ==0 )
    {
        root["error"] = "0";
    }
    else
    {
        root["error"] = "1";
    }
        
    string msg = lwriter.write(root);

    pack(msg,resp);

    /*
    Json::Reader reader;
    Json::Value value;
    if ( reader.parse(info,value))
    {
        if ( value.isMember("task_id") && !value["task_id"].isNull()
            && value["task_id"].isString())
        {
             task_id = value["task_id"].asString();
        }

        if ( value.isMember("width") && !value["width"].isNull()
            && value["width"].isString())
        {
             width = value["width"].asString();
        }

        if ( value.isMember("height") && !value["height"].isNull()
            && value["height"].isString())
        {
             height = value["height"].asString();
        }
    }
    */

    //hr._req._map_params
   
    
    //redis_mgr::instance()->get_data(vid,data);
	return 0;
}

