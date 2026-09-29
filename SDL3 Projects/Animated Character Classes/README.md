# Animated Character (Classes) — Chapter 19

In-book project for Chapter 19 of *Learning C++ by Building Games*. Every
file here matches the chapter's complete files exactly.

The runner from Chapter 17, rebuilt from classes, each in a `.h` file and a
`.cpp` file of its own:

- `Texture` owns one picture: its constructor loads it with SDL_image, and
  its destructor destroys it. Copying is deleted, so there's only ever one
  owner.
- `Animator` counts through an animation's frames at a steady rate, with
  Chapter 17's accumulator.
- `Player` is the runner. She has an `Animator` of her own, uses a sprite
  sheet that `runGame` owns, runs, turns, and jumps.
- `HUD` counts the frames drawn each second, and draws the count from a
  strip of digits.

`main.cpp` starts SDL, and `runGame` holds the game: every object it makes
is destroyed when it returns, before `main` destroys the renderer.

## Controls

- **Left and right arrows**, or **A** and **D** — run
- **Space**, **W**, or the **up arrow** — jump
- **Esc** — quit

## To build

An Empty Project with Chapter 11's settings (C++20, the SDL and SDL_image
include and library folders, `SDL3.lib` and `SDL3_image.lib`), the nine
source files, `SDL3.dll` and `SDL3_image.dll` beside them, and the `assets`
folder, which holds six of the Runner's pictures.

The earlier version of this project, a colored square with a rainbow
animator, is kept in `_pre-revision backup/SDL3 Projects/Animated Character
Classes`.
