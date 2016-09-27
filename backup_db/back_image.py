import time
import os
import traceback
import sys
sys.path.append('.')
from db.mg_connect import MongoConnect
from datetime import date,datetime
from common.conf import Conf
from tornado import log
def get_time():
    ret = datetime.strptime('1900', "%Y")
    try:
        file_object = open('time.conf',"r")
        line = file_object.readline()
        if line:
            ret = datetime.strptime(line.strip('\n'), "%Y-%m-%d %H:%M:%S %f")
        return ret
    except Exception,e:
        log.app_log.error(traceback.format_exc())
        return ret
def set_time(time):
    file_object = open('time.conf','w')
    try:
        file_object.write(time.strftime('%Y-%m-%d %H:%M:%S %f'))
    finally:
        file_object.close()
def back_image(mytime):
    try:
        conn = MongoConnect()
        ret_time = mytime
        if conn:
            meta = {"uploadDate":{"$gte":mytime}}
            res = conn.find_all(**meta).batch_size(10000)
            for data in res:
                try:
                    if 0 != data.height or 0 != data.width:
                        continue
                except Exception,e:
                    continue
                if data.uploadDate>ret_time:
                    ret_time = data.uploadDate
                list = data.contentType.split('/')
                st = data.oid
                dirpath = '%s/%s/%s/'%(Conf.path,st[:2],st[2:4])
                filename = '%s'%(data.oid)
                if not os.path.exists(dirpath):
                    os.makedirs(dirpath)
                path = '%s%s'%(dirpath,filename)
            
                f =open(path,'wb')
                f.write(data.read())
                f.close()
        return ret_time
    except Exception,e:
        log.app_log.error(traceback.format_exc())
        return ret_time
if __name__ == "__main__":
    try:
        last_time = get_time()
        print last_time
        test_time = back_image(last_time)
        print test_time
        set_time(test_time)
    except Exception,e:
        print e

                
