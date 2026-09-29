# Animated Character (Inheritance) — Chapter 21

In-book project for Chapter 21 of *Learning C++ by Building Games*. Every
file here matches the chapter's complete files exactly.

Chapter 19's runner, with company: three rivals, tinted red, green, and blue,
and see-through, who run round and round the window on their own, each at a
pace of her own, and jump at random. What the runners share has been lifted
out of Chapter 19's `Player` into a base class:

- `Entity` is anyone drawn from the runner's sprite sheet. She runs at her
  own pace, turns to face the way she's going, jumps, and is drawn in her
  own tint. Only `x_`, her position across the window, is protected.
- `Player` derives from `Entity`, and adds only what's hers: her `update`
  calls `Entity::update`, and then keeps her inside the window.
- `Ghost` derives from `Entity` too. She sets off as soon as she's made, and
  her `update` makes a random jump now and then, calls `Entity::update`,
  and then wraps her around from one side of the window to the other.
- `Texture::draw` takes a tint, an `SDL_Color`, with white as its default,
  so every other drawing is unchanged.

`Animator` and `HUD` are unchanged from Chapter 19. There are no virtual
functions yet, so `runGame` keeps the player in a `Player` variable, and the
ghosts in a `std::vector<Ghost>`, and each is updated through her own type.
Chapter 22 brings `virtual`, and Chapter 23 puts them all in one list.

## Controls

- **Left and right arrows**, or **A** and **D** — run
- **Space**, **W**, or the **up arrow** — jump
- **Esc** — quit

## To build

An Empty Project with Chapter 11's settings (C++20, the SDL and SDL_image
include and library folders, `SDL3.lib` and `SDL3_image.lib`), the thirteen
source files, `SDL3.dll` and `SDL3_image.dll` beside them, and the `assets`
folder, which holds six of the Runner's pictures.

The earlier version of this project, with an `Enemy` class derived from
`Entity`, is kept in `_pre-revision backup/SDL3 Projects/Animated Character
Inheritance`.
