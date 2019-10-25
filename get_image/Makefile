######
######   What are we building?
######

TARGET = get_image

# Objects that must be built in order to link

OBJECTS = main.o
OBJECTS += ../util/cv_painter.o

######
######   Binaries and flags
######

CPPFLAGS = -std=c++11
CPPFLAGS += -Wall

CPPFLAGS += -DLOG_TAG=MYNTEYE
CPPFLAGS += -DWITH_OPENCV
CPPFLAGS += -I/usr/include/opencv4
CPPFLAGS += -I../

CPPFLAGS += -O3
#CPPFLAGS += -g

LD = g++

LDFLAGS = -Wall -O3 -std=c++11     -rdynamic
LDFLAGS += -Wl,-rpath,/home/fassler/rpmbuild/BUILD/slightech/MYNT-EYE-D-SDK/3rdparty/eSPDI/linux/x64
#LDFLAGS += -Wl,-rpath,/usr/lib64/3rdparty

LDLIBS = -lmynteye_depth
LDLIBS += $(shell pkg-config --libs opencv4)
LDLIBS += -leSPDI


# Default target:
.PHONY: all
all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)

