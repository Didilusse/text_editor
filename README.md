# SFML Text Editor (*Name TBD*)

A simple text editor built with **C++** and **SFML**.  
I made this project to learn more about text rendering, cursor movement, and input handling using SFML.

## Features
- Text input (typing, backspace, enter)
- Movable cursor
- Keyboard navigation (Left, Right, Up, Down)
- Mouse click to move the cursor
- Vertical navigation that keeps the cursor aligned using a preferred X position
- Ability to save and load files

## Controls
- **Type** to insert text
- **Backspace** to delete
- **Enter** for a new line
- **Arrow keys** to move the cursor
- **Left mouse click** to move the cursor to a position in the text
- **Control + O** to open a file
- **Control + S** to save the file
- **Control + =** to increase font size
- **Control + -** to decrease font size

## How It Works 

This program uses a **gap buffer** to make text editing efficient.

### Gap Buffer
All text lives in a single buffer with a gap at the cursor position.
- Typing inserts characters directly into the gap
- Deleting text expands the gap
- Moving the cursor shifts the gap instead of copying text

This keeps common edits fast, even as the file grows.

### Cursor & Rendering
- Cursor movement is tracked logically, separate from rendering
- Vertical movement preserves a preferred X position across lines
- Each frame, the gap buffer is converted to a string and rendered with SFML
- The cursor is drawn based on glyph positions in the rendered text

### Input
- Keyboard input edits the gap buffer
- Mouse clicks map screen positions to text indices
- Loading a file replaces the buffer and resets cursor state

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