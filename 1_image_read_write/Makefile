ARCH?=arm

TOPDIR=${PWD}

CFLAGS := -Wall 
#CFLAGS += -I${PWD} -Wno-psabi
#CFLAGS += -I${PWD}/../include
CFLAGS +=  -DDEBUG

Q=@

CROSS_COMPILE= /opt/ingenic_compiler/mips-gcc472-glibc216/bin/mips-linux-gnu-

#CROSS_COMPILE=/opt/allwiner_compiler/external-toolchain/bin/arm-linux-gnueabi-
LIBS_PATH=/opt/yoko_libs


CXX=${CROSS_COMPILE}g++
CC=${CROSS_COMPILE}gcc
#CXX=g++
#CC=gcc
#libopencv
CFLAGS += -I${LIBS_PATH}/ingenic/opencv320/include
LDFLAGS += -lopencv_core -lopencv_video -lopencv_videoio  -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -L${LIBS_PATH}/ingenic/opencv320/lib
#

# for optimizing, close it for debug
#CFLAGS += -O 
#CFLAGS += -Os
#CFLAGS += -std=gnu++11
CFLAGS += -std=c++11

LDFLAGS += -lrt -lpthread -ldl
#LDFLAGS +=  -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_video -lopencv_videoio  -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_xfeatures2d  

ifdef FULL
CFLAGS_FULL:= -Wextra -Wfloat-equal -save-temps 
else
CFLAGS_FULL:=
endif



src := image.cpp 


objects=$(patsubst %.cpp,%.o, $(src))
target = image_test

.PHONY:all
all:${target}

${target}:$(objects)
	$(Q)$(CXX) $^ ${LDFLAGS} -o $@
	
%.o:%.cpp
	$(Q)$(CXX) -c $^ ${CFLAGS} ${CFLAGS_FULL} -o $@

.PHONY:clean
clean:
	@rm -f  ${target}
	@find -name "*.o" -execdir rm {} \;

	

