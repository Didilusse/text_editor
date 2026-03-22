# Text Editor Makefile
# Requires SFML 3 and macOS (Cocoa framework)

CXX = clang++
CC = clang
OBJC = clang

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
CFLAGS = -Wall -Wextra -O2
OBJCFLAGS = -Wall -Wextra -O2

# Include paths
INCLUDES = -I libs/nativefiledialog/src/include

# SFML 3 flags (using pkg-config)
SFML_CFLAGS = $(shell pkg-config --cflags sfml-graphics sfml-window sfml-system 2>/dev/null)
SFML_LIBS = $(shell pkg-config --libs sfml-graphics sfml-window sfml-system 2>/dev/null)

# Fallback if pkg-config doesn't work (adjust paths as needed)
ifeq ($(SFML_LIBS),)
    SFML_CFLAGS = -I/usr/local/include -I/opt/homebrew/include
    SFML_LIBS = -L/usr/local/lib -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system
endif

# macOS frameworks
FRAMEWORKS = -framework Cocoa -framework UniformTypeIdentifiers

# Source files
CXX_SOURCES = main.cpp \
              src/GapBuffer.cpp \
              src/UI.cpp \
              src/Scrollbar.cpp \
              src/TextRenderer.cpp \
              src/FileOperations.cpp \
              src/InputHandler.cpp \
              src/SearchDialog.cpp \
              src/StatusBar.cpp

C_SOURCES = libs/nativefiledialog/src/nfd_common.c

OBJC_SOURCES = libs/nativefiledialog/src/nfd_cocoa.m

# Object files
BUILD_DIR = build
CXX_OBJECTS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CXX_SOURCES))
C_OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
OBJC_OBJECTS = $(patsubst %.m,$(BUILD_DIR)/%.o,$(OBJC_SOURCES))
OBJECTS = $(CXX_OBJECTS) $(C_OBJECTS) $(OBJC_OBJECTS)

# Target
TARGET = text_editor

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(SFML_LIBS) $(FRAMEWORKS)

# Compile C++ sources
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SFML_CFLAGS) -c $< -o $@

# Compile C sources
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile Objective-C sources
$(BUILD_DIR)/%.o: %.m
	@mkdir -p $(dir $@)
	$(OBJC) $(OBJCFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Run the application
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0 -DDEBUG
debug: CFLAGS = -Wall -Wextra -g -O0 -DDEBUG
debug: OBJCFLAGS = -Wall -Wextra -g -O0 -DDEBUG
debug: clean all

# Release build with more optimizations
release: CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -DNDEBUG
release: CFLAGS = -Wall -Wextra -O3 -DNDEBUG
release: OBJCFLAGS = -Wall -Wextra -O3 -DNDEBUG
release: clean all

# Print help
help:
	@echo "Available targets:"
	@echo "  all      - Build the text editor (default)"
	@echo "  run      - Build and run the text editor"
	@echo "  clean    - Remove build files"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build with full optimizations"
	@echo "  help     - Show this help message"

.PHONY: all clean run debug release help
