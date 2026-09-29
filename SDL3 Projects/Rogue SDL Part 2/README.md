# Rogue SDL, Part 2 — Chapter 31

The companion project for Chapter 31 of *Learning C++ by Building Games*,
Final Project — Rogue SDL, Part 2: Dungeons in the Dark. It's the second of
seven stages of the book's final project, and every piece of it that the
chapter shows matches these files exactly.

- `MapGenerator` builds every level by binary space partitioning: `split`
  cuts the map in two, across its longer side, and calls itself on the two
  pieces, until a piece is too small to cut, or six cuts deep, and gets a
  room. Then `generate` joins each room to the one made before it with an
  L-shaped corridor, starts the player in a room chosen at random, and puts
  the stairs down in the room farthest away, by Manhattan distance.
- `FOV` works out what the player can see: `compute` casts 68 rays, to every
  cell around the edge of a square 17 cells across, and each ray marks the
  cells it reaches as visible and explored, until it leaves the circle of
  sight, radius 8, or meets a wall.
- `Map` has stairs, and every `Tile` remembers whether it's been explored,
  and whether it's visible. It draws each explored tile from a table of
  looks, brightly if it's in sight, and dimly if it's only remembered.
- `HUD` is the five rows below the map: the depth, the keys, and the last
  three messages, kept in a `std::deque`. `GlyphCache::drawText` draws its
  text packed tight, on the bottom of each row.
- `Game` builds a new level at the start and at every staircase, looks
  around after every step, and tells the player what's happening.

`main.cpp`, `Entity`, and `Player` are unchanged from Chapter 30. The chapter
reaches this project from Part 1's in five stages (dungeons, stairs, darkness,
sight, and the HUD), each one a game you can run, and the versions in between
aren't kept as projects of their own.

## Controls

- **Arrow keys**, or **W**, **A**, **S**, and **D** — move
- **.** (the period) — take the stairs down, when you're standing on them
- **Esc** — quit

## To build

Open `Rogue SDL Part 2.slnx` and press F5, choosing **Trust and Continue** if
Visual Studio asks. It has Chapter 30's settings (C++20, the SDL3 and
SDL3_ttf include and library folders, `SDL3.lib` and `SDL3_ttf.lib`), the
eighteen source files, `SDL3.dll` and `SDL3_ttf.dll` beside them, and the
`assets` folder, which holds the font, `RobotoMono-Light.ttf`.
