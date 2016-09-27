#ifndef __VISITOR_HANDLER_H
#define __VISITOR_HANDLER_H
#include "tcp_service.h"
//#include "http_request.h"
//#include "http_response.h"
#include <string>
#include <iostream>
using namespace std;

class visitor_handler : public netsvc::epoll_handler
{
public:
    visitor_handler();
    virtual ~visitor_handler(void);

    virtual int handle_open(void *arg);
    virtual int handle_send();
    virtual int handle_recv();
    virtual int handle_close();

    virtual int handle_run(time_t tm);
private:
    int parse_header(string::size_type position);
    int process_body();

private:
    string _req;
    string _recv;
    msg_cmd_base * _pcmd;

    //fs::request_t _http_req;// struct of request http_header

    unsigned int _send_pos;//send position
    //long long _msg_length;
    //long long  _header_length;
    int _ret;
    //int _retry_num;
};

#endif//__VISITOR_HANDLER_H


