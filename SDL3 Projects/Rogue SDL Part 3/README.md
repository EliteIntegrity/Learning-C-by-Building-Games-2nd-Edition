# Rogue SDL, Part 3 — Chapter 32

The companion project for Chapter 32 of *Learning C++ by Building Games*,
Final Project — Rogue SDL, Part 3: Monsters and Treasure. It's the third of
seven stages of the book's final project, and every piece of it that the
chapter shows matches these files exactly.

- `Enemy` and `Item` are entities, like the `Player`. Every kind of monster
  (rat, goblin, orc) and every kind of treasure (potion, gold) is a row in a
  `constexpr` table of stats, in the same order as its `enum class`, and
  `statsOf` looks a kind's row up.
- `MapGenerator` fills every level as it builds it: 3 monsters, plus the
  depth, plus 0 to 2 more, with goblins from depth 2 and orcs from depth 4,
  and 1 to 3 potions and 2 to 5 piles of gold. `takeFreeSpot` keeps two
  things from ever sharing a cell, with `std::find` and `Point`'s `==`.
- `Game` keeps the monsters and the treasure by value, in two
  `std::vector`s. A step into a monster attacks it (the bump rule), and a
  step onto treasure picks it up. After every turn of the player's,
  `endTurn` clears away the dead with `std::erase_if`, and every monster
  that has noticed the player attacks if it's beside them, or steps closer.
  A bump into a wall takes no turn.
- `Player` has health, an attack, gold, and potions, which heal 8. Health
  never goes below 0, or above 20.
- `HUD` shows the health, gold, potions, and depth, and a red line when the
  player dies. R starts a new game, and Esc quits.

`main.cpp`, `Entity`, `GlyphCache`, `Map`, and `FOV` are unchanged from
Chapter 31. The chapter reaches this project from Part 2's in four stages
(monsters and treasure that only appear, fighting and collecting, monsters
that fight back, and death), each one a game you can run, and the versions
in between aren't kept as projects of their own.

## Controls

- **Arrow keys**, or **W**, **A**, **S**, and **D** — move, and attack
- **H** — drink a potion
- **.** (the period) — take the stairs down, when you're standing on them
- **R** — play again, after dying
- **Esc** — quit

## To build

Open `Rogue SDL Part 3.slnx` and press F5, choosing **Trust and Continue** if
Visual Studio asks. It has Chapter 30's settings (C++20, the SDL3 and
SDL3_ttf include and library folders, `SDL3.lib` and `SDL3_ttf.lib`), the
twenty-two source files, `SDL3.dll` and `SDL3_ttf.dll` beside them, and the
`assets` folder, which holds the font, `RobotoMono-Light.ttf`.
