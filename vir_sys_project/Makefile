CXX = g++
CXXFLAGS = -g -Wall -Werror
LDFLAGS =
LDLIBS =

# systemc setup
CXXFLAGS += $(shell pkg-config --cflags          systemc)
LDFLAGS  += $(shell pkg-config --libs-only-L     systemc)
LDLIBS   += $(shell pkg-config --libs-only-other systemc)
LDLIBS   += $(shell pkg-config --libs-only-l     systemc)

SOURCES = comm_func.cpp \
	  ingress_sch.cpp \
	  packet_gen.cpp \
	  test.cpp

OBJECTS = $(SOURCES:.cpp=.o)
APPNAME = test

all: $(APPNAME)

$(APPNAME): $(OBJECTS)
	$(CXX) -o $(APPNAME) $(OBJECTS) $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJECTS) $(APPNAME)

.PHONY: all clean
