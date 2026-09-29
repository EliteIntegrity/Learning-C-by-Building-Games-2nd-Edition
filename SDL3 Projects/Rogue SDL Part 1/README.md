# Rogue SDL, Part 1 — Chapter 30

The companion project for Chapter 30 of *Learning C++ by Building Games*,
Final Project — Rogue SDL, Part 1: A Grid of Characters. It's the first of
seven stages of the book's final project, a dungeon crawler drawn entirely in
characters, and every piece of it that the chapter shows matches these files
exactly.

- `main.cpp` starts SDL and SDL3_ttf, makes the window and the renderer, and
  hands over to `runGame`, which owns the `Game`, so that everything the game
  owns is gone before the renderer is destroyed.
- `Common.h` holds what every file shares: `Point`, the size of the grid (80
  cells across and 45 down, each 16 pixels square), and the palette.
- `GlyphCache` turns the font into a texture for each of the 95 printable
  ASCII characters, once, in white, and draws any of them in any color,
  tinted with `SDL_SetTextureColorMod`, in the middle of its cell.
- `Map` is the dungeon's grid of tiles, kept in one `std::vector`. It draws
  the floor, and only the walls that touch it.
- `Entity` is anything with a position and a character, and `Player` is the
  `@`, whose `tryMove` stops at walls.
- `Game` owns the glyph cache, the map, and the player, and runs a loop that
  sleeps in `SDL_WaitEvent` until something happens, and draws the window
  again only when something has changed. For now, it builds a test room with
  two pillars in it.

The chapter reaches this project in six checkpoints, starting from an empty
project, and the versions of `main.cpp` in between aren't kept as projects of
their own. Each of the next six chapters has a folder of its own, from
`Rogue SDL Part 2` to `Rogue SDL Part 7`.

## Controls

- **Arrow keys**, or **W**, **A**, **S**, and **D** — move
- **Esc** — quit

## To build

Open `Rogue SDL Part 1.slnx` and press F5, choosing **Trust and Continue** if
Visual Studio asks. It has the chapter's settings (C++20, the SDL3 and
SDL3_ttf include and library folders, `SDL3.lib` and `SDL3_ttf.lib`), the
twelve source files, `SDL3.dll` and `SDL3_ttf.dll` beside them, and the
`assets` folder, which holds the font, `RobotoMono-Light.ttf`, one of the
free fonts from Google Fonts.
