# Rogue SDL, Part 6 — Chapter 35

The companion project for Chapter 35 of *Learning C++ by Building Games*,
Final Project — Rogue SDL, Part 6: States and Scrolls. It's the sixth of seven
stages of the book's final project, and every piece of it that the chapter
shows matches these files exactly.

- `GameState` is the abstract base class of every state the game can be in.
  Its `handleKey` is pure virtual, and its `draw` draws nothing unless a state
  overrides it. A state never changes the stack itself: it returns a
  `StateChange`, which asks for the state to be closed, for another to be
  opened on top, for both, or for neither.
- `PlayingState` has the keys you play with, which `Game::handleKey` used to
  handle itself, and asks the game to act through its public functions.
- `InventoryState` dims the whole window with a see-through black, and lists
  the pack in a box over it. A letter uses the item in its slot (a is slot 0,
  since SDL's keycodes for the letters are in order), and Esc closes it.
  Looking takes no turn; using something does.
- `Game` keeps a stack of states, a `std::vector` of `std::unique_ptr`s, hands
  every key to the state on top, makes the change the state asks for once it
  has returned, closing first and opening second, and draws every state after
  the HUD, from the bottom up.
- The player carries a pack of up to twelve items, one for each letter from a
  to l, in place of the potion count. Anything but gold goes into it, if
  there's room.
- A scroll of magic mapping turns up on about half of the levels. Reading it
  marks every tile explored, so the whole level is drawn, in remembered
  colors, though nothing out of sight becomes visible.
- The save file is version 2: the player's line has no potion count, and a
  `CARRY` line follows for each item in the pack. A version 1 save from
  Part 5 is turned away.

The other seventeen files are unchanged from Chapter 34. The chapter reaches
this project from Part 5's in three stages (the states, with the game playing
exactly as before; the pack, with its inventory and version 2 of the save
file; and the scroll), each one a game you can run, and the versions in
between aren't kept as projects of their own.

If reading a scroll is silent, look in the console window: a sound that
couldn't be loaded says why there, and the game carries on without it.

## Controls

- **Arrow keys**, or **W**, **A**, **S**, and **D** — move, and attack
- **I** — open the inventory; then a letter uses that item, and **Esc** closes it
- **.** (the period) — take the stairs down, when you're standing on them
- **F5** — save the game (in the game's window; in Visual Studio, F5 starts it)
- **F9** — load the saved game
- **R** — play again, after dying
- **Esc** — quit

## To build

Open `Rogue SDL Part 6.slnx` and press F5, choosing **Trust and Continue** if
Visual Studio asks. It has Chapter 30's settings (C++20, the SDL3 and
SDL3_ttf include and library folders, `SDL3.lib` and `SDL3_ttf.lib`), the
thirty-six source files, `SDL3.dll` and `SDL3_ttf.dll` beside them, and the
`assets` folder, which holds the font, `RobotoMono-Light.ttf`, and seven
sounds, the scroll's among them.
