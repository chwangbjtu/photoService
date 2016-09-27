#include <sys/time.h>
#include <sstream>
#include "k_str.h"
#include "tcp_netio.h"
#include "dbg.h"
#include "util.h"
#include "session_mgr.h"
#include "mongo_mgr.h"
#include "proto_dispatcher.h"
#include "http_response.h"
#include "configure.h"
#include "tlogger.h"
using namespace std;

tcp_netio::tcp_netio():
	_snd_len(0),
	//_ctime(time(NULL)),
	_proto_len(0),
	_content_length(-1),
	_header_length(0)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    _ctime = tv.tv_sec * 1000 + tv.tv_usec/1000;
	_timeout = configure::instance()->get_http_timeout();
    _width = 0;
    _height = 0;
}

tcp_netio::~tcp_netio()
{
}
int tcp_netio::handle_recv()
{
	const int BUF_LEN = 2048;
	char buf[BUF_LEN] = {0};
	ssize_t recv_len = 0;
	while((recv_len = ::recv(sock(), buf, BUF_LEN, 0)) > 0) 
	{
		_request.append(buf, recv_len);
	}
	if(recv_len < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) 
	{
		return -1;
	} 
	else if(recv_len == 0 && _request.empty()) 
	{
		return -1;
	}
 
	if ( recv_finished())
	{
		if(proto_dispatcher::instance()->process(_request, peer_ip(),thread_index() ,_oid,_width,_height,_response) < 0) 
		{
			return -1;//close socket
		} 
		else
		{
            stringstream ss;
            ss<<_oid<<"_"<<_width<<"_"<<_height;
            _task_id = ss.str();
			return handle_send();
		}
	} 
	else
	{
		return 0;//continue to receive
	}
	return -1;
}

int tcp_netio::handle_open(void *arg)
{
    //_tid = pthread_self();
	return 0;
}

int tcp_netio::handle_send()
{
	if(_response.length() == 0) 
	{
		return 0;
	}
	size_t resp_len = _response.length();
	while (_snd_len < resp_len) 
	{
		ssize_t n = ::send(sock(), _response.data()+_snd_len,resp_len-_snd_len, 0);
		if(n > 0)
		{
			_snd_len += (size_t)n;
		}
		else if(n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK ))
		{
			/* action blocked */
			return 0;
		}
		else 
		{
			break;
		}
	}
	//send all response
	_response.clear();
	return -1;
}
int tcp_netio::handle_close()
{
	return -1;
}

int tcp_netio::handle_run(time_t t)
{
	//if((t - _ctime) > _timeout) 
    struct timeval tv;
    gettimeofday(&tv,NULL);
    unsigned long long tnow = tv.tv_sec * 1000 + tv.tv_usec/1000;
    if (tnow - _ctime > _timeout)
	{
        //_data = util::instance()->get_error_resp();
        tlogger::instance()->log(_task_id,1);
		return -1;
	}
    else
    {
        //redis_mgr::instance()->get_data(_obj_id,_data);
        int res = session_mgr::instance()->get_resp(_task_id);
        //trans over
        if ( res == 0)
        {
		
            if (mongo_mgr::instance()->findfile(thread_index(),_oid,_width,_height))
            {
                mongo_mgr::instance()->get_pic_info(thread_index(),_oid,_width,_height,_content_type,_data);
            }
	    /*
            if (db_mgr::instance()->findfile(thread_index(),_oid,_width,_height))
            {
                db_mgr::instance()->get_pic_info(thread_index(),_oid,_width,_height,_content_type,_data);
            }
	   */
            if (_data.size() == 0)
            {
                tlogger::instance()->log(_task_id,3);//trans error,not find picture
                return -1;
            }
        }
        else if (res == -1)//not trans over
        {
        }
        else if (res == -2)//trans error;
        {
            tlogger::instance()->log(_task_id,2);//trans error
            //_data = util::instance()->get_error_resp();
            return -1;
        }
    }

    if (_data.size() > 0 )
    {
        fs::http_response http_resp;

        http_resp._resp._content = _data;
        int pres = 0;
        if (_content_type.size() > 0)
            pres = http_resp.pack(_response,_content_type);
        else
            pres = http_resp.pack(_response);


        if( pres < 0)
        {
            return -1;
        }

        return handle_send();
    }
	return 0;
}

bool tcp_netio::recv_finished()
{
	if ( _request.compare(0, 4, "GET ", 4) == 0)
	{
		return get_finished();
	} 
	else if (_request.compare(0, 5, "POST ", 5) == 0) 
	{
		return post_finished();
	} 
	else
	{
		return false;
	}
}

int tcp_netio::extract_between(const std::string& data, std::string& result, const std::string& separator1, const std::string& separator2)
{
	std::string::size_type start, limit;

	start = data.find(separator1, 0);

	if ( std::string::npos != start)
	{
		start += separator1.length();
		limit = data.find(separator2, start);
		if ( std::string::npos != limit)
		{
			result = data.substr(start, limit - start);
			return 0;
		} 

	}
	return -1;
}

bool tcp_netio::post_finished()
{
	if ( _content_length <0)
	{
		std::string::size_type pos; 
		pos = _request.find("\r\n\r\n",0);
		if (string::npos == pos )
		{
			return false;
		}
		_header_length = pos +4;
		std::string content_len_str; 
		int ret = 0;
        /*
		string content_lenth = "Content-Length: ";
		ret = extract_between(_request, content_len_str, content_lenth);//in this service http-post-header must have Content-Length:
		if ( ret < 0 )
		{
            return false;
  		} 
        */
        ///
        string content_lenth = "Content-Length: ";
        string content_lenth1 = "Content-length: ";
        ret = extract_between(_request, content_len_str, content_lenth1);
        if (ret < 0)
        {
           ret = extract_between(_request, content_len_str, content_lenth); 
        }
        if (ret < 0)
        {
            return false;
        }
        ///
		if (is_digit(content_len_str) <0) 
		{
			return false;
		}
		_content_length = atoi(content_len_str.c_str());
         }
	if ( (_content_length + _header_length ) > _request.length())
	{
		return false;
	} 
	return true;
}

bool tcp_netio::get_finished()
{
	std::string::size_type pos; 
	pos = _request.rfind("\r\n\r\n");
	if (string::npos == pos ) 
	{
		return false;
	} 
	else 
	{
		return true;
	}
}

int tcp_netio::is_digit(const string & str)
{
	int size = str.length();
	if ( size == 0)
	{
		return -1;
	}
	
	const char DIGIT_MAX = '9';
	const char DIGIT_MIN = '0';
	int i = 0;
	int ret = 0;
	
	while ( i < size )
	{
		if ( str[i] <= DIGIT_MAX && str[i] >= DIGIT_MIN )
		{
			i++;
			continue;
		}
		else 
		{
			ret = -1;
			break;
		}
	}

	return ret;
}




