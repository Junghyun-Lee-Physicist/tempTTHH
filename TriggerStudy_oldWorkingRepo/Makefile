#=============================================
# Makefile for exe_TrigStudy (TrigNbTag/TriggerStudy)
#=============================================

# 1) Final executable name
PROGRAM := exe_TrigStudy

# 2) Compiler (CMSSW-provided g++ via cmsenv)
CXX := $(shell which g++)
LD  := $(CXX)

# 3) ROOT flags via root-config
ROOTCFLAGS := $(shell root-config --cflags)
ROOTLIBS   := $(shell root-config --libs)

# 4) Compilation and link flags (include local headers)
CXXFLAGS := -g -O2 -std=c++17 $(ROOTCFLAGS) -Iinclude
#LDFLAGS  := $(ROOTLIBS) -ldl
LDFLAGS  := $(ROOTLIBS)

# 5) Source files and object files under tmp/
SRCS := main.cpp \
        src/EventLooper.cpp \
        src/EventLooperWithCorrections.cpp
OBJS := $(SRCS:%.cpp=tmp/%.o)

# 6) Default target: build executable
.PHONY: all clean run
all: $(PROGRAM)

# 7) Link step: objects → executable
$(PROGRAM): $(OBJS)
	$(LD) $^ -o $@ $(LDFLAGS)

# 8) Compile rule: .cpp → tmp/.o
tmp/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 9) Run target
run: all
	./$(PROGRAM) ttJets

# 10) Clean: remove tmp directory and executable
clean:
	rm -rf tmp $(PROGRAM)

