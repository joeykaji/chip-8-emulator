# CHIP-8 Emulator

A CHIP-8 emulator written in C++ with SDL2 for display, input, and timing.

## Dependencies

- C++17 or later
- SDL2

### Installing SDL2

**macOS**
```bash
brew install sdl2
```

**Ubuntu/Debian**
```bash
sudo apt install libsdl2-dev
```

## Building

```bash
g++ -std=c++17 -o chip8 src/main.cpp src/cpu/cpu.cpp src/platform/platform.cpp $(sdl2-config --cflags --libs)
```

## Usage

```bash
./chip8 <rom> [cycleRate]
```

- `rom` — path to a `.ch8` ROM file
- `cycleRate` — instructions per second (default: 700, adjust if a game runs too fast or slow)

**Example**
```bash
./chip8 roms/snake.ch8 700
```

## Keymap

The CHIP-8 keypad is mapped to the keyboard as follows:

```
CHIP-8 Keypad       Keyboard
-----------         --------
1 2 3 C             1 2 3 4
4 5 6 D      →      Q W E R
7 8 9 E             A S D F
A 0 B F             Z X C V
```

Press `ESC` to quit.

## Project Structure

```
.
├── src/
│   ├── main.cpp
│   ├── cpu/
│   │   ├── cpu.hpp
│   │   └── cpu.cpp
│   └── platform/
│       ├── platform.hpp
│       └── platform.cpp
└── README.md
```

## Architecture

### CPU (`src/cpu/`)
Emulates the CHIP-8 hardware:
- 16 x 8-bit general purpose registers (`V0`–`VF`)
- 4096 bytes of RAM
- 64×32 pixel monochrome display buffer
- 16-level call stack
- Delay and sound timers running at 60Hz
- 16-key keypad input

### Platform (`src/platform/`)
Handles everything outside the CPU using SDL2:
- **Display** — scales the 64×32 pixel buffer to the window
- **Input** — maps keyboard events to the CHIP-8 keypad
- **Timers** — decrements delay and sound timers at 60Hz independently of the CPU cycle rate

## Implemented Opcodes

| Opcode | Description |
|--------|-------------|
| `00E0` | Clear display |
| `00EE` | Return from subroutine |
| `1NNN` | Jump to address |
| `2NNN` | Call subroutine |
| `3XNN` | Skip if VX == NN |
| `4XNN` | Skip if VX != NN |
| `5XY0` | Skip if VX == VY |
| `6XNN` | Set VX = NN |
| `7XNN` | Set VX += NN |
| `8XY0` | Set VX = VY |
| `8XY1` | Set VX = VX OR VY |
| `8XY2` | Set VX = VX AND VY |
| `8XY3` | Set VX = VX XOR VY |
| `8XY4` | Set VX += VY, VF = carry |
| `8XY5` | Set VX -= VY, VF = borrow |
| `8XY6` | Set VX = VY >> 1, VF = LSB |
| `8XY7` | Set VX = VY - VX, VF = borrow |
| `8XYE` | Set VX = VY << 1, VF = MSB |
| `9XY0` | Skip if VX != VY |
| `ANNN` | Set I = NNN |
| `BNNN` | Jump to NNN + V0 |
| `CXNN` | Set VX = random AND NN |
| `DXYN` | Draw N-byte sprite at (VX, VY), VF = collision |
| `EX9E` | Skip if key VX pressed |
| `EXA1` | Skip if key VX not pressed |
| `FX07` | Set VX = delay timer |
| `FX0A` | Wait for key press, store in VX |
| `FX15` | Set delay timer = VX |
| `FX18` | Set sound timer = VX |
| `FX1E` | Set I += VX |
| `FX29` | Set I = font sprite address for VX |
| `FX33` | Store BCD of VX in memory[I..I+2] |
| `FX55` | Store V0–VX in memory starting at I |
| `FX65` | Read memory starting at I into V0–VX |

## Cycle Rate Reference

| Game | Recommended Rate |
|------|-----------------|
| Pong | 60 |
| Space Invaders | 400 |
| Snake | 700 |
| Tetris | 600 |

## Resources

- [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [CHIP-8 Test Suite (Corax+)](https://github.com/Timendus/chip8-test-suite)
