import md5
import os
import time
import random
import traceback
from tornado import log

import sys
sys.path.append('.')
from conf import Conf
from db.gridfs_connect import GridFsConnect

class Test(object):

    def __init__(self):
        self._oids = []
        self._dir = '/home/workspace/siv/images/full'
        self._in_file = 'oid.txt'
        self._out_file = 'out_oid.txt'
        self._coll_name = 'fs'
        self._total_num = 100000
        self._md5 = md5.new()
        self._urls = '125.88.157.196:20000;125.88.157.197:20000;125.88.157.198:20000'
        self._dbname = 'zyx_test_3'
        self._username = 'siv'
        self._passwd = 'siv'
        self._gridfs_connect = GridFsConnect(self._urls, self._dbname, self._username, self._passwd)

    def write(self):
        try:
            print 'write start...'
            listfile = os.listdir(self._dir)
            count = 0
            total_time = 0
            os.remove(self._out_file)
            oid_f = open(self._out_file, 'a')
            for file in listfile:
                file_name = self._dir + '/' + file
                f = open(file_name, 'r')
                data = f.read()
                start_time = time.time()
                self._md5.update(data)
                img_md5 = self._md5.hexdigest()
                kwargs = {'oid':img_md5,'width':0,'height':0,'contentType':'image/jpeg'}
                res = self._gridfs_connect.put(self._coll_name, data, **kwargs)
                end_time = time.time()
                oid_f.write('%s\n' % img_md5)
                total_time = total_time + (end_time - start_time)
                f.close()
                count = count + 1
                if count == self._total_num:
                    break
            oid_f.close()
            avg_time = total_time / float(count)
            log.app_log.info('Write:    total time:%s; total num:%s; avg time:%s' % (total_time, count, avg_time))
        except Exception,e:
            log.app_log.info(traceback.format_exc())

    def read(self):
        try:
            print 'read start...'
            if not self._oids:
                self.load_oid()
            rand_top = len(self._oids)
            total_time = 0
            for i in range(self._total_num):
                ran = random.randint(0, rand_top)
                oid = self._oids[ran]
                start_time = time.time()
                res = self._gridfs_connect.find(self._coll_name, {'oid':oid})
                for c in res:
                    data = c.read()
                    break
                end_time = time.time()
                total_time = total_time + (end_time - start_time)
            avg_time = total_time / float(self._total_num)
            log.app_log.info('Read:     total time:%s; total num:%s; avg time:%s' % (total_time, self._total_num, avg_time))
        except Exception,e:
            log.app_log.info(traceback.format_exc())

    def load_oid(self):
        try:
            f = open(self._in_file, "r")
            while True:
                line = f.readline()
                if line:
                    line = line.strip()
                    self._oids.append(line) 
                else:
                    break
            f.close()
        except Exception,e:
            log.app_log.info(traceback.format_exc())

if __name__ == '__main__':
    test = Test()
    test.read()
    #test.write()
