#include <map>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include "json/json.h"
#include "dbg.h"
#include "http_request.h"
#include "http_response.h"
#include "util.h"
#include "configure.h"
#include "mongo_mgr.h"
#include "msg_manager.h"
#include "msg_cmd_base.h"
#include "fp_download_req_v1_handler.h"
#include "tlogger.h"
using  fs::http_response;
using  fs::http_request;
int fp_download_req_v1_handler::process(const fs::http_request& hr,unsigned int ip,unsigned int tid,string& oid,int& width,int& height,string& resp)
{
    int res = 0;
    //hr._req._content
    string info = hr._req._content;

    //string task_id;
    //int width = 0;
    //int height = 0;
    width = 0;
    height = 0;

    if (hr._req._map_params.size() != 3)
    {
        return -1;
    }
    
    multimap<string, string>::const_iterator iter = hr._req._map_params.find("oid");
    if ( iter != hr._req._map_params.end())
    {
        oid = iter->second;
    }
    else
    {
        return -1;
    }

    iter = hr._req._map_params.find("w");
    string swidth;
    if ( iter != hr._req._map_params.end())
    {
        swidth = iter->second;
        if (is_digit(swidth) != 0)
        {
            return -1;
        }
        width = atoi(swidth.data());
    }
    else
    {
        return -1;
    }

    iter = hr._req._map_params.find("h");
    string sheight;
    if ( iter != hr._req._map_params.end())
    {
        sheight = iter->second;
        if (is_digit(sheight) != 0)
        {
            return -1;
        }
        height = atoi(sheight.data());
    }
    else
    {
        return -1;
    }


    string content_type;
    string pic;
    //vector <unsigned char> pic;
    if (mongo_mgr::instance()->findfile(tid,oid,width,height))
    {
        res = mongo_mgr::instance()->get_pic_info(tid,oid,width,height,content_type,pic);
    }
    //if (db_mgr::instance()->findfile(tid,oid,width,height))
    //{
     //   res = db_mgr::instance()->get_pic_info(tid,oid,width,height,content_type,pic);
    //}
    else
    {
        res = -1;
    }

    //find
    if (res == 0)
    {
        http_response http_resp;
	//char *p=(char *)&pic[0];
        //http_resp._resp._content.assign(p,pic.size());
        http_resp._resp._content = pic;
        if(http_resp.pack(resp,content_type,configure::instance()->get_http_cache_time()) < 0)
        { 
                return -1;
        }
        return 0;
    }//send info to trans
    else
    {
	
		/*------------------------------------------------------*/
		if (width <=0 || height <=0 || width>20000 || height>20000)
		{
			return -1;
		}
		vector<unsigned char> ori_pic;
		int ret = 0;
		int w = 0;
		int h = 0;
		if (mongo_mgr::instance()->findfile(tid,oid,w,h))
		//if (db_mgr::instance()->findfile(tid,oid,w,h))
		{
			ret = mongo_mgr::instance()->get_pic_info(tid,oid,w,h,content_type,ori_pic);
			//ret = db_mgr::instance()->get_pic_info(tid,oid,w,h,content_type,ori_pic);
			if (-1 == ret)
			{
				tlogger::instance()->log("get_pic_info return value error.");
				return -1;
			}
		}
		else
		{
			return -1;
		}
		if(0 == ori_pic.size())
		{
			tlogger::instance()->log("get_pic_info return pic lenth is 0.");
			return -1;
		}
		string loginfo = "oid: " + oid + "; content_type: " + content_type;
		//cout<<loginfo<<endl;
		std::size_t pos = content_type.find("image");
		if (0 != pos)
		{
			//string loginfo = "oid: " + oid + "; content_type: " + content_type;
			tlogger::instance()->log("content_type error.");
			tlogger::instance()->log(loginfo);
			return -1;
		}
		tlogger::instance()->log(loginfo);
		string new_pic;
		http_response myresp;
		//int iRet = deal_image_mgr::instance()->deal(tid,ori_pic,width,height,content_type,new_pic);
		int iRet = 0;
		myresp._resp._content = new_pic	;
		if (0 == iRet)
		{
			//write mongodb
			if(configure::instance()->get_en_write_mongo())
			{
				if ( !mongo_mgr::instance()->findfile(tid,oid,width,height))
				{
					 mongo_mgr::instance()->store_deal_pic(tid,oid,new_pic,width,height);
				}
				//if ( !db_mgr::instance()->findfile(tid,oid,width,height))
				//{
				//	 db_mgr::instance()->store_deal_pic(tid,oid,new_pic,width,height);
				//}
			}
			if(myresp.pack(resp,content_type,configure::instance()->get_http_cache_time()) < 0)
			{
				return -1;
			}
			return 0;
		}
		else
		{
			return -1;
		}	

	}

	return 0;
}

