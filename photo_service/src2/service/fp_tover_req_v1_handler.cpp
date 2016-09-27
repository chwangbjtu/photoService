#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "json/json.h"
#include "dbg.h"
#include "util.h"
#include "session_mgr.h"
#include "fp_tover_req_v1_handler.h"

int fp_tover_req_v1_handler::process(const fs::http_request& hr,unsigned int ip,unsigned int tid,string& oid,int& width,int& height,string& resp)
{
    int res = 0;
    string info = hr._req._content;

    if (hr._req._map_params.size() != 0)
    {
        return -1;
    }

    string task_id;
    string error;

    Json::Reader reader;
    Json::Value value;
    if ( reader.parse(info,value) && value.type() == Json::objectValue)
    {
        if ( value.isMember("oid") && !value["oid"].isNull() 
            && value["oid"].isString())
        {
             oid = value["oid"].asString();
        }

        if ( value.isMember("error") && !value["error"].isNull()
            && value["error"].isString())
        {
             error = value["error"].asString();
        }

        string swidth;
        string sheight;

        if ( value.isMember("w") && !value["w"].isNull()
            && value["w"].isString())
        {
             swidth = value["w"].asString();
             width = atoi(swidth.data());
        }

        if ( value.isMember("h") && !value["h"].isNull()        
            && value["h"].isString())
        {
             sheight = value["h"].asString();
             height = atoi(sheight.data());
        }

        if ( oid.size() == 0 || swidth.size() == 0 || sheight.size() == 0)
        {
            return -1;
        }

        task_id = oid + "_" + swidth + "_" + sheight;
    }

    if (task_id.size() > 0 && error.size() > 0)
    {
        if ( error == "0")
        {
            session_mgr::instance()->put_resp_status(task_id,0);
        }
        //else if ( error == "1")
        else
        {
            session_mgr::instance()->put_resp_status(task_id,1);
        }
    }
    
    string msg = util::instance()->get_ok_resp();
    if ( task_id.size() == 0)
    {
        msg = util::instance()->get_error_resp();
    }

    pack(msg,resp);

	return 0;
}

