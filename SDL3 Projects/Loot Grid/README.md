# Loot Grid — Chapter 16

In-book project for Chapter 16 of *Learning C++ by Building Games*. The
source in `main.cpp` matches the chapter's complete listing exactly.

A small grid game built on the collections from Chapter 15. The world is a
`std::map` from grid cells (`std::pair<int, int>`, called `Cell`) to the loot
in them, storing only the cells that hold something; the inventory is a
`std::map` that counts each kind of loot; and a `const std::unordered_map`
holds each kind's name and color. Plain structs and functions, no classes.

## Controls

- **W, A, S, D** — move one cell
- **Tab** — list the inventory, and how much loot is left, in the console
- **R** — a new world
- **Esc** — quit

## To build

An Empty Project with the Chapter 1 settings (C++20, the SDL include and
library folders, `SDL3.lib`), `main.cpp`, and `SDL3.dll` beside it. Core
SDL 3 only, with no add-on libraries.

The original version of this project is kept in
`_pre-revision backup/SDL3 Projects/Loot Grid`.
