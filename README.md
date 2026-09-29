# Learning C++ by Building Games, Second Edition

The code for every project in *Learning C++ by Building Games: Learn C++ from Scratch & Build Games using SDL 3 and AI*, second edition, by John Horton. You can read the book free online at [GameCodeSchool.com](https://gamecodeschool.com), where you'll find the paperback and the ebook, too.

Looking for the first edition's code? It's still at [Learning-C-by-Building-Games](https://github.com/EliteIntegrity/Learning-C-by-Building-Games).

## What's here

Everything is in the `SDL3 Projects` folder. Each project has a folder of its own, named in a note at the start of its chapter, and SDL 3, SDL_image, and SDL_ttf sit beside them, set up for Visual Studio.

| Chapter | Project | Folder |
|---|---|---|
| 1 | Controllable Square | `ControllableSquare` |
| 3 | Bouncing Ball | `Bouncing ball` |
| 5 | Square Invader | `Square Invader` |
| 7 | Cascade | `Draw squares` |
| 9 | Act 1 Capstone Shooter | `Capstone Shooter` |
| 11 | Whack-a-Mole | `Whack-a-Mole` |
| 12 | Click Particle | `Click Particle` |
| 14 | Particle Fountain | `Particle Fountain` |
| 16 | Loot Grid | `Loot Grid` |
| 17 | The Runner | `Runner` |
| 19 | Animated Character (Classes) | `Animated Character Classes` |
| 21 | Animated Character (Inheritance) | `Animated Character Inheritance` |
| 23 | Animated Character (Interfaces) | `Animated Character Interfaces` |
| 24 | Files and the system | `File IO and System` |
| 26 | Vibe Snake | `Vibe Snake` |
| 27 | Vibe Asteroids | `Vibe Asteroids` |
| 28 | The Component Pattern | `Animated Character Components` |
| 29 | The Data Locality Pattern | `Particle Timings` |
| 30 to 36 | Rogue SDL, Parts 1 to 7 | `Rogue SDL Part 1` to `Rogue SDL Part 7` |

## On Windows

Open a project's `.slnx` file (or its `.sln` file, for the first two projects) in Visual Studio 2026, choose **Trust and Continue** if Visual Studio asks, and press **F5**. The projects already point at the SDL folders beside them, and each project folder has the DLLs it needs to run. Chapter 1 explains the setup, so you can do the same for projects of your own.

## On macOS and Linux

`SDL3 Projects/CMakeLists.txt` builds every project with CMake. It uses SDL, SDL_image, and SDL_ttf if they're installed and new enough, and downloads them the first time if they aren't. Appendix B of the book, For Mac and Linux Users, walks through it with VS Code, and it's free to read at GameCodeSchool.com.

## Licenses

SDL, SDL_image, and SDL_ttf are by their own authors, and come with their own licenses, in their folders.

The font in the Rogue SDL projects, `RobotoMono-Light.ttf`, is Roboto Mono, copyright 2015 The Roboto Mono Project Authors, and is used under the Apache License 2.0. A copy of the license is beside the font in each `assets` folder, as `RobotoMono-LICENSE.txt`.
