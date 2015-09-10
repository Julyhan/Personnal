SOURCE_FILES = MorphoFeatures.cpp
CFLAGS = -D__STDC_CONSTANT_MACROS -O3 -rdynamic
LDFLAGS = -lc -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video -lopencv_calib3d
CXX = g++

all: $(SOURCE_FILES)
	mkdir -p build
	$(CXX) $(SOURCE_FILES) -o build/main $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf build
