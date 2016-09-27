# -*- coding:utf-8 -*-
import sys
import time
sys.path.append('.')
import traceback

import pymongo
import gridfs

from bson import BSON
from tornado import log
from bson.objectid import ObjectId

from common.conf import Conf

class Mongo3Connect(object):

    def __init__(self):
        self.client = None
        self.db_conn = None

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
            self.client = pymongo.MongoClient(host=Conf.mg2_host, port=Conf.mg2_port)
            self.db_conn = getattr(self.client, Conf.mg2_name)
            self.db_conn.authenticate(Conf.mg2_user, Conf.mg2_password)
            return True
        except Exception, e:
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

    def put_one(self, obj):
        try:
            '''
            files = {'_id':obj['_id'], 
                    'vid':obj['vid'], 
                    'proj_id':obj['proj_id'],
                    'img_type':obj['img_type'],
                    'seq_id':obj['seq_id'],
                    'width':obj['width'],
                    'height':obj['height'],
                    'origin':obj['origin'],
                    }
            chunks = {'_id':obj['_id'],
                    'data':obj['data']}
            '''
            self.db_conn.auto.insert(obj) 
            pass
        except Exception, e:
            print e
            log.app_log.error(traceback.format_exc())

    def find_one(self, obj_id):
        try:
            #pass
            #self.db_conn.auto.find_one({'_id':ObjectId(obj_id)}) 
            self.db_conn.manual.find_one({'_id':obj_id})  
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    def find_all(self):
        try:
            return self.db_conn.manual.find({}, {'_id': 1})
        except Exception, e:
            log.app_log.error(traceback.format_exc())

if __name__ == "__main__":

    try:
        conn = MongoConnect()
    except Exception as e:
        log.app_log.error("Exception: %s" % traceback.format_exc())
