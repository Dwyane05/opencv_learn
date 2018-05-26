CXX = g++
CC  = gcc

LDFLAGS += -lrt -lpthread -ldl -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc -lopencv_xfeatures2d -lopencv_features2d -lopencv_flann -lopencv_calib3d -lopencv_ml

#src := sample.cpp 
#src := mouse_roi.cpp
#src := flann.cpp
src := findRect.cpp

#src += utils/MultipleImageWindow.cpp
#CFLAGS += -I${PWD}/utils

objects=$(patsubst %.cpp,%.o, $(src))
target = wd_app

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

	

