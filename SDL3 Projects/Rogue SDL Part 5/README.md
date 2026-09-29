# Rogue SDL, Part 5 — Chapter 34

The companion project for Chapter 34 of *Learning C++ by Building Games*,
Final Project — Rogue SDL, Part 5: Sound and Light. It's the fifth of seven
stages of the book's final project, and every piece of it that the chapter
shows matches these files exactly.

- `Sound` loads a WAV file with `SDL_LoadWAV`, and opens an audio stream of its
  own with `SDL_OpenAudioDeviceStream`, which starts paused until
  `SDL_ResumeAudioStreamDevice`. `play` clears the stream and puts the whole
  sound in, so a sound played again starts over, and different sounds, each
  with its own stream, play at once, mixed by SDL. It owns its samples and
  its stream, frees them in its destructor, and can't be copied.
- `Sounds` holds the game's six sounds, loaded when the game is made: `hit`,
  `kill`, `hurt`, `pickup`, `drink`, and `stairs`, all 16-bit mono WAV files
  at 44,100 samples a second, in `assets`.
- `FlashEffects` keeps short flashes of color over cells: red where something
  is hit, and green where the player heals. Each one starts at its color's
  own alpha and fades to nothing over `FLASH_MS`, 200 milliseconds, by the
  clock (`SDL_GetTicks`), drawn with `SDL_BLENDMODE_BLEND`.
- `Game::run` still sleeps in `SDL_WaitEvent` while nothing is happening, but
  while any flash is fading, it waits in `SDL_WaitEventTimeout` for at most
  `FRAME_MS`, 16 milliseconds, and draws the window again each time it wakes,
  until the last flash is gone.
- `main` starts SDL with `SDL_INIT_VIDEO | SDL_INIT_AUDIO`.

The other twenty-two files are unchanged from Chapter 33. The chapter reaches
this project from Part 4's in three stages (the sounds; the flashes, drawn but
not yet fading, since the loop still sleeps; and a loop that wakes while they
fade), each one a game you can run, and the versions in between aren't kept as
projects of their own.

If the game is silent, look in the console window: a sound that couldn't be
loaded, or couldn't reach a playback device, says why there, and the game
carries on without it.

## Controls

- **Arrow keys**, or **W**, **A**, **S**, and **D** — move, and attack
- **H** — drink a potion
- **.** (the period) — take the stairs down, when you're standing on them
- **F5** — save the game (in the game's window; in Visual Studio, F5 starts it)
- **F9** — load the saved game
- **R** — play again, after dying
- **Esc** — quit

## To build

Open `Rogue SDL Part 5.slnx` and press F5, choosing **Trust and Continue** if
Visual Studio asks. It has Chapter 30's settings (C++20, the SDL3 and
SDL3_ttf include and library folders, `SDL3.lib` and `SDL3_ttf.lib`), the
thirty source files, `SDL3.dll` and `SDL3_ttf.dll` beside them, and the
`assets` folder, which holds the font, `RobotoMono-Light.ttf`, and the six
sounds.
