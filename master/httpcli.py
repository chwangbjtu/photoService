#!/usr/bin/python
# -*- coding:utf-8 -*-  
import tornado.options
import tornado.httpclient
from tornado import log
import traceback

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
