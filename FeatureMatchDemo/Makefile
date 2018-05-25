CXX = g++
CC  = gcc

LDFLAGS += -lrt -lpthread -ldl -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc -lopencv_xfeatures2d -lopencv_features2d -lopencv_calib3d

CFLAGS += -std=c++11

#src := sample1.cpp 
src :=main.cpp  

src += FeatureMatchTest.cpp   
#  Triangulation.cpp  

#src += utils/MultipleImageWindow.cpp
#CFLAGS += -I${PWD}/utils

objects=$(patsubst %.cpp,%.o, $(src))
target = surf

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

	

