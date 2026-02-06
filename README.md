# SFML Text Editor (*Name TBD*)

A simple text editor built with **C++** and **SFML**.  
I made this project to learn more about text rendering, cursor movement, and input handling using SFML.

## Features
### Core Editing
- Text input (typing, backspace)
- Movable cursor with blinking animation
- Keyboard navigation (Left, Right, Up, Down with auto-repeat)
- Mouse click to position cursor anywhere in text
- Click and drag text selection with visual highlighting
- Vertical navigation that keeps the cursor aligned using a preferred X position
- Automatic word wrapping that adjusts to window size
### File Operations
- Save files with native file dialog (`Ctrl+S` / `Cmd+S`)
- Load files with native file dialog (`Ctrl+O` / `Cmd+O`)
- Automatic `.txt` extension on save
### UI & Interaction
- Resizable window with responsive UI elements
- Scrollbar with multiple interaction modes:
    - Click and drag the scrollbar thumb
    - Click anywhere on the track to jump to that position
    - Mouse wheel scrolling support
- Auto-scroll to keep cursor visible
- Dynamic font size adjustment (`Ctrl+Plus` / `Ctrl+Minus`)
- Font size display (anchored to top-right corner)
- Clean header bar with Save and Load buttons

### Visual Polish
- Blinking cursor (500ms interval)
- Semi-transparent blue selection highlighting
- Smooth scrolling animations
- Proportional scrollbar thumb (size reflects document length)
- Responsive layout that adapts to window resizing

## Controls
### Keyboard Shortcuts
- **Type** to insert text
- **Backspace** to delete character before cursor
- **Arrow keys** to move the cursor (hold for auto-repeat)
- **Ctrl/Cmd + O** to open a file
- **Ctrl/Cmd + S** to save the file
- **Ctrl/Cmd + =** (Plus) to increase font size
- **Ctrl/Cmd + -** (Minus) to decrease font size (minimum 6pt)

### Mouse Controls
- **Left click** to position cursor in text
- **Click and drag** to select text
- **Mouse wheel** to scroll through document
- **Click scrollbar thumb** and drag to scroll
- **Click scrollbar track** to jump to that position
- **Save/Load buttons** in top-left corner

## How It Works 

This program uses a **gap buffer** for efficient text editing and a **modular architecture** for maintainability.

### Gap Buffer
All text lives in a single buffer with a gap at the cursor position.
- Typing inserts characters directly into the gap
- Deleting text expands the gap
- Moving the cursor shifts the gap instead of copying text

This keeps common edits fast, even as the file grows.

### Modular Architecture
The codebase is organized into focused, reusable components:

- **GapBuffer** (`src/GapBuffer.h/cpp`): Efficient text storage and manipulation
- **UI** (`src/UI.h/cpp`): Button creation and cursor management
- **Scrollbar** (`src/Scrollbar.h/cpp`): Complete scrollbar with mouse interaction
- **TextRenderer** (`src/TextRenderer.h/cpp`): Text wrapping, cursor movement, and selection rendering
- **FileOperations** (`src/FileOperations.h/cpp`): Save and load dialogs
- **InputHandler** (`src/InputHandler.h/cpp`): Mouse click processing and coordinate mapping

This separation makes the code easier to:
- Read and understand (each file has one clear purpose)
- Maintain and debug (changes are localized)
- Test (components can be tested independently)
- Extend (new features have clear homes)

### Cursor & Rendering
- Cursor movement is tracked logically, separate from rendering
- Vertical movement preserves a preferred X position across lines
- Each frame, the gap buffer is converted to a string and rendered with SFML
- The cursor is drawn based on glyph positions in the rendered text

### Input
- Keyboard input edits the gap buffer
- Mouse clicks map screen positions to text indices
- Loading a file replaces the buffer and resets cursor state

## Project Structure
```
text-editor/
├── main_refactored.cpp          # Main application entry point (~330 lines)
├── CMakeLists.txt               # Build configuration
├── fonts/
│   └── Roboto.ttf              # Default font
├── src/
│   ├── GapBuffer.h/cpp         # Text buffer implementation
│   ├── UI.h/cpp                # UI components
│   ├── Scrollbar.h/cpp         # Scrollbar system
│   ├── TextRenderer.h/cpp      # Text display and wrapping
│   ├── FileOperations.h/cpp    # File I/O
│   └── InputHandler.h/cpp      # Mouse and keyboard input
└── libs/
    └── nativefiledialog/       # Native file dialogs
```

## Requirements
- C++17 or newer
- SFML 3
- A font file (currently uses `fonts/Roboto.ttf`)

## Build & Run
Make sure SFML is installed and linked correctly.

If using CMake:
```bash
mkdir build
cd build
cmake ..
make
./text_editor
```

## Future Features
- Ability to upload and change fonts
- Different themes
- More GUI elements

## Learning Outcomes

This project demonstrates:
- **Data structures**: Gap buffer for efficient text editing
- **Event-driven programming**: SFML event loop and input handling
- **Software architecture**: Modular design with separation of concerns
- **Graphics programming**: Text rendering, scrolling, and UI elements
- **File I/O**: Native dialogs and file operations
- **State management**: Cursor, selection, and scroll positions
- **Coordinate systems**: Screen space, world space, and text positions
- **Performance optimization**: Efficient rendering and input processing
