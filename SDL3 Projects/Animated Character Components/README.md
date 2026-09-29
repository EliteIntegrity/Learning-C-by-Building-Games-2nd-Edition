# Animated Character (Components) — Chapter 28

The companion project for Chapter 28 of *Learning C++ by Building Games*, The
Component Pattern. It's Chapter 23's runner and her rivals, rebuilt from
components, and every piece of it that the chapter shows matches these files
exactly.

- Every runner is one class, `GameObject`: a position (`x` and `y`, shared by
  its components) and three components, called in order every frame.
- `InputComponent` is an interface, with three kinds of input: `KeyboardInput`
  (the arrow keys, or A and D, to run, and a jump for each press of Space, W,
  or the up arrow), `GhostInput` (one way forever, with random jumps), and
  `ShadowInput` (follows a leader, and jumps when she jumps).
- `PhysicsComponent` runs, jumps, falls, and lands her, and keeps to her rule
  at the window's edges: `Edges::Stop`, `Edges::Wrap`, or `Edges::Free`.
- `GraphicsComponent` animates her and draws her, tinted, from the runner's
  sprite sheet.
- `main.cpp` has a recipe for each kind of runner (`makePlayer`, `makeGhost`,
  and `makeShadow`), and a demo mode: press Tab, and the player's input is
  swapped for a `ShadowInput` that follows the red ghost.

`Texture`, `Animator`, `HUD`, `Scenery`, `IUpdatable`, and `IDrawable` are
unchanged from Chapter 23. Its `Entity`, `Player`, `Ghost`, and `Shadow` are
gone: their jobs are the components' now. The chapter reaches this project in
three steps from Chapter 23's, and shows or describes every change; the two
steps in between aren't kept as projects of their own.

## Controls

- **Left and right arrows**, or **A** and **D** — run
- **Space**, **W**, or the **up arrow** — jump
- **Tab** — hand the player to the computer, and back
- **Esc** — quit

## To build

Open `Animated Character Components.slnx` and press F5, choosing **Trust and
Continue** if Visual Studio asks. It has Chapter 11's settings (C++20, the SDL
and SDL_image include and library folders, `SDL3.lib` and `SDL3_image.lib`),
the twenty-four source files, `SDL3.dll` and `SDL3_image.dll` beside them, and
the `assets` folder, which holds six of the Runner's pictures.
