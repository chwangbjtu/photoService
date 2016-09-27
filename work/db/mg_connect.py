# -*- coding:utf-8 -*-
import sys
import time
sys.path.append('.')
import traceback

import pymongo
import gridfs

from tornado import log
from bson.objectid import ObjectId

from common.conf import Conf

class MongoConnect(object):

    def __init__(self):
        self.client = None
        self.db_conn = None
        self.db_fs = None

        self.connect()

    def __del__(self):
        if self.client:
            self.client.close()

    def ping(self):
        try:
            return self.client.alive()
        except Exception, e:
            return False

    def close(self):
        try:
            if self.client:
                self.client.close()
                self.client = None
            return True
        except Exception, e:
            return False

    def connect(self):
        try:
            hosts = []
            if Conf.mg_host:
                hosts_tmp = Conf.mg_host.split(',')
                for host in hosts_tmp:
                    host = host.strip()
                    hosts.append(host) 
            self.client = pymongo.MongoClient(host=hosts, connect=False)
            self.db_conn = getattr(self.client, Conf.mg_name)
            self.db_conn.authenticate(Conf.mg_user, Conf.mg_password)
            self.db_fs =  gridfs.GridFS(self.db_conn)
            return True
        except Exception, e:
            print e
            log.app_log.error(str(e))
            return False

    def reconnect(self):
        try:
            log.app_log.error('++reconnect db')
            while True:
                if not self.ping():
                    self.connect()
                else:
                    break
                time.sleep(1)
            log.app_log.error('++reconnected db')
                
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    def put_object(self, data, **kwargs):
        try:
            return self.db_fs.put(data, **kwargs)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    def get_object(self, obj_id):
        try:
            return self.db_fs.get(obj_id)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    def find_object(self, **kwargs):
        try:
            return self.db_fs.find_one(kwargs)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    def find_one(self, obj_id):
        try:
            return self.db_fs.find_one({'_id':ObjectId(obj_id)})
        except Exception, e:
            log.app_log.error(traceback.format_exc())
  
    def find_all(self):
        try:
            return self.db_fs.find().limit(1)
            
            #return self.db_fs.find().limit(1000000)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

if __name__ == "__main__":

    try:
        conn = MongoConnect()
        if conn:
            for i in range(200):
                data = 'hello world'
                meta = {'vid': '888'}
                _id = conn.put_object(data, **meta)
                print _id
    except Exception as e:
        print e
        log.app_log.error("Exception: %s" % traceback.format_exc())
