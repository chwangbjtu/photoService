##target lib###
CXX=g++
TARGET=photo_service
FUNSHION_LIB := ../../../../Library/funshion/lib
FREYJA_LIB := ../../../../freyja
LOGINSERVER_LIB := ../lib
DEST_ETC := ../bin/etc/
DEST_LOG := ../bin/log/


OPENCV_LIB := ../../../../freyja/opencv/opencv-2.4.9/lib

LIB := $(LOGINSERVER_LIB)/libservice.a
LIB += $(LOGINSERVER_LIB)/libvisitor.a
LIB += $(LOGINSERVER_LIB)/libmongo_mgr.a
LIB += $(LOGINSERVER_LIB)/libctrl.a
LIB += $(LOGINSERVER_LIB)/libhttp.a
LIB += $(LOGINSERVER_LIB)/libsession.a
LIB += $(LOGINSERVER_LIB)/libconfig.a
LIB += $(LOGINSERVER_LIB)/libutil.a
LIB += $(LOGINSERVER_LIB)/libnetsvc.a
LIB += $(LOGINSERVER_LIB)/libjson_linux-gcc-4.1.2_libmt.a
#LIB += $(LOGINSERVER_LIB)/libmongoclient.a 
LIB += $(FREYJA_LIB)/mongo/mongo_0_3_0/lib/libmongoclient.a
LIB += $(FUNSHION_LIB)/kernel/0.1.2/libkernel.a
#LIB += $(FUNSHION_LIB)/netsvc/0.1.4/libnetsvc.a
LIB += $(FUNSHION_LIB)/msgq/0.1.0/libmsgq.a 
LIB += $(FUNSHION_LIB)/logger/0.2.0/liblogger.a
LIB += $(FUNSHION_LIB)/timer/0.1.0/libtimer.a 
LIB += $(FUNSHION_LIB)/thread/0.1.0/libthread.a 
LIB += $(FUNSHION_LIB)/zlib/1.2.4/libz.a
LIB += $(FREYJA_LIB)/boost/boost_1_58/lib/libboost_system.a
LIB += $(FREYJA_LIB)/boost/boost_1_58/lib/libboost_regex.a
LIB += $(FREYJA_LIB)/boost/boost_1_58/lib/libboost_thread-mt.a

HEADER_DIR += ../../../../Library/funshion/include/kernel/0.1.2/
HEADER_DIR += ../../../../Library/funshion/include/logger/0.2.0/
HEADER_DIR += ../../../../Library/funshion/include/thread/0.1.0/
HEADER_DIR += ../../../../Library/funshion/include/timer/0.1.0 /
HEADER_DIR += ../../../../Library/funshion/include/msgq/0.1.0/
HEADER_DIR += ../../../../freyja/boost/boost_1_58/include
HEADER_DIR += ../../../../freyja/mongo/mongo_0_3_0/include/mongo
HEADER_DIR += ../../../../freyja/mongo/mongo_0_3_0/include/
HEADER_DIR += netsvc/include/
HEADER_DIR += util/include/
HEADER_DIR += config/include/
HEADER_DIR += session_mgr/include/
HEADER_DIR += ctrl/include/
HEADER_DIR += mongo_mgr/include/
HEADER_DIR += visitor/include/
HEADER_DIR += service/include/
HEADER_DIR += ../include/
HEADER_DIR += ./include/


INCLUDE := $(addprefix -I ,$(HEADER_DIR)) 

CXXFLAGS :=${flags} -DMAKEFILEBUILD_DATE="\"`date '+%Y-%m-%d %H:%M:%S'`"\"

SRCS := $(wildcard *.cpp $(SRCDIR)/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

$(TARGET):$(OBJS)
	#$(CXX) $(CXXFLAGS) $^ -o $@  $(LIB)  -lpthread -lm -lrt -ldl -lcrypto -lssl -lmagic `pkg-config opencv --cflags --libs`
	#$(CXX) $(CXXFLAGS) $^ -o $@  $(LIB)  -lpthread -lm -lrt -ldl -lcrypto -lssl -lmagic -ldc1394 -lxml2 -lgstreamer-0.10 -lgstbase-0.10 -lglib-2.0 -lgthread-2.0 -lgmodule-2.0 -lgobject-2.0 -lfontconfig -lfreetype -lpango-1.0 -lcairo -lgdk_pixbuf-2.0 -lpangocairo-1.0 -lpangoft2-1.0 -lgio-2.0 -latk-1.0 -lgdk-x11-2.0 -lgtk-x11-2.0  -lstdc++  
	$(CXX) $(CXXFLAGS) $^ -o $@  $(LIB)  -lpthread -lm -lrt -ldl -lcrypto -lssl -lmagic -lpng -ljpeg -lz
	cp $(TARGET) ../bin/
	-@if [ ! -d ${DEST_ETC} ]; then mkdir -p ${DEST_ETC}; fi
	-@if [ ! -d ${DEST_LOG} ]; then mkdir -p ${DEST_LOG}; fi
	#cp -f ../etc/* $(DEST_ETC)

$(OBJS):%.o:%.cpp 
	$(CXX) $(INCLUDE) $(CXXFLAGS) -c $< -o $@    

echo:
	echo $(SRCS)
clean:
	-rm -rf $(TARGET) *.o	

check:
	cppcheck -j 4 --enable=all $(INCLUDE) -f -q . 2>cppcheck.log

