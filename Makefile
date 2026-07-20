CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Isrc
BUILD    := build
TARGET   := $(BUILD)/tablo
SRCS     := $(wildcard src/*.cpp)
HDRS     := $(wildcard src/*.hpp)

RADDIR   := rad
PAPER    := $(RADDIR)/seminarski

.PHONY: all run clean pdf

all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

$(BUILD):
	mkdir -p $(BUILD)

run: $(TARGET)
	./$(TARGET)

pdf: $(PAPER).pdf

$(PAPER).pdf: $(PAPER).tex
	cd $(RADDIR) && pdflatex -interaction=nonstopmode seminarski.tex \
	  && pdflatex -interaction=nonstopmode seminarski.tex

clean:
	rm -rf $(BUILD)
	rm -f $(PAPER).aux $(PAPER).log $(PAPER).out $(PAPER).toc $(PAPER).pdf
