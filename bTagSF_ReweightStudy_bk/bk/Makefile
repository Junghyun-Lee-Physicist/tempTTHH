# ============================================================================
# Makefile for B-tag Shape SF Processor
#
# Prerequisites:
#   - ROOT 6 (with root-config in PATH)
#   - correctionlib (with correction in PATH, pip install correctionlib)
#
# Usage:
#   make              # build exe_BTagSF
#   make clean        # remove build artifacts
#
# Author: Junghyun Lee
# ============================================================================

# -----------------------------------------------------------------------------
# 1) OS Detection & Compiler Setting
# -----------------------------------------------------------------------------
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
  CXX := clang++
  LD  := clang++
  $(info >>> [System] macOS detected. Using clang++.)
else
  CXX := g++
  LD  := g++
  $(info >>> [System] Linux detected. Using g++.)
endif

# -----------------------------------------------------------------------------
# 2) Configuration
# -----------------------------------------------------------------------------
PROGRAM := exe_BTagSF
INC_DIR := include
SRC_DIR := src
OBJ_DIR := tmp


# -----------------------------------------------------------------------------
# 3) Flags from external tools
# -----------------------------------------------------------------------------
ROOT_CFLAGS := $(shell root-config --cflags)
ROOT_LIBS   := $(shell root-config --libs)

# CorrectionLib flags (may be empty if 'correction' is not available)
CORR_CFLAGS  := $(shell correction config --cflags 2>/dev/null)
CORR_LDFLAGS := $(shell correction config --ldflags 2>/dev/null)

# CorrectionLib runtime library path (for macOS rpath issues)
CORR_LIB_PATH := $(shell python3 -c "import correctionlib, os; print(os.path.join(os.path.dirname(correctionlib.__file__), 'lib'))" 2>/dev/null)


# -----------------------------------------------------------------------------
# 4) Compiler & Linker Flags
# -----------------------------------------------------------------------------
# CPPFLAGS: preprocessor flags (include paths, defines)
CPPFLAGS := -I. -I$(INC_DIR) -I$(SRC_DIR) $(ROOT_CFLAGS) $(CORR_CFLAGS)

# CXXFLAGS: compilation flags
CXXFLAGS := -g -O2 -std=c++17 -Wall -fPIC -MMD -MP

# LDFLAGS: linker flags (rpath, etc.)
# Note: ROOT_LIBS typically already contains needed ROOT libraries and rpath (brew)
LDFLAGS := $(ROOT_LIBS) $(CORR_LDFLAGS)

# Add rpath for correctionlib if path was found
ifneq ($(strip $(CORR_LIB_PATH)),)
  LDFLAGS += -Wl,-rpath,$(CORR_LIB_PATH)
endif


# -----------------------------------------------------------------------------
# 5) Source & Object Files
# -----------------------------------------------------------------------------
SRCS := run_BTagSF.cpp \
        $(SRC_DIR)/BTagSFProcessor.cpp \
        $(SRC_DIR)/NtupleReader.cc

# Map sources to objects under OBJ_DIR while preserving directories
# e.g. src/foo.cc -> tmp/src/foo.o
OBJS := $(addprefix $(OBJ_DIR)/,$(SRCS))
OBJS := $(OBJS:.cpp=.o)
OBJS := $(OBJS:.cc=.o)

# Dependency files produced by -MMD -MP
DEPS := $(OBJS:.o=.d)


# -----------------------------------------------------------------------------
# 6) Build Rules
# -----------------------------------------------------------------------------
.PHONY: all clean info

all: $(PROGRAM)
	@echo ">>> [Done] Build complete: $(PROGRAM)"

# Link step
$(PROGRAM): $(OBJS)
	@echo ">>> [Link] Linking executable: $@"
	@echo ">>> [Link] Objects: $(OBJS)"
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

# Compile rules for .cpp
$(OBJ_DIR)/%.o: %.cpp
	@echo ">>> [Compile] $<"
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Compile rules for .cc
$(OBJ_DIR)/%.o: %.cc
	@echo ">>> [Compile] $<"
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Print configuration (useful for debugging)
info:
	@echo ">>> [Info] PROGRAM     = $(PROGRAM)"
	@echo ">>> [Info] UNAME_S     = $(UNAME_S)"
	@echo ">>> [Info] CXX/LD      = $(CXX) / $(LD)"
	@echo ">>> [Info] INC_DIR     = $(INC_DIR)"
	@echo ">>> [Info] SRC_DIR     = $(SRC_DIR)"
	@echo ">>> [Info] OBJ_DIR     = $(OBJ_DIR)"
	@echo ">>> [Info] SRCS        = $(SRCS)"
	@echo ">>> [Info] OBJS        = $(OBJS)"
	@echo ">>> [Info] CPPFLAGS    = $(CPPFLAGS)"
	@echo ">>> [Info] CXXFLAGS    = $(CXXFLAGS)"
	@echo ">>> [Info] LDFLAGS     = $(LDFLAGS)"
	@echo ">>> [Info] CORR_LIB_PATH = $(CORR_LIB_PATH)"

clean:
	@echo ">>> [Clean] Removing output files..."
	rm -rf $(OBJ_DIR) $(PROGRAM)

# Include dependency files if they exist
-include $(DEPS)

