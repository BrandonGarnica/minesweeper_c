ROOT_DIR      := $(CURDIR)
# Select backend: sdl (default) or embedded
BACKEND       ?= sdl

BUILD_DIR     := $(ROOT_DIR)/build/$(BACKEND)
BIN_DIR_DEBUG := $(BUILD_DIR)/bin/Debug
BIN_DIR_REL   := $(BUILD_DIR)/bin/Release
EXE_NAME      := minesweeper.exe

CMAKE := cmake
MAKE  := make
CC    := gcc

# Map BACKEND -> CMake flags
ifeq ($(BACKEND),sdl)
  CMAKE_BACKEND_FLAGS := -DUSE_SDL=ON
else ifeq ($(BACKEND),embedded)
  CMAKE_BACKEND_FLAGS := -DUSE_SDL=OFF
else
  $(error BACKEND must be 'sdl' or 'embedded')
endif

.PHONY: all setup debug release clean run

all: debug

debug:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) -G "Unix Makefiles" \
		-DCMAKE_BUILD_TYPE=Debug $(CMAKE_BACKEND_FLAGS) $(ROOT_DIR)
	cd $(BUILD_DIR) && $(MAKE)

release:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) -G "Unix Makefiles" \
		-DCMAKE_BUILD_TYPE=Release $(CMAKE_BACKEND_FLAGS) $(ROOT_DIR)
	cd $(BUILD_DIR) && $(MAKE)

setup:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) -G "Unix Makefiles" \
		-DCMAKE_C_COMPILER=$(CC) $(CMAKE_BACKEND_FLAGS) $(ROOT_DIR) \
		$(CMAKE_BACKEND_FLAGS) $(ROOT_DIR)

clean:
	rm -rf $(ROOT_DIR)/build

# Allow CONFIG=Debug or CONFIG=Release when running 'make run'
CONFIG ?= Debug

run:
	$(BUILD_DIR)/bin/$(CONFIG)/$(EXE_NAME)