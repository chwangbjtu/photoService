from tornado import options
from tornado import log

class Conf(object):
    options.define('mg_host', type=str)
    options.define('mg_port', type=int)
    options.define('mg_name', type=str)
    options.define('mg_user', type=str)
    options.define('mg_password', type=str)
    options.define('path', type=str)
    options.parse_config_file('back.conf')
    
    mg_host = options.options.mg_host
    mg_port = options.options.mg_port
    mg_name = options.options.mg_name
    mg_user = options.options.mg_user
    mg_password = options.options.mg_password
    path = options.options.path

if __name__ == "__main__":
    conf = Conf()
    log.app_log.debug('mg_host: %s' % conf.mg_host)
    log.app_log.debug('mg_port: %s' % conf.mg_port)
    log.app_log.debug('mg_name: %s' % conf.mg_name)
    log.app_log.debug('mg_user: %s' % conf.mg_user)
    log.app_log.debug('mg_password: %s' % conf.mg_password)
