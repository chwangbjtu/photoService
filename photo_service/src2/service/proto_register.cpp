#include "proto_register.h"

#include "fp_upload_req_v1_handler.h"
#include "fp_download_req_v1_handler.h"
#include "fp_tover_req_v1_handler.h"
#include "proto_dispatcher.h"
#include "proto_constant.h"

bool proto_register::init = false;

int proto_register::start()
{
	if ( !init)
	{
		init = true;

        //proto_dispatcher::instance()->reg("/parse",new fp_parse_req_v1_handler());
        proto_dispatcher::instance()->reg("/upload",new fp_upload_req_v1_handler());
        proto_dispatcher::instance()->reg("/download",new fp_download_req_v1_handler());
        proto_dispatcher::instance()->reg("/transcode",new fp_tover_req_v1_handler());
		
	}
	return 0;
}

