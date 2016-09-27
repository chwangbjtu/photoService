#-*-coding:utf-8-*-
import traceback
from collections import deque
import threading
import time
import SocketServer
import json

import tornado.ioloop
import tornado.web
import tornado.httpserver
import tornado.options
from tornado import log
from common.conf import Conf
from httpcli import httpcli

semaphore = threading.Semaphore(0)
mutex = threading.Lock()
def singleton(cls, *args, **kw):
    instances = {}
    def _singleton():
        if cls not in instances:
            instances[cls] = cls(*args, **kw)
        return instances[cls]
    return _singleton

@singleton
class job(object):
    def __init__(self):
        self.__job_queue     = deque()
        self.__job_fin_queue  = deque()
        self.__worker_apply_num = 0
        self.__worker_finish_succ = 0
        self.__worker_finish_fail = 0
        self.__server_trim_record = 0
        self.__server_trim_queue  = 0
        self.__server_transcode_secc = 0
        self.__server_transcode_fail = 0

        self.__task_record = {}
    def add_task_record(self,key):
        try:
            if key in self.__task_record:
                self.__task_record[key] += 1
                return False
            else:
                self.__task_record[key] = 1
                return True
        except Exception,e:
            log.app_log.info(traceback.format_exc())
            return False
    def delete_task_record(self,key):
        try:
            del self.__task_record[key]
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def get_task_record(self,key):
        try:
            return self.__task_record[key]
        except Exception,e:
            log.app_log.info(traceback.format_exc())
        
    def get_worker_apply_num(self):
        try:
            return self.__worker_apply_num
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def get_worker_finish_succ(self):
        try:
            return self.__worker_finish_succ
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def get_worker_finish_fail(self):
        try:
            return self.__worker_finish_fail
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def get_server_trim_record(self):
        try:
            return self.__server_trim_record
        except Exception ,e:
            log.app_log.info(traceback.format_exc())

    def get_server_trim_queue(self):
        try:
            return self.__server_trim_queue
        except Exception ,e:
            log.app_log.info(traceback.format_exc())
    
    def get_server_transcode_secc(self):
        try:
            return self.__server_transcode_secc
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def get_server_transcode_fail(self):
        try:
            return self.__server_transcode_fail
        except Exception,e:
            log.app_log.info(traceback.format_exc())

    
    def add_worker_apply_num(self):
        try:
            self.__worker_apply_num += 1
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def add_worker_finish_succ(self):
        try:
            self.__worker_finish_succ += 1
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def add_worker_finish_fail(self):
        try:
            self.__worker_finish_fail += 1
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def add_server_trim_queue(self):
        try:
            self.__server_trim_queue += 1
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def add_server_trim_record(self):
        try:
            self.__server_trim_record += 1
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def add_server_transcode_secc(self):
        try:
            self.__server_transcode_secc += 1
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def add_server_transcode_fail(self):
        try:
            self.__server_transcode_fail += 1
        except Exception ,e:
            log.app_log.info(traceback.format_exc())


    def put_data_to_deque(self,input):
        try:
            self.__job_queue.append(input)
        except Exception ,e:
            log.app_log.info(traceback.format_exc())
    def put_data_to_fin_queue(self,input):
        try:
            self.__job_fin_queue.append(input)
        except Exception, e:
            log.app_log.info(traceback.format_exc())

    def get_queue_data(self):
        try:
            if self.__job_queue.__len__()>0:
                value = self.__job_queue.popleft()
                return value
            else:
                return None
        except Exception ,e:
            log.app_log.info(traceback.format_exc())

    def get_fin_data(self):
        try:
            if self.__job_fin_queue.__len__()>0:
                value = self.__job_fin_queue.popleft()
                return value
            else:
                return None
        except Exception,e:
            log.app_log.info(traceback.format_exc())

    def get_queue_size(self):
        try:
            return self.__job_queue.__len__()
        except Exception,e:
            log.app_log.info(traceback.format_exc())

    def get_fin_queue_size(self):
        try:
            return self.__job_fin_queue.__len__()
        except Exception,e:
            log.app_log.info(traceback.format_exc())

    def get_all_job_queue_data(self):
        try:
            myString = ''
            for v in self.__job_queue:
                myString+="%10s" % v
            return myString
        except Exception,e:
            log.app_log.info(traceback.format_exc())

    def get_all_job_fin_queue_data(self):
        try:
            myString = ''
            for v in self.__job_fin_queue:
                myString+="%10s" % v
            return myString
        except Exception,e:
            log.app_log.info(traceback.format_exc())

class master(object):
    def __init__(self,input):
        try:
            self.__port = int(input)
            self.__app=tornado.web.Application([(r"/trim", WebHandler),(r"/work", WorkHandler)])
            self.__to_web_thread = threading.Thread(target=self.run_to_web)
            self.__watch_thread = threading.Thread(target=self.watch)
            self.__job=job()
            self.__httpclient = httpcli()
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def watch(self):
        self.server = SocketServer.UDPServer((Conf.lis_host, Conf.lis_port), MyUDPHandler)
        self.server.serve_forever()

    def run_to_web(self):
        try:
            while True:
                if semaphore.acquire():
                    if 0 < self.__job.get_fin_queue_size():
                        value = self.__job.get_fin_data()
                        msg = 'Finish;%s' % value
                        log.app_log.info(msg)
                        record= {}
                        record["oid"]    = value["oid"]
                        record["width"]  = value["width"]
                        record["height"] = value["height"]
                        
                        Url = 'http://%s:%d/transcode' % (Conf.net_host,Conf.net_port)
                        
                        send_value = {}
                        send_value["error"] = value["error"]
                        send_value["code"] = value["code"]
                        send_value["oid"] = value["oid"]
                        send_value["w"] = value["width"]
                        send_value["h"] = value["height"]
                        
                        mutex.acquire()
                        num = self.__job.get_task_record(json.dumps(record))
                        self.__job.delete_task_record(json.dumps(record))
                        mutex.release()
                        
                        for i in range(num):
                            if '0' == value['error']:
                                self.__job.add_server_transcode_secc() 
                            else:
                                self.__job.add_server_transcode_fail()
                            ret = self.__httpclient.post_req(Url,json.dumps(send_value))
                            log.app_log.info(ret)
        except Exception,e:
            log.app_log.info(traceback.format_exc())

    def start(self):
        try:
            self.__to_web_thread.setDaemon(True)
            self.__to_web_thread.start()
            
            self.__watch_thread.setDaemon(True)
            self.__watch_thread.start()

            http_server = tornado.httpserver.HTTPServer(self.__app)
            http_server.listen(self.__port)
            tornado.ioloop.IOLoop.instance().start()
        except Exception,e:
            log.app_log.info(traceback.format_exc())

class WebHandler(tornado.web.RequestHandler):
    def post(self, *args, **kwargs):
        try:
            if self.request.body:
                jbody = json.loads(self.request.body)
                if 3==len(jbody) and "w" in jbody and "oid" in jbody and "h" in jbody:
                    self.job = job()
                    
                    record= {}
                    record["oid"]    = jbody["oid"]
                    record["width"]  = jbody["w"]
                    record["height"] = jbody["h"]
                    
                    mutex.acquire()
                    ret = self.job.add_task_record(json.dumps(record))
                    if ret:
                        self.job.put_data_to_deque(json.dumps(record))
                        self.job.add_server_trim_queue()
                 
                    self.job.add_server_trim_record()
                    mutex.release()
                    value = {"error":"0","code":""}
                    self.write(value)
                    log.app_log.info(self.request.body)
        except Exception,e:
            log.app_log.info(traceback.format_exc())

class WorkHandler(tornado.web.RequestHandler):
    def post(self, *args, **kwargs):
        self.job = job()
        try:
            if self.request.body:
                jbody = json.loads(self.request.body)
                if 1==len(jbody) and "apply" in jbody:
                    value = self.job.get_queue_data()
                    if None == value:
                        data = {"oid":'0',"width":'0',"height":'0'}
                        self.write(json.dumps(data))
                    else:
                        self.job.add_worker_apply_num()
                        self.write(value)
                        log.app_log.info(value)
                elif 6==len(jbody) and "finish" in jbody:
                    log.app_log.info(jbody)
                    finishTask = {}
                    finishTask["error"] = jbody["error"]
                    finishTask["code"] = jbody["code"]
                    finishTask["oid"] = jbody["oid"]
                    finishTask["width"] = jbody["width"]
                    finishTask["height"] = jbody["height"]

                    self.job.put_data_to_fin_queue(finishTask)
                    semaphore.release()
                    ret_value={}
                    if '0' == finishTask["error"]:
                        self.job.add_worker_finish_succ()
                        ret_value["oid"]=jbody["oid"]
                        ret_value["error"]="0"
                    else:
                        self.job.add_worker_finish_fail()
                        ret_value["oid"]=""
                        ret_value["error"]="1"
                    self.write(ret_value)
                else:
                    pass
        except Exception,e:
            log.app_log.info(traceback.format_exc())


class MyUDPHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        try:
            data = self.request[0].strip()
            socket = self.request[1]
            self.job = job()
            self.mystring = "             get task:%10d\n          finish task:%10d\n     worker_apply_num:%10d\n   worker_finish_succ:%10d\n   worker_finish_fail:%10d\n   server_trim_record:%10d\n    server_trim_queue:%10d\nserver_transcode_secc:%10d\nserver_transcode_fail:%10d" % (self.job.get_queue_size(),self.job.get_fin_queue_size(),self.job.get_worker_apply_num(),self.job.get_worker_finish_succ(),self.job.get_worker_finish_fail(),self.job.get_server_trim_record(),self.job.get_server_trim_queue(),self.job.get_server_transcode_secc(),self.job.get_server_transcode_fail())
            socket.sendto(self.mystring, self.client_address)
        except Exception,e:
            log.app_log.info(traceback.format_exc())

if __name__ == "__main__":
    mymaster = master(Conf.lis_work_port)
    mymaster.start()

