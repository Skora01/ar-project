CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Isrc
BUILD    := build
TARGET   := $(BUILD)/tablo
SRCS     := $(wildcard src/*.cpp)
HDRS     := $(wildcard src/*.hpp)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

$(BUILD):
	mkdir -p $(BUILD)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD)
