# Makefile for DeepEfficiency (C++ parts)
#
#
# Run with: make
#
#
# USE [TABS] for intendation while modifying this!
#
# mikael.mieskolainen@cern.ch, 26/07/2018
# ------------------------------------------------------------------------

# ROOT installation path
ROOTLIBDIR   = $(shell root-config --libdir)


# ------------------------------------------------------------------------
# Libraries

# ROOT
ROOTlib      = -L$(ROOTLIBDIR) -lCore -lCore -lCint -lRIO -lNet \
               -lHist -lGraf -lGraf3d -lGpad -lTree -lRint \
               -lPostscript -lMatrix -lPhysics -lMathCore \
               -lThread -lGui -lRooFit -lMinuit

# C++ standard
STANDARDlib  = -pthread -rdynamic -lm -ldl -lrt


# ------------------------------------------------------------------------
# Compiler options

CXX       = g++

INCLUDES  = -Iinclude -Ilib
INCLUDES +=  $(STANDARDlib)
INCLUDES += -I/usr/include
INCLUDES += -I$(ROOTSYS)/include

CXXFLAGS  = -ansi -pedantic -Wall -pipe -march=native -O2 -ftree-vectorize -std=c++17 $(INCLUDES)

# CPU optimization  with -march=native
# Autovectorization with -free-vectorize
# Floating point super-optimization with -ffast-math (fast but breaks floating point standards!)
# -O2 is safe, -O3 usually too
# Faster compilation with -pipe
# Profiling with -pg (REMEMBER TO REMOVE, MAKES A SIGNIFICANT PERFORMANCE HIT)

# ------------------------------------------------------------------------
# Source and objects

SRC_DIR = src
OBJ_DIR = obj

SRC     = $(wildcard $(SRC_DIR)/*.cc)
OBJ     = $(SRC:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

LINK_LIBS += $(ROOTlib)

# ------------------------------------------------------------------------

.SUFFIXES:      .o .cc
all:	libraries deeplot


# Object files
libraries: $(OBJ)

# Programs
deeplot: deeplot.o $(OBJ)
	$(CXX) $@.o $(OBJ) $(LINK_LIBS) -o $@ $(CXXFLAGS)


# ------------------------------------------------------------------------
# Compile objects (.o) from sources (.cc)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@


# ------------------------------------------------------------------------
# Remove any object files
clean:
	rm *.o
	rm $(OBJ_DIR)/*.o

