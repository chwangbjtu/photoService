#/bin/sh

build()
{
   make clean -C $1
   make -C $1 flags=-g
}


build ./netsvc/
build ./util/
build ./http/
build ./visitor/
build ./config/
build ./session_mgr/
build ./mongo_mgr/
build ./ctrl/
build ./service/
build ./
