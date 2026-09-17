CXX ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Iinclude
TARGET ?= build/backend.exe

SRCS = src/main.cpp \
       src/memory_tracer.cpp \
       src/benchmark.cpp \
       src/algorithms/sorting.cpp \
       src/algorithms/search.cpp \
       src/algorithms/graphs.cpp

OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))

LDFLAGS ?= -static

ifeq ($(OS),Windows_NT)
    MKDIR_P = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
    RM_RF   = if exist build rmdir /s /q build
else
    MKDIR_P = mkdir -p $(1)
    RM_RF   = rm -rf build
endif

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(call MKDIR_P,$(dir $@))
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

build/%.o: src/%.cpp
	@$(call MKDIR_P,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM_RF)

run: $(TARGET)
	./$(TARGET)

