# -*- coding:utf-8 -*-
import time
import gridfs
import pymongo
import traceback
from tornado import log
from pymongo import MongoClient
from bson.objectid import ObjectId

import sys
sys.path.append('.')

#refer: http://api.mongodb.org/python/current/api/
class GridFsConnect(object):

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
        except Exception, e:
            log.app_log.error(str(e))

    '''
    Delete a file from GridFS by "_id"
    Multi-threads safely is not supported
    Any processes/threads reading from the file while this method is executing will likely see an invalid/corrupt file. Care should be taken to avoid concurrent reads to a file while it is being deleted.
    Parameters:
        file_id: "_id" of the file to delete
    '''    
    def delete(self, coll_name, file_id):
        try:
            grid_fs = gridfs.GridFS(self._db, coll_name) 
            return grid_fs.delete(file_id)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    '''
    Returns True if a matching file exists, False otherwise
    Parameters:
        document_or_id (optional): query document, or _id of the document to check for
        **kwargs (optional): keyword arguments are used as a query document, if they’re present.
    The file to check for can be specified by the value of its _id key, or by passing in a query document. A query document can be passed in as dictionary, or by using keyword arguments. Thus, the following three calls are equivalent:
    Examples:
        fs.exists({"foo": {"$gt": 12}}) or fs.exists(foo={"$gt": 12})
        fs.exists({"filename": "mike.txt"}) or fs.exists(filename="mike.txt")
    '''
    def exists(self, coll_name, document_or_id=None, **kwargs):
        try:
            grid_fs = gridfs.GridFS(self._db, coll_name) 
            return grid_fs.exists(document_or_id, **kwargs)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    '''
    Returns a cursor that iterates across files matching arbitrary queries on the files collection
    Parameters:
        filter (optional): a SON object specifying elements which must be present for a document to be included in the result set
        skip (optional): the number of files to omit (from the start of the result set) when returning the results
        limit (optional): the maximum number of results to return
        no_cursor_timeout (optional): if False (the default), any returned cursor is closed by the server after 10 minutes of inactivity. If set to True, the returned cursor will never time out on the server. Care should be taken to ensure that cursors with no_cursor_timeout turned on are properly closed.
        sort (optional): a list of (key, direction) pairs specifying the sort order for this query. See sort() for details.
    '''
    def find(self, coll_name, *args, **kwargs):
        try:
            grid_fs = gridfs.GridFS(self._db, coll_name) 
            return grid_fs.find(*args, **kwargs)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    '''
    All arguments to find() are also valid arguments for find_one()
    Returns a single GridOut, or None if no matching file is found
    Parameters:
        filter (optional): a dictionary specifying the query to be performing OR any other type to be used as the value for a query for "_id" in the file collection
        *args (optional): any additional positional arguments are the same as the arguments to find().
        **kwargs (optional): any additional keyword arguments are the same as the arguments to find().
    Examples:
        fs.find_one({"filename": "lisa.txt"})
    '''
    def find_one(self, coll_name, filter=None, *args, **kwargs):
        try:
            grid_fs = gridfs.GridFS(self._db, coll_name) 
            return grid_fs.find_one(filter, *args, **kwargs)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    '''
    Get a file from GridFS by "_id"
    Returns an instance of GridOut
    Parameters:
        file_id: "_id" of the file to get
    '''
    def get(self, coll_name, file_id):
        try:
            grid_fs = gridfs.GridFS(self._db, coll_name) 
            return grid_fs.get(file_id)
        except Exception, e:
            log.app_log.error(traceback.format_exc())
    
    '''
    Create a new file in GridFS
    Returns a new GridIn instance to which data can be written
    Parameters:
        **kwargs (optional): keyword arguments for file creation
    '''
    def new_file(self, coll_name, **kwargs):
        try:
            grid_fs = gridfs.GridFS(self._db, coll_name) 
            return grid_fs.new_file(**kwargs)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    '''
    Put data in GridFS as a new file.
    Returns the "_id" of the created file
    If the "_id" of the file is manually specified, it must not already exist in GridFS. Otherwise FileExists is raised.
    Equivalent to doing:
    try:
        f = new_file(**kwargs)
        f.write(data)
    finally:
        f.close()
    '''
    def put(self, coll_name, data, **kwargs):
        try:
            grid_fs = gridfs.GridFS(self._db, coll_name) 
            return grid_fs.put(data, **kwargs)
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    '''
    you have to replace it by deleting the file by _id, then adding the new version using the same _id.
    '''
    def update(self, coll_name, file_id, **kwargs):
        try:
            log.app_log.error('not implement')
        except Exception, e:
            log.app_log.error(traceback.format_exc())

    def count(self, coll_name, *args, **kwargs):
        try:
            grid_fs = gridfs.GridFS(self._db, coll_name) 
            return grid_fs.find(*args, **kwargs).count()
        except Exception, e:
            log.app_log.error(traceback.format_exc())

if __name__ == '__main__':
    instance = GridFsConnect()
    data = 'hello world'
    coll_name = 'fs'
    kwargs = {'_id':'1234567890', 'info':'11'}
    _id = instance.put(coll_name, data, **kwargs)
    print 'done'
