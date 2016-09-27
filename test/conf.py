from tornado import options
from tornado import log

class Conf(object):
    options.parse_config_file('work.conf')

if __name__ == "__main__":
    conf = Conf()
