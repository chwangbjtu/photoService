# -*-coding:utf-8 -*-
import urllib2
import traceback

class Upload(object):
    def __init__(self):
        self._url = "http://passport.iqiyi.com/apis/pusericon/upload_icon.action"
        self._boundary = "cMXsCqJo4YKHyXzG6L_8sj03RhPbs0oYzyw"
        self._auth = "3cvP8PIZKux36PcoVDSISaiJm1fdGTzcpPQ00uOwEkraM0gLYiWA24nKLA5YQwtEq3i16"
        self._carf = "\r\n"
      
    def post_data(self, url, body, headers=None, timeout=3):
        data = ''
        try:
            req = urllib2.Request(url, body)
            req.add_header('Content-Type', 'multipart/form-data; boundary=%s' % self._boundary)
            resp = urllib2.urlopen(req, timeout=timeout)
            chunk_size = 100 * 1024
            while True:
                chunk = resp.read(chunk_size)
                if not chunk:
                    break
                data += chunk
            resp.close()
            
        except urllib2.HTTPError, e:
            print e.read()
        except Exception, e:
            print traceback.format_exc()
        finally:
            return data
            
    def upload(self, filename):
        #read file
        file_obj = open(filename, 'r')
        #up_data = self._carf
        up_data = ""
        file_str = self._carf
        for line in file_obj:
            file_str += line
        
        data_header = self._carf
        data_header += "--" + self._boundary + self._carf
        data_header += "Content-Disposition: form-data; name=\"img\"; filename=\"" + filename + "\"" + self._carf
        data_header += "Content-Type: application/octet-stream" + self._carf
        #print data_header
        
        auth_cookie = self._carf
        auth_cookie += "--" + self._boundary + self._carf
        auth_cookie += "Content-Disposition: form-data; name=\"authcookie\"" + self._carf
        auth_cookie += self._carf + self._auth
        #print auth_cookie
        
        end_data = self._carf
        end_data += "--" + self._boundary + self._carf
        end_data += "Content-Disposition: form-data; name=\"agenttype\"" + self._carf
        end_data += self._carf + "21" + self._carf
        end_data += "--" + self._boundary + "--" + self._carf
        
        up_data += data_header + file_str + auth_cookie + end_data
        #print up_data
        
        http_headers = {}
        http_headers['Content-Type'] = "multipart/form-data; boundary=" + self._boundary
       
        ret = self.post_data(self._url, up_data, http_headers)
        print ret

if __name__ == "__main__":
    import sys
    obj = Upload()
    obj.upload(sys.argv[1])
            
            
            