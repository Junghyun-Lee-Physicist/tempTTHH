# =============================================================================
# Makefile for exe_TrigStudy
#
# Features:
#  1) Auto-detect OS (macOS vs Linux) and choose compiler
#  2) Build objects under OBJ_DIR while preserving subdirectories
#  3) Support both .cpp and .cc sources
#  4) Generate dependency files (.d) automatically (header change triggers rebuild)
#  5) Add helpful build logs and an 'info' target
# =============================================================================

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
PROGRAM := exe_TrigStudy
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
# SHARED_INC: analyzer 프로젝트의 include/ (SampleAlias.h / SampleRegistry.hh).
#   두 SF 패키지가 같은 sample 이름·weight 정의를 쓰게 하는 단일 소스이므로
#   복사본을 두지 않고 상위 디렉토리를 직접 include 한다.
SHARED_INC := ../include

CPPFLAGS := -I. -I$(INC_DIR) -I$(SRC_DIR) -I$(SHARED_INC) $(ROOT_CFLAGS) $(CORR_CFLAGS)

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
SRCS := TriggerSFCalculator.cpp \
        $(SRC_DIR)/EventLooper.cpp \
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

