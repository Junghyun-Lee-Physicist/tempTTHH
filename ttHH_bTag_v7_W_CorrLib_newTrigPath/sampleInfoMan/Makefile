# 컴파일러 및 기본 옵션
CC      = g++
CFLAGS  = -std=c++17 -Wall -O2 -fPIC -Iinclude

AR      = ar
ARFLAGS = rcs

# 디렉토리 설정
SRCDIR  = src
INCDIR  = include
OBJDIR  = obj
LIBDIR  = lib

# 라이브러리 이름 (생성될 파일: libSampleInfoManager.a)
LIBNAME = libSampleInfoManager.a

# src 디렉토리 내의 모든 .cpp 파일
SRCS    = $(wildcard $(SRCDIR)/*.cpp)
# 각 .cpp 파일을 obj 디렉토리 내의 .o 파일로 변환
OBJS    = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

all: $(LIBDIR)/$(LIBNAME)
	@echo "Deleting intermediate object files..."
	rm -rf $(OBJDIR)

$(LIBDIR)/$(LIBNAME): $(OBJS)
	@mkdir -p $(LIBDIR)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	@echo "Library created: $@"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(LIBDIR)/$(LIBNAME)
	@echo "Cleaned build files."

.PHONY: all clean
