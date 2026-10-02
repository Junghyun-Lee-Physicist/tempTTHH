# ============================================================================
# Makefile for B-tag Shape SF Processor + Reweight JSON Writer
#
# Prerequisites:
#   - ROOT 6 (with root-config in PATH)
#   - correctionlib (with correction in PATH, pip install correctionlib)
#   - nlohmann/json (header-only, in include/nlohmann/)
#
# Usage:
#   make                  # build both executables
#   make exe_BTagSF       # build only the reweight processor
#   make exe_MakeJSON     # build only the JSON writer
#   make clean            # remove build artifacts
#
# Author: Junghyun Lee
# ============================================================================

# -----------------------------------------------------------------------------
# 1) OS Detection & Compiler Setting
# -----------------------------------------------------------------------------
UNAME_S := $(shell uname -s)

# [2026-07-29] 컴파일러를 PATH 의 `clang++` 로 무조건 고정하지 않는다.
#
#   문제: macOS 에서 PATH 에 Homebrew llvm / conda / venv 의 clang 이 있으면
#   그게 잡힌다. 그런데 include 되는 libc++ 헤더는 CommandLineTools SDK 의 것
#   (최신)이라, 헤더가 요구하는 `__builtin_ctzg` / `__builtin_clzg`(Clang 19+)
#   를 구버전 컴파일러가 몰라서 **libc++ 헤더 안에서** 에러가 쏟아진다:
#       error: use of undeclared identifier '__builtin_ctzg'
#       error: static assertion failed: Cannot rebind pointer; ...
#   우리 코드와 무관한데 우리 코드 탓처럼 보이는 종류의 실패다.
#
#   그래서 macOS 기본값은 **SDK 와 짝이 맞는 Apple clang** 절대경로로 둔다.
#   `$(origin CXX)` 가 default 일 때만 덮어쓰므로, 환경변수나
#   `make CXX=... LD=...` 로 준 값은 그대로 존중된다.
ifeq ($(UNAME_S),Darwin)
  ifeq ($(origin CXX),default)
    CXX := /usr/bin/clang++
  endif
  ifeq ($(origin LD),default)
    LD  := $(CXX)
  endif
  $(info >>> [System] macOS detected. CXX = $(CXX))
else
  ifeq ($(origin CXX),default)
    CXX := g++
  endif
  ifeq ($(origin LD),default)
    LD  := $(CXX)
  endif
  $(info >>> [System] Linux detected. CXX = $(CXX))
endif
$(info >>> [System] $(shell $(CXX) --version 2>/dev/null | head -1))

# -----------------------------------------------------------------------------
# 2) Configuration
# -----------------------------------------------------------------------------
INC_DIR := include
SRC_DIR := src
OBJ_DIR := tmp

PROGRAMS := exe_BTagSF exe_MakeJSON

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
# SHARED_INC: analyzer 프로젝트의 include/ (SampleAlias.h / SampleRegistry.hh).
#   TriggerStudy 와 **같은 헤더**를 보게 해서 두 도구의 정규화가 갈라지지 않게 한다.
SHARED_INC := ../include

CPPFLAGS := -I. -I$(INC_DIR) -I$(SRC_DIR) -I$(SHARED_INC) $(ROOT_CFLAGS) $(CORR_CFLAGS)
CXXFLAGS := -g -O2 -std=c++17 -Wall -fPIC -MMD -MP
LDFLAGS  := $(ROOT_LIBS) $(CORR_LDFLAGS)

ifneq ($(strip $(CORR_LIB_PATH)),)
  LDFLAGS += -Wl,-rpath,$(CORR_LIB_PATH)
endif


# -----------------------------------------------------------------------------
# 5) Source & Object Files
# -----------------------------------------------------------------------------
vpath %.cpp . $(SRC_DIR)
vpath %.cc  . $(SRC_DIR)

# exe_BTagSF: full reweight processor
OBJS_BTAG := $(OBJ_DIR)/run_BTagSF.o \
             $(OBJ_DIR)/BTagSFProcessor.o \
             $(OBJ_DIR)/NtupleReader.o

# exe_MakeJSON: correctionlib JSON writer (no NtupleReader needed)
OBJS_JSON := $(OBJ_DIR)/makeReweightJSON.o

# All dependency files
DEPS := $(OBJS_BTAG:.o=.d) $(OBJS_JSON:.o=.d)


# -----------------------------------------------------------------------------
# 6) Build Rules
# -----------------------------------------------------------------------------
.PHONY: all clean info

all: $(PROGRAMS)
	@echo ">>> [Done] Build complete: $(PROGRAMS)"

# ── exe_BTagSF ──
exe_BTagSF: $(OBJS_BTAG)
	@echo ">>> [Link] $@"
	$(LD) $(OBJS_BTAG) -o $@ $(LDFLAGS)

# ── exe_MakeJSON ──
exe_MakeJSON: $(OBJS_JSON)
	@echo ">>> [Link] $@"
	$(LD) $(OBJS_JSON) -o $@ $(LDFLAGS)

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

info:
	@echo ">>> [Info] PROGRAMS    = $(PROGRAMS)"
	@echo ">>> [Info] UNAME_S     = $(UNAME_S)"
	@echo ">>> [Info] CXX/LD      = $(CXX) / $(LD)"
	@echo ">>> [Info] OBJS_BTAG   = $(OBJS_BTAG)"
	@echo ">>> [Info] OBJS_JSON   = $(OBJS_JSON)"
	@echo ">>> [Info] CPPFLAGS    = $(CPPFLAGS)"
	@echo ">>> [Info] LDFLAGS     = $(LDFLAGS)"

clean:
	@echo ">>> [Clean] Removing build artifacts..."
	rm -rf $(OBJ_DIR) $(PROGRAMS)

-include $(DEPS)
