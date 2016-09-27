#include"dbg.h"
#include "msg_cmd_base.h"
#include "msg_manager.h"
#include "visitor_handler.h"

//using fs::http_response;

visitor_handler::visitor_handler()
{
    _ret = -1;
    _send_pos = 0;
    _pcmd = NULL;
}

visitor_handler::~visitor_handler()
{
    delete _pcmd;
    _pcmd = NULL; 

}

int visitor_handler::handle_open(void *arg)
{
    _pcmd = (msg_cmd_base*)arg;
    _req = _pcmd->get_msg();
    set_timeout(20);

    return 0;
}

int visitor_handler::handle_send(void) // whether to return -1
{
    ssize_t ret=0;

    if (_send_pos >= _req.size())
    {
        return 0;
    }

    while ((ret = send(_req.c_str() + _send_pos, _req.size() - _send_pos, 0) ) > 0)
    {
        _send_pos += ret;
        if (_send_pos >= _req.size())
        {
            break;
        }
    }
    if ((ret < 0 && errno != EWOULDBLOCK && errno != EAGAIN) || ret == 0)
    {
        return -1;
    }

    return 0;
}

int visitor_handler::handle_recv(void)
{
    char buf[1024] = {0};
    ssize_t recv_len = 0;
    string::size_type position;

    while ((recv_len = recv(buf,1024,0)) > 0)
    {
        _recv.append(buf,recv_len);
    }
    if ((recv_len < 0 && errno != EWOULDBLOCK && errno != EAGAIN) || recv_len == 0 && _recv.empty())
    {
        return -1;
    }

    //http_response resp;
    //fs::response_t resp_t;

    if ((position = _recv.find("\r\n\r\n")) != _recv.npos)
    {
        /*
        //int res = resp.parse(_recv.data(),_recv.size());
        //if ( res == 0)
        {//Content-Length
            multimap<string, string>::iterator iter = resp._map_headers.begin();
            
            iter = resp._map_headers.find("Content-Length");
            if ( iter != resp._map_headers.end())
            {
                int len = atoi(iter->second.data());
                if ( position + len >= _recv.size())
                {
                    return -1;
                }
            }
            else
            {
                return -1;
            }
        return -1;
        }
        */
    }
    else
    {
        //MAX_HEADER_SIZE
        if ( 8192 < _recv.size())
        {
            return -1;
        }
    }
    
    return 0;
}

int visitor_handler::handle_close(void)
{
    delete _pcmd;
    _pcmd = NULL;

    return -1;
}

int visitor_handler::handle_run(time_t tm)
{
    return 0;
}

int visitor_handler::parse_header(string::size_type position)
{
    return 0;
}

int visitor_handler::process_body()
{
}



