from tornado import options
from tornado import log

class Conf(object):
    options.define('mg_host', type=str)
    options.define('mg_name', type=str)
    options.define('mg_user', type=str)
    options.define('mg_password', type=str)

    options.define('master_host',type=str)
    options.define('master_port',type=int)

    options.define('work_Process_nu',type = int)
    options.define('work_Process_sleep_time',type=float)

    options.parse_config_file('work.conf')

    mg_host = options.options.mg_host
    mg_name = options.options.mg_name
    mg_user = options.options.mg_user
    mg_password = options.options.mg_password
    
    master_host = options.options.master_host
    master_port = options.options.master_port

    work_Process_nu = options.options.work_Process_nu
    work_Process_sleep_time=options.options.work_Process_sleep_time

if __name__ == "__main__":
    conf = Conf()
