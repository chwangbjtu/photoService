#ifndef __TCONNECTOR__
#define __TCONNECTOR__

#include <list>
#include <vector>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
//#include "tcp_service.h"
#include "linux/epoll_worker.h"

using namespace netsvc;

template< class handler>
class tconnector
{
public:
    tconnector();
    virtual ~tconnector();

    /*
    *       start connector with @worker number int epoll trigger mode
    *return:
    *       0--success, other--failed.
    */
    int start(int worker_num);


    /*
    *   connect to remote peer with @ip:@port in nonblocking mode,
    *where the @ip&@port are host byte order, with the @arg to
    *pass to handler object.
    *return:
    *   0--success, other--failed.
    */
    int connect(unsigned long ip, unsigned short port, unsigned int timeout, void *arg);


    /*
    *   stop connector service
    *return:
    *   always return 0
    */
    int stop();
    
    /*concurrent connection status*/
    unsigned int concurrency();

public:
    /*set&get the max connections concurrence limit*/
    void set_max_conns(int maxconns){_max_conns = maxconns;}
    int get_max_conns(){return _max_conns;}

    /*set&get the max events processed for each iocp worker*/
    void set_max_events(int maxevt){_max_events = maxevt;}
    int get_max_events(){return _max_events;}

    /*set&get the max wait time for wait an iocp event, in million seconds*/
    void set_max_waittm(int maxwt){_max_waittm = maxwt;}
    int get_max_waittm(){return _max_waittm;}

    /*set&get the dispatch strategy of new connection to workers*/
    void set_dispatch(dispatch_t type){_dispatch = type;}
    dispatch_t get_dispatch(){return _dispatch;}

private:
    /*push the pending connecting handler to the pending list*/
    int push(handler *hd);

    /*remove the pending connecting handler from the pending list*/
    int remove(handler *hd);

    /*dispatch handler @hd with socket @sock to a epoll worker*/
    int dispatch(handler *hd);

    /*check if the connect timeout*/
    int check_timeout();

    /*thread function for connect to remote address*/
    static void * connect_thread(void *arg);

    /*make the @fd nonblocking mode*/
    static int set_nonblock(int fd);

private:
    //epoll for connecting handlers
    int _epoll_fd;
    //pending connect handlers, only for tracking
    list<handler*> _pending_handlers;
    //mutex for pending handler list
    pthread_rwlock_t _plock;

    //connect thread handler
    pthread_t _thread_connector;
    //stop flag for connector
    bool _connector_stop;

    //handler counter
    unsigned int _hd_counter;
    //worker number
    int _worker_num;
    //iocp workers for accepter
    vector<epoll_worker*> _workers;

    //max concurrence connections for the accepter
    int _max_conns;
    //max events processed per time for worker
    int _max_events;
    //max wait time for the get iocp status
    int _max_waittm;
    //dispatch strategy
    dispatch_t _dispatch;
};


template<class handler>
//tconnector<handler>::tconnector():_epoll_fd(-1),_connector_stop(true),_hd_counter(0),_worker_num(0),_max_conns(100000),_max_events(256),_max_waittm(10),_dispatch(round)
tconnector<handler>::tconnector():_epoll_fd(-1),_connector_stop(true),_hd_counter(0),_worker_num(0),_max_conns(100000),_max_events(256),_max_waittm(10),_dispatch(netsvc::round)
{

}

template<class handler>
tconnector<handler>::~tconnector()
{

}

template<class handler>
int tconnector<handler>::start(int worker_num)
{
    /*initial member*/
    _worker_num = worker_num;

    /*initial lock*/
    if(pthread_rwlock_init(&_plock, NULL) != 0)
        return -1;

    /*start epoll io workers*/
    int worker_conn_limit = _max_conns/_worker_num;
    for(int i=0; i<_worker_num; i++)
    {
        epoll_worker *worker = new epoll_worker();
        if(worker->start(worker_conn_limit, _max_events, _max_waittm,0) != 0)
            return -1;
        _workers.push_back(worker);
    }

    /*create connect handler's epoll*/
    _epoll_fd = epoll_create(256);
    if(_epoll_fd == -1)
        return -1;

    /*start connector thread*/
    _connector_stop = false;
    if(pthread_create(&_thread_connector, NULL, connect_thread, this) != 0)
    {
        _connector_stop = true;
        return -1;
    }

    return 0;
}

template<class handler>
int tconnector<handler>::connect(unsigned long ip, unsigned short port, unsigned int timeout, void *arg)
{
    /*create socket*/
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        return -1;
    /*set nonblocking socket*/
    int err = set_nonblock(sock);
    if(err != 0)
    {
        close(sock);
        return -1;
    }

    /*set remote address*/
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(ip);

    /*connect to remote*/
    err = ::connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if(err != 0)
    {
        if(errno != EINPROGRESS && errno != EALREADY)
        {
            close(sock);
            return -1;
        }
        
        //close(sock);
        //return -1;
    }

    /*set the handler with address information*/
    handler *hdr = new handler();
    hdr->userarg(arg);
    hdr->sock(sock);
    hdr->peer_ip(ip);
    hdr->peer_port(port);
    hdr->set_timeout(timeout);

    /*add the handler to epoll worker*/
    err = push(hdr);
    if(err != 0)
    {
        delete hdr;
        return -1;
    }

    return 0;
}

template<class handler>
int tconnector<handler>::stop()
{
    /*stop connector*/
    if(!_connector_stop)
    {
        _connector_stop = true;
        pthread_join(_thread_connector, 0);
    }

    /*free the pending connect handlers*/
    pthread_rwlock_wrlock(&_plock);
    typename list<handler*>::iterator piter=_pending_handlers.begin(), piter_end=_pending_handlers.end();
    for(; piter!=piter_end; piter++)
    {
        (*piter)->handle_close();
        delete *piter;
    }
    _pending_handlers.clear();
    pthread_rwlock_unlock(&_plock);
    pthread_rwlock_destroy(&_plock);

    /*stop epoll workers*/
    vector<epoll_worker*>::iterator iter = _workers.begin(), iter_end = _workers.end();
    for(; iter!=iter_end; iter++)
    {
        (*iter)->stop();
        delete *iter;
    }
    _workers.clear();

    return 0;
}

template<class handler>
unsigned int tconnector<handler>::concurrency()
{
    int conn = 0;
    for(int i=0; i<_worker_num; i++)
        conn += _workers[i]->concurrency();
    return conn;
}

template<class handler>
int tconnector<handler>::push(handler *hd)
{
    int ret = 0;
    pthread_rwlock_wrlock(&_plock);
    /*add the pending connect handler to epoll*/
    int epoll_opt = EPOLL_CTL_ADD;
    u_int events = EPOLLOUT|EPOLLET|EPOLLRDHUP;
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = hd;

    if(epoll_ctl(_epoll_fd, epoll_opt, hd->sock(), &ev) == -1)
        ret = -1;
    else
        _pending_handlers.push_back(hd);
    pthread_rwlock_unlock(&_plock);
    return ret;
}

template<class handler>
int tconnector<handler>::remove(handler *hd)
{
    pthread_rwlock_wrlock(&_plock);
    _pending_handlers.remove(hd);
    pthread_rwlock_unlock(&_plock);

    return 0;
}

template<class handler>
int tconnector<handler>::dispatch(handler *hd)
{
    //if(_dispatch == round)
    if(_dispatch == netsvc::round)
    {
        int err = _workers[_hd_counter++%_worker_num]->dispatch(hd);
        if(err != 0)
            return -1;
    }
    else
    {
        int pos = 0;
        unsigned int minload = (unsigned int)-1;

        for(int i=0; i<_worker_num; i++)
        {
            unsigned int load = _workers[i]->concurrency();
            if(load < minload)
            {
                minload = load;
                pos = i;
            }
        }

        int err = _workers[pos]->dispatch(hd);
        if(err != 0)
            return -1;
    }

    return 0;
}

template<class handler>
int tconnector<handler>::check_timeout()
{
    pthread_rwlock_wrlock(&_plock);
    typename list<handler*>::iterator iter = _pending_handlers.begin();
    for(; iter!=_pending_handlers.end();)
    {
        if ((*iter)->is_timeout(time(NULL)))
        {
            handler* hd = (*iter);
            hd->handle_open(hd->userarg()); /////////////////
            hd->handle_timeout();
            hd->handle_close();
            delete hd;
            _pending_handlers.erase(iter++);
        }
        else
        {
            iter++;
        }
    }
    pthread_rwlock_unlock(&_plock);

    return 0;
}

template<class handler>
int tconnector<handler>::set_nonblock(int fd)
{
    int opts = fcntl(fd, F_GETFL);
    if(opts < 0) 
        return -1;
    opts = opts | O_NONBLOCK;
    if(fcntl(fd, F_SETFL, opts) < 0)
        return -1;
    return 0;
}

template<class handler>
void* tconnector<handler>::connect_thread(void *arg)
{
    tconnector* conn = (tconnector*) arg;
    int max_events = conn->_max_events;
    int max_waittm = conn->_max_waittm;
    struct epoll_event *events = new epoll_event[max_events];

    while(!conn->_connector_stop)
    {
        int num = epoll_wait(conn->_epoll_fd, events, max_events, max_waittm);
        for(int i=0; i<num; i++)
        {
            handler *hd = (handler*)events[i].data.ptr;

            if(events[i].events & EPOLLERR)
            {
                /*remove from the pending handlers first*/
                conn->remove(hd);

                /*first invoke the handle open method, pass the arg to handler object*/
                hd->handle_open(hd->userarg());
                /*then invoke the close method, for the user to do something*/
                hd->handle_close();
                /*free the handler*/
                delete hd;
            }
            else if (events[i].events & EPOLLOUT)
            {
                /*remove from the pending handlers first*/
                conn->remove(hd);

                /*remove from connector epoll*/
                struct epoll_event ev;
                int epoll_opt = EPOLL_CTL_DEL;
                epoll_ctl(conn->_epoll_fd, epoll_opt, hd->sock(), &ev);

                /*add to epoll worker*/
                int err = conn->dispatch(hd);
                if(err != 0)
                {
                    /*remove from the pending handlers first*/
                    conn->remove(hd);

                    /*first invoke the handle open method, pass the arg to handler object*/
                    hd->handle_open(hd->userarg());
                    /*then invoke the close method, for the user to do something*/
                    hd->handle_close();
                    /*free the handler*/
                    delete hd;
                }
            }
            else
            {//!!impossible!!
                /*first invoke the handle open method, pass the arg to handler object*/
                hd->handle_open(hd->userarg());
                /*then invoke the close method, for the user to do something*/
                hd->handle_close();
                /*free the handler*/
                delete hd;
            }
        }

        conn->check_timeout(); //remove the timeout socket
    }

    delete []events;
    pthread_exit(0);
    return 0;
}


#endif


