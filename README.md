# SFML Text Editor (*Name TBD*)

A simple text editor built with **C++** and **SFML**.  
I made this project to learn more about text rendering, cursor movement, and input handling using SFML.

## Features
- Text input (typing, backspace, enter)
- Movable cursor
- Keyboard navigation (Left, Right, Up, Down)
- Mouse click to move the cursor
- Vertical navigation that keeps the cursor aligned using a preferred X position

## Controls
- **Type** to insert text
- **Backspace** to delete
- **Enter** for a new line
- **Arrow keys** to move the cursor
- **Left mouse click** to move the cursor to a position in the text

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
- Ability to save and load files (WIP)
- Ability to upload and change fonts
- Different themes
- More GUI elements