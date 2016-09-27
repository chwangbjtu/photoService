from tornado import options
from tornado import log

class Conf(object):
    
    options.define('net_host', type=str)
    options.define('net_port', type=int)
    
    options.define('lis_host', type=str)
    options.define('lis_port', type=int)

    options.define('lis_work_port',type =int)
    
    options.parse_config_file('ps.conf')


    lis_host = options.options.lis_host
    lis_port = options.options.lis_port

    net_host = options.options.net_host
    net_port = options.options.net_port

    lis_work_port = options.options.lis_work_port
if __name__ == "__main__":
    conf = Conf()

