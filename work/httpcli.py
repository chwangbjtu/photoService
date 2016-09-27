#!/usr/bin/python
# -*- coding:utf-8 -*-  
import sys
sys.path.append('.')
import tornado.options
import tornado.httpclient
from tornado import log
import traceback
import json
class httpcli:

    http_client = tornado.httpclient.HTTPClient()    


    def get_req(self,url):
        res = None
        try:
            #http_request = tornado.httpclient.HTTPRequest(url=url,body=body,method='POST',headers=http_header,connect_timeout=20,request_timeout=600)
            http_request = tornado.httpclient.HTTPRequest(url=url,connect_timeout=5,request_timeout=600)
            http_response = self.http_client.fetch(http_request)
            res = http_response.body
        except Exception, err:
            log.app_log.error(traceback.format_exc())
        return res

    def post_req(self,url,parastr):
        res = None
        try:
            http_request = tornado.httpclient.HTTPRequest(url=url,body=parastr,method='POST',connect_timeout=5,request_timeout=600)
            #http_request = tornado.httpclient.HTTPRequest(url=url,connect_timeout=5,request_timeout=600)
            http_response = self.http_client.fetch(http_request)
            res = http_response.body
        except Exception, err:
            log.app_log.error(traceback.format_exc())
        return res

if __name__ == "__main__":

    url = "http://192.168.144.137:8888/web/"

    data = {"task_id":'1312',"oid_w_h":'24_34_35'}
    parastr =json.dumps(data)
    print parastr
    url2 = "http://www.baidu.com"
    httpclient = httpcli()
    #print httpclient.get_req(url2)
    httpclient.post_req(url,parastr)




