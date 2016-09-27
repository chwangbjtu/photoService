#coding=utf-8
import sys
sys.path.append('.')   
import subprocess
import time
import re
from db.mg_connect import MongoConnect
from bson.objectid import ObjectId
from deal_image import deal_image
import traceback
import json
from common.conf import Conf
from tornado import log
import magic 
class proc_image(object):
    def __init__(self):
        self.__width =0
        self.__height =0
        self.__MD5 = None
        self.__contentType=None
        self.__imageData = None
        self.__conn = MongoConnect()
        self.__magic = magic.Magic(mime=True)
        self.__info = {'error':'1','code':''}
    def set_input_data(self,input):
        try:
            self.__input = input
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def decode_data(self):
        data = json.loads(self.__input) 
        self.__MD5 = data['oid']
        self.__width = int(data['width'])
        self.__height = int(data['height'])
	self.__magic.from_buffer(self.__imageData)
    def get_image_fromDB(self):
        try:
            if self.__conn:
                self.decode_data()
                if self.__width<=0 or self.__height<=0:
                    self.__info['error']='1'
                    log.app_log.info("宽高不合法")
                    return 1
                if self.__width>10000 or self.__height>10000:
                    self.__info['error']='1'
                    return 1
                meta = {'oid': self.__MD5,'width':self.__width,'height':self.__height}
                obj = self.__conn.find_object(**meta)
                if None == obj:
                    meta = {'oid': self.__MD5,'width':0,'height':0}
                    obj1 = self.__conn.find_object(**meta)
                    if None == obj1:
                        self.__info['error']='1'
                        return 1
                    else:
                        self.__imageData = obj1.read()
                        if obj1.contentType:
                            self.__contentType = obj1.contentType
                        else:
                            self.__contentType = self.__magic.from_buffer(self.__imageData)
                        self.__info['error']='0'
                        return 0
                else:
                    self.__info['error']='0'
                    return 1
        except Exception,e:
            log.app_log.info(traceback.format_exc())
            return 1

    def handle_image(self):
        try:
            ret = self.get_image_fromDB()
            if 1 == ret:
                return None
            else:
                mydealImage = deal_image(self.__imageData,self.__width,self.__height)
                mydealImage.deal()
                return mydealImage.get_image()
        except Exception,e:
            log.app_log.info(traceback.format_exc())
            return None

    def get_result(self):
        try:
            ret = self.handle_image()
            if None == ret:
                self.__info['error']='1'
                return self.__info
            else:
                if self.__conn:
                    meta = {'oid': self.__MD5,'width':self.__width,'height':self.__height,'contentType':self.__contentType}
                    retValue = self.__conn.put_object(ret, **meta)
                    if None == retValue:
                        self.__info['error']='1'
                        return self.__info
                    else:
                        return self.__info
        except Exception,e:
            log.app_log.info(traceback.format_exc())
            self.__info['error']='1'
            return self.__info
    
    def disConnect(self):
        pass
if __name__ == '__main__':
    data1 = {"oid":"8a4f1753da8aec8b381fa5e029daabcd","width":"20","height":"20"}
    myprocImage = proc_image()
    myprocImage.set_input_data(json.dumps(data1))
    print myprocImage.get_result()
