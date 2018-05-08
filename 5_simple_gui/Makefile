CXX = g++
CC  = gcc

LDFLAGS += -lrt -lpthread -ldl -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio 

#src := sample2.cpp 
src := gui.cpp 

objects=$(patsubst %.cpp,%.o, $(src))
target = gui

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

	

