# Animated Character (Interfaces) — Chapter 23

In-book project for Chapter 23 of *Learning C++ by Building Games*, the
capstone of Act 3. Every file here matches the chapter's complete files
exactly.

Chapter 21's runner and her three ghosts, rebuilt on Chapter 22's virtual
functions and interfaces, with a new runner, a shadow, who follows the player
wherever she goes, and jumps when she jumps:

- `IUpdatable` and `IDrawable` are interfaces: one pure virtual function
  each, `update` and `draw`, and a virtual destructor.
- `Entity` keeps both promises, so every runner does. `Player`, `Ghost`, and
  the new `Shadow` derive from it, each overriding `update`. `Entity` also
  answers two questions, `getX` and `isJumping`, for the shadow.
- `HUD` keeps both promises too, with no base class in common with the
  runners. `Scenery` owns the landscape's four pictures, and keeps only
  `IDrawable`'s promise, because it never changes.
- `runGame` owns every runner through one
  `std::vector<std::unique_ptr<Entity>>`, keeps an observer of the player for
  the keyboard, and builds two views, a `std::vector<IUpdatable*>` and a
  `std::vector<IDrawable*>`. One loop updates everything, and another draws
  everything.

`Texture`, `Animator`, `Player.cpp`, `Ghost.cpp`, and `HUD.cpp` are unchanged
from Chapter 21.

## Controls

- **Left and right arrows**, or **A** and **D** — run
- **Space**, **W**, or the **up arrow** — jump
- **Esc** — quit

## To build

An Empty Project with Chapter 11's settings (C++20, the SDL and SDL_image
include and library folders, `SDL3.lib` and `SDL3_image.lib`), the nineteen
source files, `SDL3.dll` and `SDL3_image.dll` beside them, and the `assets`
folder, which holds six of the Runner's pictures.

The earlier version of this project, with colored squares, an `Enemy` class,
and a frame-time HUD, is kept in `_pre-revision backup/SDL3 Projects/Animated
Character Interfaces`.
