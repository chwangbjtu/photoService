 #-*-coding:utf-8-*-
import sys
sys.path.append('.')
import multiprocessing
import tornado.httpclient
import time
from proc_image import proc_image 
import traceback
from tornado import log
from common.conf import Conf
#from httpcli import httpcli
from http_client import HttpDownload
import json
import datetime
class WorkProcess(multiprocessing.Process):
    def __init__(self, interval):
        multiprocessing.Process.__init__(self)
        self.interval = interval
        self._infos = {"finish":'0',"oid":'',"error":'',"code":'',"width":'',"height":''}
        #self._httpcli = httpcli()
        self._httpcli = HttpDownload()
        self._myprocImage = proc_image()
    def run(self):
        while True:
            try:
                url = 'http://%s:%d/work' % (Conf.master_host,Conf.master_port)
                data = {"apply":''}
                parastr =json.dumps(data)
                #ret = self._httpcli.post_req(url,parastr)
                ret = self._httpcli.post_data(url,parastr)
                if None == ret or ''==ret:
                    time.sleep(self.interval)
                    continue
                if "oid" not in ret or "width" not in ret or "height" not in ret:
                    time.sleep(self.interval)
                    continue
                log.app_log.info(ret)
                retInfo = json.loads(ret)
                if 3!=len(retInfo) or "0" == retInfo["oid"]:
                    time.sleep(self.interval)
                else:
                    msg = 'Get Task,oid:%s;width:%s:height:%s' % (retInfo['oid'],retInfo['width'],retInfo['height'])
                    log.app_log.info(msg)
                    
                    self._infos['oid']    = retInfo['oid']
                    self._infos['width']  = retInfo['width']
                    self._infos['height'] = retInfo['height']
                    
                    self._myprocImage.set_input_data(ret)
                    ret2 = self._myprocImage.get_result()
                    
                    self._infos['error'] = ret2['error']
                    
                    msg = 'Task Finish,oid:%s;width:%s:height:%s' % (retInfo['oid'],retInfo['width'],retInfo['height'])
                    log.app_log.info(msg)

                    url_finish = 'http://%s:%d/work' % (Conf.master_host,Conf.master_port) 
                    return_data = json.dumps(self._infos)
                    #rets = self._httpcli.post_req(url_finish,return_data)
                    rets = self._httpcli.post_data(url_finish,return_data)
                    log.app_log.info(rets)
            except Exception,e:
                log.app_log.info(traceback.format_exc())
                time.sleep(self.interval)

if __name__ == "__main__":
    try:
        p = {}
        for i in range(0,Conf.work_Process_nu):
            p[i] = WorkProcess(Conf.work_Process_sleep_time)
        for v in p:
            p[v].start()
        for v in p:
            p[v].join()
    except Exception,e:
        log.app_log.info(traceback.format_exc())



