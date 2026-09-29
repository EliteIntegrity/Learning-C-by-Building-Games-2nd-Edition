# Vibe Asteroids — Chapter 27

The finished game from Chapter 27 of *Learning C++ by Building Games*, built
by describing it to an AI in nine rounds, from a plan made before the first
prompt, and reading, running, and editing every answer. Everything is in one
file, `main.cpp`, and every piece of it that the chapter shows matches this
file exactly.

This is **my** finished version. Yours will look different: an AI's answers
are never quite the same twice, and your plan, your edits, and your pushback
will differ from mine.

- One ship, a `std::vector` of bullets, and a `std::vector` of asteroids, in
  one `Game` struct, with small functions, one job each, called from one
  `update`: `updateShip`, `updateBullets`, `updateAsteroids`,
  `shootAsteroids`, and `crashShip`, then the next wave, and the title bar.
- `Vec2` is Chapter 24's `Vector2`, with `+` and `*`. The ship is drawn by
  turning its corners from its own frame into the window, and asteroids are
  ten-cornered outlines whose lumps turn with them.
- The fixes from the chapter's messy middle are all here: the `break` after a
  hit, drag that can't reverse the ship, delta time capped at `MAX_DELTA`,
  waves that never arrive beside the ship, and a lost ship that waits for a
  clear middle before it comes back.
- The AI's own version, before those edits and fixes, is kept in
  `_pre-revision backup/SDL3 Projects/Vibe Asteroids`.

## Controls

- **Left and right arrows**, or **A** and **D** — turn
- **Up arrow**, or **W** — thrust
- **Space** — fire
- **R** (once the game is over) — play again
- **Esc** — quit

The score and the lives show in the window's title bar.

## To build

Open `Vibe Asteroids.slnx` and press F5. Like Chapter 1's project, it uses
the `SDL3` folder beside it in `SDL3 Projects`, and `SDL3.dll` is already in
the project folder, so nothing needs setting up. It's plain SDL 3, with no
add-on libraries.
