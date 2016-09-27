##target lib###
LIBS=libvisitor.a
DEST=../../lib

##complier##
CXX=g++
CXXFLAGS=${flags}
LDFLAGS=

##include dir and source dir##
#        -I ../../../../../../Library/funshion/src/netsvc/0.1.4/src/include \
        -I ../../../../../Library/funshion/include/boost/boost_1_40 \
        -I ../../../../../Library/funshion/include/mongodb/cxx_driver/2.0 \
        -I ../../../../../Library/funshion/include/netsvc/0.1.4 \

INCLUDE_DIRS=   \
        -I ../../../../../Library/funshion/src/kernel/0.1.2/include\
        -I ../../../../../Library/funshion/include/thread/0.1.0 \
        -I ../../../../../Library/funshion/include/msgq/0.1.0 \
        -I ../netsvc/include \
        -I ../config/include \
        -I ../util/include \
        -I ../http/include \
        -I ../ctrl/include \
        -I ../include \
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


