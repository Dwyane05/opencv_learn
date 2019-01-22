CXX = g++
CC  = gcc

CFLAGS := -O0 -g -c -Wall -Wno-deprecated-declarations -std=c++11 -fPIC -m64 -ffunction-sections
CFLAGS +=  -DDEBUG

LDFLAGS += -lrt -lpthread -ldl
#LDFLAGS += -lIlmImf  -lHalf -L/usr/lib/x86_64-linux-gnu
LDFLAGS += $(shell pkg-config --cflags --libs opencv) 

# for optimizing, close it for debug
#CFLAGS += -O 
#CFLAGS += -Os
#CFLAGS += -std=gnu++11

src := createsamples.cpp  utility.cpp

#src += opencv_done/opencv_done.cpp 
#CFLAGS += -I${PWD}/opencv_done


objects=$(patsubst %.cpp,%.o, $(src))
target = create_samples

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

	

