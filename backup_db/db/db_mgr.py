# -*- coding:utf-8 -*-
import sys
import traceback
from tornado import log
from db_connect import MysqlConnect
from mg_connect import MongoConnect
from img_dao import ImgDao

class DbManager(object):

    _db_conn = None
    _mg_conn = None
    _daos = {}

    @staticmethod
    def instance():
        if not hasattr(DbManager, "_instance"):
            DbManager._instance = DbManager()
            DbManager._db_conn = MysqlConnect()
            DbManager._mg_conn = MongoConnect()
            DbManager._daos.update({'img': ImgDao(DbManager._db_conn), 
                                    })
        return DbManager._instance

    def commit_transaction(self):
        self._db_conn.commit()

    def get_img_meta(self):
        try:
            dao = self._daos['img']
            res1 = dao.get_poster()
            res2 = dao.get_thumb()

            return res1 + res2
        except Exception, e:
            log.app_log.error(str(e))

    def put_object(self, data, **kwargs):
        try:
            return self._mg_conn.put_object(data, **kwargs)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    def update_img_object(self, obj_id, iid, itype):
        try:
            dao = self._daos['img']
            dao.update_img_object(obj_id, iid, itype)
            self.commit_transaction()

        except Exception, e:
            log.app_log.error(str(e))

if __name__ == "__main__":
    try:
        import json
        mgr = DbManager.instance()

        '''
        res = mgr.get_img_meta()
        '''
        '''
        data = 'hello world'
        meta = {'iid': '888', 'itype': 0, 'url': 'http://www.xxx.com'}
        mgr.put_object(data, **meta)
        '''
        mgr.update_img_object('54fd2979cbd97222ebfe8643', 8, 0)
        mgr.update_img_object('54fd2979cbd97222ebfe8643', 3, 1)

    except Exception, e:
        log.app_log.error(str(e))
