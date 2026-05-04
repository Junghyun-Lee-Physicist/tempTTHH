# -----------------------------------------------------------------------------
# Build Process Description:
# 1. Generate ROOT dictionary source (dictionary.cc) from headers:
#      include/tnm.h and include/linkdef.h
# 2. Compile all library sources in src/ and the dictionary into object files.
# 3. Link these objects with ROOT libraries to create
#      shared library lib$(LIB_NAME)$(SHARED_LIB_EXT).
# 4. Compile application sources at the project root
#      and link them against ROOT and lib$(LIB_NAME).
#
# Usage:
#   make                # builds everything quietly
#   make VERBOSE=1      # prints detailed info messages
#
# Shared headers from b-tag SF reweight tool:
#   The header `Config_TtCatGroup.hh` (process group mapping for b-tag
#   normalization SF, ttH AN App. A.2.1) is a single-source-of-truth shared
#   between the b-tag SF tool and this analyzer. Resolved via the variable
#   BTAGSF_INCDIR (default: ../bTagSF_ReweightStudy/include relative to this
#   Makefile's directory). Override at the command line if your tool lives
#   elsewhere:
#       make BTAGSF_INCDIR=/abs/path/to/bTagSF_ReweightStudy/include
#   or export it from your environment.
# -----------------------------------------------------------------------------

ifndef ROOTSYS
  $(error *** Please set up ROOTSYS (e.g. source /path/to/thisroot.sh))
endif

# ----------------------
#   Verbosity Control
# ----------------------
ifdef VERBOSE
INFO := @echo
$(info [Log Verbosity] : Verbose log output enabled)
else
INFO := @:
$(info [Log Verbosity] : Verbose log output disabled)
endif
HIDE := @

# ----------------------
#   Configuration
# ----------------------
CXX            := g++
ROOT_CFLAGS    := $(shell root-config --cflags)
ROOT_LIBS      := $(shell root-config --libs)

# ───────────────────────────────
#  correctionlib C++ component flags
#  (ensure correctionlib is installed & in your PATH)
# ───────────────────────────────
CORR_CFLAGS    := $(shell correction config --cflags)
CORR_LDFLAGS   := $(shell correction config --ldflags)

# Log the correctionlib flags
$(info [CorrectionLib] CFLAGS: $(CORR_CFLAGS))
$(info [CorrectionLib] LDFLAGS: $(CORR_LDFLAGS))

# ───────────────────────────────
#  Shared headers from b-tag SF reweight tool
#  (single source of truth for Config_TtCatGroup.hh — ttH AN App. A.2.1)
# ───────────────────────────────
# Default location: sibling directory of analyzer.
# Override at the command line:  make BTAGSF_INCDIR=/abs/path/to/include
BTAGSF_INCDIR  ?= ./bTagSF_ReweightStudy/include

# Sanity check: warn (don't fail) if the expected header is missing.
# This lets developers who don't use the shared header still build.
ifeq ($(wildcard $(BTAGSF_INCDIR)/Config_TtCatGroup.hh),)
  $(warning [BTagSF Headers] Config_TtCatGroup.hh NOT FOUND in $(BTAGSF_INCDIR))
  $(warning [BTagSF Headers]   → Set BTAGSF_INCDIR to the b-tag SF tool's include/ dir.)
  $(warning [BTagSF Headers]   → Build will fail if any source #include's it.)
else
  $(info [BTagSF Headers] Using shared header from $(BTAGSF_INCDIR))
endif

SRC_DIR        := src
OBJ_DIR        := tmp
LIB_DIR        := lib
INC_DIR        := include
LIB_NAME       := ToolsForAnalysis

# Include ROOT, correctionlib, AND the shared b-tag SF headers
INCLUDE_FLAGS  := -I. -I$(INC_DIR) -I$(SRC_DIR) -I$(BTAGSF_INCDIR) \
                  $(ROOT_CFLAGS) $(CORR_CFLAGS)
COMPILE_FLAGS  := -c -O2 -Wall -fPIC

# Link against ROOT, correctionlib, and our shared library
GENERAL_LINK   := $(ROOT_LIBS) $(CORR_LDFLAGS) -L$(LIB_DIR)

# Detect platform and set shared library variables
OS_Name        := $(shell uname -s)
ifeq ($(OS_Name),Darwin)
	SHARED_LINK_CMD := $(CXX) -dynamiclib
	SHARED_LIB_EXT  := .dylib
else
	SHARED_LINK_CMD := $(CXX) -shared
	SHARED_LIB_EXT  := .so
endif

SHARED_LIB     := $(LIB_DIR)/lib$(LIB_NAME)$(SHARED_LIB_EXT)

# Source and target lists
DICT_HEADER    := $(INC_DIR)/tnm.h
DICT_LINKDEF   := $(INC_DIR)/linkdef.h
DICT_SOURCE    := $(SRC_DIR)/dictionaryForROOT.cc

APP_SOURCES    := $(wildcard *.cc)
APPLICATIONS   := $(APP_SOURCES:.cc=)

LIB_SOURCES    := $(filter-out $(DICT_SOURCE), $(wildcard $(SRC_DIR)/*.cc))
LIB_OBJECTS    := $(patsubst $(SRC_DIR)/%.cc,$(OBJ_DIR)/%.o,$(LIB_SOURCES) $(DICT_SOURCE))

# Default target
all: prepare_dirs $(SHARED_LIB) $(APPLICATIONS)
	$(INFO) "[Done] Built shared lib=$(SHARED_LIB) and apps=$(APPLICATIONS)"

.PHONY: all clean prepare_dirs

prepare_dirs:
	$(HIDE) mkdir -p $(OBJ_DIR) $(LIB_DIR)

# ----------------------
#   Dictionary Generation
# ----------------------
$(DICT_SOURCE): $(DICT_HEADER) $(DICT_LINKDEF)
	$(INFO) "[Dict] Generating ROOT dictionary source"
##	$(HIDE) rootcling -f $(DICT_SOURCE) \
##                      -I$(INC_DIR) -I$(ROOTSYS)/include \
##                      $(DICT_HEADER) $(DICT_LINKDEF)
	$(INFO) "[Dict] Generating ROOT dictionary source"
	$(HIDE) rootcling -f $(DICT_SOURCE) \
             -I$(INC_DIR) -I$(ROOTSYS)/include \
             $(DICT_HEADER) $(DICT_LINKDEF)

# ----------------------
#   Library Compilation
# ----------------------
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | prepare_dirs
	$(INFO) "[Lib] Compiling $< -> $@"
	$(HIDE) $(CXX) $(COMPILE_FLAGS) $(INCLUDE_FLAGS) $< -o $@

# Link shared library with ROOT and correctionlib
$(SHARED_LIB): $(LIB_OBJECTS) | prepare_dirs
	$(INFO) "[Lib] Linking shared library $@"
	$(HIDE) $(SHARED_LINK_CMD) -fPIC \
           $(LIB_OBJECTS) $(GENERAL_LINK) -o $@

# ----------------------
#   Application Build
# ----------------------
$(OBJ_DIR)/%.o: %.cc | prepare_dirs
	$(INFO) "[App] Compiling application $< -> $@"
	$(HIDE) $(CXX) $(COMPILE_FLAGS) $(INCLUDE_FLAGS) $< -o $@

$(APPLICATIONS): % : $(OBJ_DIR)/%.o $(SHARED_LIB)
	$(INFO) "[App] Linking application $@"
	$(HIDE) $(CXX) -fPIC $(OBJ_DIR)/$@.o -o $@ \
                  $(GENERAL_LINK) -l$(LIB_NAME)

# ----------------------
#   Clean
# ----------------------
clean:
	$(HIDE) rm -rf $(OBJ_DIR) $(LIB_DIR) $(SRC_DIR)/dictionaryForROOT.cc $(APPLICATIONS)
