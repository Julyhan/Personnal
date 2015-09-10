SOURCE_FILES = findContour.cpp
CFLAGS = -D__STDC_CONSTANT_MACROS -O3 -rdynamic
LDFLAGS = -lc -lopencv_core -lopencv_flann -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_objdetect -lopencv_photo -lopencv_nonfree -lopencv_features2d -lopencv_calib3d -lopencv_legacy -lopencv_contrib -lopencv_stitching -lopencv_videostab
CXX = g++

all: $(SOURCE_FILES)
	mkdir -p build
	$(CXX) $(SOURCE_FILES) -o build/main $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf build
