# Windows-compatible Makefile for CMD or PowerShell in VSCode

# Define paths
ROOT_DIR := $(CURDIR)
BUILD_DIR := $(ROOT_DIR)\build
BIN_DIR_DEBUG := $(BUILD_DIR)\bin\Debug
BIN_DIR_RELEASE := $(BUILD_DIR)\bin\Release
EXE_NAME := minesweeper.exe

# Full paths to UCRT64 tools
CMAKE	:= C:\msys64\ucrt64\bin\cmake.exe
MAKE 	:= C:\msys64\ucrt64\bin\make.exe
CC 		:= C:\msys64\ucrt64\bin\gcc.exe
CXX 	:= C:\msys64\ucrt64\bin\g++.exe

# Default target: runs 'make debug' when you just type 'make'
.DEFAULT_GOAL := debug

# Required tools
REQUIRED_TOOLS := $(CMAKE) $(MAKE) $(CC)

.PHONY: require setup clean debug release run

require:
	@echo Checking required tools...
	@if not exist "$(CMAKE)" (echo ERROR: cmake not found & exit /b 1)
	@if not exist "$(CC)" (echo ERROR: gcc not found & exit /b 1)
	@if not exist "$(CXX)" (echo ERROR: g++ not found & exit /b 1)
	@if not exist "$(MAKE)" (echo ERROR: make not found & exit /b 1)
	@echo All required tools found.

setup: require
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@cd "$(BUILD_DIR)" && "$(CMAKE)" -G "Unix Makefiles" -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_BUILD_TYPE=Debug ..

debug:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@cd "$(BUILD_DIR)" && "$(CMAKE)" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
	@cd "$(BUILD_DIR)" && "$(MAKE)"

release:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@cd "$(BUILD_DIR)" && "$(CMAKE)" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
	@cd "$(BUILD_DIR)" && "$(MAKE)"

clean:
	@if exist "$(BUILD_DIR)" rd /s /q "$(BUILD_DIR)"

# Default run target uses CONFIG=Debug or CONFIG=Release
CONFIG ?= Debug

run:
	@if exist "$(BUILD_DIR)\bin\$(CONFIG)\$(EXE_NAME)" ( \
		"$(BUILD_DIR)\bin\$(CONFIG)\$(EXE_NAME)" \
	) else ( \
		echo ERROR: $(CONFIG) executable not found. Build it first with 'make $(CONFIG)' & exit /b 1 \
	)