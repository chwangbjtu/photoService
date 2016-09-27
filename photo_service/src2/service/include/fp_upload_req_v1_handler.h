#ifndef __FP_UPLOAD_REQ_V1_HANDLER_H
#define __FP_UPLOAD_REQ_V1_HANDLER_H

#include "fp_base_handler.h"

class fp_upload_req_v1_handler: public fp_base_handler
{
public:
	virtual int process(const fs::http_request& hr,unsigned int ip,unsigned int tid,string& obj_id,int& width,int& height,string& resp);
private:
	//int unpack(const multimap<string,string>& req,android_req_struct_t& req_struct);
};

#endif//__FP_UPLOAD_REQ_V1_HANDLER_H

