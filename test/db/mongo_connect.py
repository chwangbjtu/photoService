# -*-coding:utf-8 -*-
import traceback
from tornado import log
from pymongo import MongoClient

import sys
sys.path.append('.')

#refer: http://api.mongodb.org/python/current/api/pymongo/
class MongoConnect(object):
    
    def __init__(self, urls, db, user, passwd):
        self._urls = urls
        self._dbname = db
        self._username = user
        self._passwd = passwd
        
        self.connect()

    def connect(self):
        try:
            urls = []
            if self._urls:
                url_tmp = self._urls.split(';')
                for url in url_tmp:
                    url = url.strip()
                    urls.append(url)
            self._mongo = MongoClient(urls)
            self._db = getattr(self._mongo, self._dbname)
            self._db.authenticate(self._username, self._passwd)
        except Exception, err:
            log.app_log.error(traceback.format_exc())

    '''
        db.test.delete_one({'x': 1})
        [{'x':1,'_id':0},{'x':1,'_id':1}] -> [{'x':1,'_id':1}]
    '''
    def delete_one(self, coll_name, filter):
        try:
            return self._db[coll_name].delete_one(filter)
        except Exception, err:
            log.app_log.error(traceback.format_exc())

    '''
        db.test.delete_many({'x': 1})
        [{'x':1,'_id':0},{'x':1,'_id':1}] -> []
    '''
    def delete_many(self, coll_name, filter):
        try:
            return self._db[coll_name].delete_many(filter)
        except Exception, err:
            log.app_log.error(traceback.format_exc())

    '''
    Parameters:
        filter: A query that matches the document to update.
        update: The modifications to apply.
        upsert (optional): If True, perform an insert if no documents match the filter.
    Examples:
        db.test.update_one({'x': 1}, {'$inc': {'x': 3}})
        {'x':1, '_id':0} -> {'x':4, '_id':0}
        {'x':1, '_id':1} -> {'x':1, '_id':1}
    '''
    def update_one(self, coll_name, filter, update, upsert=False):
        try:
            return self._db[coll_name].update_one(filter, update, upsert)  
        except Exception, err:
            log.app_log.error(traceback.format_exc())

    '''
    Parameters:
        filter: A query that matches the document to update.
        update: The modifications to apply.
        upsert (optional): If True, perform an insert if no documents match the filter.
    Examples:
        db.test.update_one({'x': 1}, {'$inc': {'x': 3}})
        {'x':1, '_id':0} -> {'x':4, '_id':0}
        {'x':1, '_id':1} -> {'x':4, '_id':1}
    '''
    def update_many(self, coll_name, filter, update, upsert=False):
        try:
            return self._db[coll_name].update_one(filter, update, upsert)  
        except Exception, err:
            log.app_log.error(traceback.format_exc())

    def insert_one(self, coll_name, document):
        try:
            return self._db[coll_name].insert_one(document)
        except Exception, err:
            log.app_log.error(traceback.format_exc())

    '''
    Parameters:
        ordered (optional): 
            If True (the default) documents will be inserted on the server serially, in the order provided. If an error occurs all remaining inserts are aborted. 
            If False, documents will be inserted on the server in arbitrary order, possibly in parallel, and all document inserts will be attempted
    '''
    def insert_many(self, coll_name, documents, ordered=True):
        try:
            return self._db[coll_name].insert_many(documents, ordered)
        except Exception, err:
            log.app_log.error(traceback.format_exc())


    '''
    Get a single document from the collection of coll_name
    Parameters:
        filter (optional): a dictionary specifying the query to be performed OR any other type to be used as the value for a query for "_id"
    '''
    def find_one(self, coll_name, filter_or_id=None, *args, **kwargs):
        try:
            return self._db[coll_name].find_one(filter_or_id, *args, **kwargs)
        except Exception, err:
            log.app_log.error(traceback.format_exc())
        

    '''
    The filter argument is a prototype document that all results must match. For example:
    db.test.find({"hello": "world"})
    Parameters:
        sort: ("_id", pymongo.ASCENDING)
        allow_partial_results (optional): if True, mongos will return partial results if some shards are down instead of returning an error.
    '''
    def find(self, coll_name, filter=None, skip=0, limit=0, sort=None, allow_partial_results=False):
        try:
            return self._db[coll_name].find(filter=filter, skip=skip, limit=limit, sort=sort, allow_partial_results=allow_partial_results)
        except Exception, err:
            log.app_log.error(traceback.format_exc())

    '''
    All optional count parameters should be passed as keyword arguments to this method. Valid options include
    Parameters:
        filter (optional): A query document that selects which documents to count in the collection.
        **kwargs:
            hint (string or list of tuples): The index to use. Specify either the index name as a string or the index specification as a list of tuples (e.g. [(‘a’, pymongo.ASCENDING), (‘b’, pymongo.ASCENDING)]).
            limit (int): The maximum number of documents to count.
            skip (int): The number of matching documents to skip before returning results.
            maxTimeMS (int): The maximum amount of time to allow the count command to run, in milliseconds
    '''
    def count(self, coll_name, filter=None, **kwargs):
        try:
            return self._db[coll_name].count(filter, **kwargs)
        except Exception, err:
            log.app_log.error(traceback.format_exc())
