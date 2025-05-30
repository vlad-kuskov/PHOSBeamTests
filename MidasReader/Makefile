#
# Example Makefile for ROOTANA-based projects
#

CXXFLAGS = -g -std=c++11 -O3 -Wall -Wuninitialized

# required ZLIB library

CXXFLAGS += -DHAVE_LIBZ

# required ROOTANA library

ifndef ROOTANASYS
ROOTANASYS=/home/petukhov/midas/rootana
endif

CXXFLAGS += -I$(ROOTANASYS)/include
CXXFLAGS += -I./include
LIBS += -L$(ROOTANASYS)/lib -lrootana

# ROOT libraries

ifdef ROOTSYS

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTFEATURES := $(shell root-config --features)
ROOTGLIBS    := $(shell root-config --glibs) -lThread
HAVE_ROOT_HTTP := $(findstring http,$(ROOTFEATURES))
HAVE_ROOT_XML  := $(findstring xml,$(ROOTFEATURES))

CXXFLAGS  += -DHAVE_ROOT $(ROOTCFLAGS)

ifdef HAVE_ROOT_XML
CXXFLAGS  += -DHAVE_ROOT_XML
ROOTGLIBS += -lXMLParser
endif

ifdef HAVE_ROOT_HTTP
CXXFLAGS  += -DHAVE_ROOT_HTTP -DHAVE_THTTP_SERVER
ROOTGLIBS += -lRHTTP
endif

endif # ROOTSYS

# optional MIDAS library

ifdef MIDASSYS

MIDASLIBS = $(MIDASSYS)/linux/lib/libmidas.a -lutil -lrt
CXXFLAGS += -DHAVE_MIDAS -DOS_LINUX -Dextname -I$(MIDASSYS)/include

UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
MIDASLIBS = $(MIDASSYS)/darwin/lib/libmidas.a
endif

LIBS += $(MIDASLIBS)

endif # MIDASSYS

# OBJS:=
# OBJS += TV792Histogram.o TV1190Histogram.o
# OBJS += TL2249Histogram.o TAgilentHistogram.o
# OBJS += TV1720Waveform.o TDT724Waveform.o
# OBJS += TV1730DppWaveform.o TV1730RawWaveform.o
# OBJS += TV1720Correlations.o TTRB3Histogram.o
# OBJS += TCamacADCHistogram.o
# OBJS += TAnaManager.o

# all: $(OBJS) ana.exe anaDisplay.exe midas2root.exe

# ana.exe: ana.cxx $(OBJS) 
# 	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

# anaDisplay.exe: anaDisplay.cxx $(OBJS) 
# 	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

# midas2root.exe: midas2root.cxx $(OBJS) 
# 	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

MidasReader.exe: MidasReader.cxx  
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

Midas2TTreeConverter.exe: Midas2TTreeConverter.cxx
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

%.o: %.cxx
	$(CXX) -o $@ $(CXXFLAGS) -c $<

dox:
	doxygen

clean::
	-rm -f *.o *.a
	-rm -f *.exe
	-rm -rf *.exe.dSYM

# end
