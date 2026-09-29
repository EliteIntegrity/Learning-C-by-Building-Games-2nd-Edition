# Rogue SDL, Part 4 — Chapter 33

The companion project for Chapter 33 of *Learning C++ by Building Games*,
Final Project — Rogue SDL, Part 4: Hunters and Save Files. It's the fourth of
seven stages of the book's final project, and every piece of it that the
chapter shows matches these files exactly.

- `AStar::findPath` finds the shortest path from one cell to another, around
  the walls, with A*. Its candidates wait in a `std::priority_queue` with
  `std::greater`, smallest guess first, where a guess is the steps so far
  plus the Manhattan distance still to go. Two `std::unordered_map`s keyed by
  `Point` hold the fewest steps to each cell reached, and the cell it was
  reached from, and the trail back from the goal, reversed, is the path.
- `Common.h` specializes `std::hash<Point>`, so that a `Point` can be a key in
  an unordered map. A point's hash is its tile's index in the map's vector.
- A monster that sees the player hunts them, and remembers where it saw them
  last. Every turn, a hunting monster attacks if it's next to the player, and
  otherwise takes the first step of A*'s path to its last sighting, unless
  another monster stands there. When it reaches the last sighting and can't
  see the player, it gives up, and waits.
- F5 saves the game to `rogue_save.txt`, in the working directory, which is
  the project folder when the game runs from Visual Studio, and F9 loads it.
  `SaveLoad` writes the depth, the player, the monsters, the treasure, and
  the map, one thing to a line, with each tile as a letter: W, F, or S, in
  capitals where the player has explored. Loading reads everything into new
  variables, checks every kind and every cell, and changes the game only if
  the whole file was good. Neither key takes a turn.
- `static_assert` checks each stats table against its count of kinds,
  `MONSTER_KINDS` and `ITEM_KINDS`, while the program builds.

`main.cpp`, `Entity`, `GlyphCache`, `Map`, `FOV`, `MapGenerator`, and `HUD.h`
are unchanged from Chapter 32. The chapter reaches this project from Part 3's
in two stages (the hunters, and then saving and loading), each one a game you
can run, and the version in between isn't kept as a project of its own.

## Controls

- **Arrow keys**, or **W**, **A**, **S**, and **D** — move, and attack
- **H** — drink a potion
- **.** (the period) — take the stairs down, when you're standing on them
- **F5** — save the game (in the game's window; in Visual Studio, F5 starts it)
- **F9** — load the saved game
- **R** — play again, after dying
- **Esc** — quit

## To build

Open `Rogue SDL Part 4.slnx` and press F5, choosing **Trust and Continue** if
Visual Studio asks. It has Chapter 30's settings (C++20, the SDL3 and
SDL3_ttf include and library folders, `SDL3.lib` and `SDL3_ttf.lib`), the
twenty-six source files, `SDL3.dll` and `SDL3_ttf.dll` beside them, and the
`assets` folder, which holds the font, `RobotoMono-Light.ttf`.
