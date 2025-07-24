# Cross-platform Makefile for MSYS2 UCRT64 environment

ROOT_DIR      := $(CURDIR)
BUILD_DIR     := $(ROOT_DIR)/build
BIN_DIR_DEBUG := $(BUILD_DIR)/bin/Debug
BIN_DIR_REL   := $(BUILD_DIR)/bin/Release
EXE_NAME      := minesweeper.exe

CMAKE         := cmake
MAKE          := make
CC            := gcc
CXX           := g++

.PHONY: all setup debug release clean run

all: debug

setup:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) -G "Unix Makefiles" -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_BUILD_TYPE=Debug ..

debug:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
	cd $(BUILD_DIR) && $(MAKE)

release:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
	cd $(BUILD_DIR) && $(MAKE)

clean:
	rm -rf $(BUILD_DIR)

# Allow setting CONFIG=Debug or CONFIG=Release when running 'make run'
CONFIG ?= Debug

run:
	$(BUILD_DIR)/bin/$(CONFIG)/$(EXE_NAME)