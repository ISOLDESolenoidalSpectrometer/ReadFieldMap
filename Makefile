ROOTCFLAGS	:= $(shell root-config --cflags)
ROOTINC		:= $(shell root-config --prefix)/include
ROOTLIBS	:= $(shell root-config --libs)
ROOTVER		:= $(shell root-config --version | head -c1)
ROOTCXX		:= $(shell root-config --cxx)

BIN_DIR		:= .

CXX		:= $(ROOTCXX)
CFLAGS		:= -pedantic -Wall -g -O3 $(ROOTCFLAGS) 
LIBS		:= $(ROOTLIBS)

INCLUDES  	:= -I$(ROOTINC) -I./

.SUFFIXES: .cc, .hh

all: ReadFieldMap

ReadFieldMap: ReadFieldMap.cc ReadFieldMap.hh
	$(CXX) $(CFLAGS) $(INCLUDES) $< -o $(BIN_DIR)/$@ $(LIBS)

clean:
	rm -f $(BIN_DIR)

