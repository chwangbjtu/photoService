import time
import os
import traceback
import magic
import sys
sys.path.append('.')
import commands
from db.mg_connect import MongoConnect
from datetime import date,datetime
from common.conf import Conf
from tornado import log
def image_to_mongo():
    try:
        conn = MongoConnect()
        _magic = magic.Magic(mime=True)
        commod = 'ls %s'%Conf.path
        first=os.popen(commod).readlines()
        ##print first
        for folder_first in first:
            commod2 =commod + '/%s'%folder_first.strip('\n')
            #print com
            second = os.popen(commod2).readlines() 
            for folder_second in second:
                #print com
                commod3 = commod2 + '/%s'%folder_second.strip('\n')
                third = os.popen(commod3).readlines()
                for file in third:
                    filepath = Conf.path+"/"+folder_first.strip('\n')+"/"+folder_second.strip('\n')+"/"+file.strip('\n')
                    #print filepath
                    #continue
                    f =open(filepath)
                    data = f.read( )
                    f.close()
                    type = _magic.from_buffer(data)
                    meta_find = {"height":0,"width":0,"oid":file.strip('\n')}
                    meta_insert = {"height":0,"width":0,"oid":file.strip('\n'),"contentType":type}
                    if conn:
                        #if conn.find_object(**meta_find):
                        res = conn.find_object(**meta_find)
                        if res:
                            continue
                        conn.put_object(data, **meta_insert)
                    else:
                        conn.reconnect()
        return 
    except Exception,e:
        log.app_log.error(traceback.format_exc())
        return
if __name__ == "__main__":
    try:
        image_to_mongo()
    except Exception,e:
        print e







