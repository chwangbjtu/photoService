# -*- coding:utf-8 -*-
import sys
from db_connect import MysqlConnect
from tornado import log
import traceback

class ImgDao(object):
    def __init__(self, db_conn=None):
        if not db_conn:
            db_conn = MysqlConnect()
        self._db_conn = db_conn

    def get_poster(self):
        try:
            sql = "SELECT poster_id as iid, 0 AS itype, url FROM poster WHERE obj_id is null"
            res = self._db_conn.db_fetchall(sql, as_dic=True)
            self._db_conn.commit()

            return res
        except Exception, e:
            log.app_log.error("Get poster exception: %s " % traceback.format_exc())
            log.app_log.debug("sql: %s para: %s" % (sql, para))
    
    def get_thumb(self):
        try:
            sql = "SELECT thumb_id as iid, 1 AS itype, url FROM thumb WHERE obj_id is null"
            res = self._db_conn.db_fetchall(sql, as_dic=True)
            self._db_conn.commit()

            return res
        except Exception, e:
            log.app_log.error("Get thumb exception: %s " % traceback.format_exc())
            log.app_log.debug("sql: %s para: %s" % (sql, para))

    def update_img_object(self, obj_id, iid, itype):
        try:
            if itype == 0:
                sql = "UPDATE poster SET obj_id = %s WHERE poster_id = %s"
            else:
                sql = "UPDATE thumb SET obj_id = %s WHERE thumb_id = %s"

            para = [obj_id, iid]
            res = self._db_conn.execute_sql(sql, para)
            self._db_conn.commit()

        except Exception, e:
            log.app_log.error("Get thumb exception: %s " % traceback.format_exc())
            log.app_log.debug("sql: %s para: %s" % (sql, para))

if __name__ == "__main__":

    try:
        import json
        db_conn = MysqlConnect()
        img_dao = ImgDao(db_conn)

        '''
        res = img_dao.get_poster()
        log.app_log.info(json.dumps(res))
        res = img_dao.get_thumb()
        log.app_log.info(json.dumps(res))
        '''

        img_dao.update_img_object('54fd2979cbd97222ebfe8643', 8, 0)
        img_dao.update_img_object('54fd2979cbd97222ebfe8643', 3, 1)

        db_conn.commit()

    except Exception,e:
        log.app_log.error(traceback.format_exc())
