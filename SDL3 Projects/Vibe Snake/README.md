# Vibe Snake — Chapter 26

The finished game from Chapter 26 of *Learning C++ by Building Games*, built
by describing it to an AI in six rounds, and reading, running, and editing
every answer. Everything is in one file, `main.cpp`, and every piece of it
that the chapter shows matches this file exactly.

This is **my** finished version. Yours will be different: an AI's answers
are never quite the same twice, and your edits and your pushback will differ
from mine.

- The AI's code is here after my edits: `static_cast` instead of C-style
  casts, full names instead of one-letter ones, the colors as named
  constants, SDL's `SDL_rand` instead of C's `rand`, and the score in the
  title bar built with a `std::string`, in `showScore`.
- `stepSnake` holds the game's rules: the queued turn from `pendingDir`, the
  walls, the snake's own body (skipping the tail, which is leaving), and
  eating, growing, and speeding up.
- The AI's own first version, before those edits, is kept in
  `_pre-revision backup/SDL3 Projects/Vibe Snake`.

## Controls

- **Arrow keys**, or **W**, **A**, **S**, and **D** — steer
- **R** (once the game is over) — play again
- **Esc** — quit

The score shows in the window's title bar.

## To build

Open `Vibe Snake.slnx` and press F5. Like Chapter 1's project, it uses the
`SDL3` folder beside it in `SDL3 Projects`, and `SDL3.dll` is already in the
project folder, so nothing needs setting up. It's plain SDL 3, with no
add-on libraries.
