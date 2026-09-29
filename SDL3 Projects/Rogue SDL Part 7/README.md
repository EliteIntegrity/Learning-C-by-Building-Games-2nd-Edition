# Rogue SDL, Part 7 — Chapter 36

The companion project for Chapter 36 of *Learning C++ by Building Games*,
Final Project — Rogue SDL, Part 7: Swords and Fireballs. It's the last of the
seven stages of the book's final project, and the finished game: every piece
of it that the chapter shows matches these files exactly.

- Weapons: a dagger, a sword, and a warhammer, which add 2, 5, and 10 to every
  hit. They turn up more often the deeper you go, and better ones too, and
  you wield one by using it from the inventory. The player holds a weapon in
  a `std::optional<ItemKind>`, empty for bare hands, and a weapon you stop
  wielding goes back into your pack. The HUD says what you're wielding.
- A scroll of fireball, on about a third of the levels. Using it from the
  inventory opens `TargetingState` in the inventory's place: the arrows, or
  W, A, S, and D, move a cursor, a see-through orange glow shows every cell
  that would burn, Enter casts it, and Esc puts the scroll away. Moving the
  cursor takes no turn.
- The blast burns every cell within two steps of where it lands (a diamond of
  13), apart from walls and anything out of sight, and does 8 damage to every
  monster in it. It never hurts the player.
- The save file is version 3: the `PLAYER` line ends with the weapon, as its
  kind's number, or -1 for bare hands. A version 2 save from Part 6 is turned
  away.

The other twenty-four files are unchanged from Chapter 35. The chapter reaches
this project from Part 6's in two stages (the weapons, with version 3 of the
save file; then the fireball, with its aiming state), each one a game you can
run, and the version in between isn't kept as a project of its own.

## Controls

- **Arrow keys**, or **W**, **A**, **S**, and **D** — move, and attack
- **I** — open the inventory; then a letter uses that item, and **Esc** closes it
- While aiming a fireball: the arrows, or **W**, **A**, **S**, and **D**, move
  the cursor, **Enter** casts it, and **Esc** puts the scroll away
- **.** (the period) — take the stairs down, when you're standing on them
- **F5** — save the game (in the game's window; in Visual Studio, F5 starts it)
- **F9** — load the saved game
- **R** — play again, after dying
- **Esc** — quit

## To build

Open `Rogue SDL Part 7.slnx` and press F5, choosing **Trust and Continue** if
Visual Studio asks. It has Chapter 30's settings (C++20, the SDL3 and
SDL3_ttf include and library folders, `SDL3.lib` and `SDL3_ttf.lib`), the
thirty-eight source files, `SDL3.dll` and `SDL3_ttf.dll` beside them, and the
`assets` folder, which holds the font, `RobotoMono-Light.ttf`, and the seven
sounds.
