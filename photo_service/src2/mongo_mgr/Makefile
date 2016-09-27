##target lib###
LIBS=libmongo_mgr.a
DEST=../../lib

##complier##
CXX=g++
CXXFLAGS=${flags}
LDFLAGS=

#-I ../../../../../Library/funshion/include/mongodb/cxx_driver/2.0 \
#-I ../../../../../Library/funshion/include/boost/boost_1_40 \
        -I ../../../../../freyja/mongo/mongo_0_3_0/include/mongo \
        -I ../include/mongo \
##include dir and source dir##
INCLUDE_DIRS=   \
        -I ../../../../../Library/funshion/include/thread/0.1.0 \
        -I ../../../../../freyja/boost/boost_1_58/include \
        -I ../../../../../freyja/mongo/mongo_0_3_0/include/mongo \
        -I ../../../../../freyja/mongo/mongo_0_3_0/include \
        -I ../config/include \
        -I ../util/include \
        -I ./include \

SRC_DIRS=.

##objects##
OBJS=$(patsubst %.cpp, %.o, $(wildcard $(SRC_DIRS)/*.cpp))
SRC=$(wildcard $(SRC_DIRS)/%.cpp)

vpath %.h $(INCLUDE_DIRS)
vpath %.cpp $(SRC_DIRS)

$(LIBS):$(OBJS)

	-@if [ ! -d ${DEST} ]; then mkdir -p ${DEST}; fi
	ar r $(DEST)/$@ $^


$(OBJS):%.o:%.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< $(LDFLAGS) -o $@
    
.PHONY : clean

clean:
	rm -rf $(OBJS)


