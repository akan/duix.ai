CC = g++

CPPFLAGS = -g -fPIC -Wall -std=c++17 \
		   -Ithird/x86/include/ \
		   -Ithird/x86/include/ncnn \
		   -Iinclude -Iinclude/csf -I/usr/include/opencv4/opencv2 \
		   -Iinclude/onnx -Iinclude/pplcv  -I/usr/include/opencv4/  \

LIBS = -Llib \
	   -Lthird/x86/lib	\
	   -L/opt/opencv4/lib \
		-lopencv_core -lopencv_dnn -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_videoio \
		-lonnxruntime -lncnn

#-DUSE_X11

PROM = alltest
SA += lib/libpplcv_static.a
SA += lib/libpplcommon_static.a

DEPS = $(shell find ./include -name "*.h")
DEPS = $(shell find ./include -name "*.hpp")
SRC = $(shell find ./src -name "*.cpp")
SRC += $(shell find ./src -name "*.c")

OBJS = $(SRC:%.cpp=%.o)

all: $(PROM)

$(PROM): $(OBJS)
	$(CC) $(CPPFLAGS) -o $(PROM) $(OBJS) $(SA) $(LIBS)

#%.o: %.cpp $(DEPS)
%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) alltest
