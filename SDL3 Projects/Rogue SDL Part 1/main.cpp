/*
    Rogue SDL
    The final project from Learning C++ by Building Games, built over
    Chapters 30 to 36

    A dungeon crawler in the tradition of Rogue, drawn entirely in
    characters. You're the @, and nothing happens until you move.

    New in this project: SDL3_ttf, SDL's add-on library for drawing text.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Common.h"
#include "Game.h"

// The whole game. The Game, and everything it owns, is destroyed when this
// returns, before main destroys the renderer
bool runGame(SDL_Renderer* renderer)
{
    Game game(renderer);
    if (!game.isLoaded())
        return false;

    game.run();
    return true;
}

int main(int argc, char* argv[])
{
    // Start SDL and SDL3_ttf, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    if (!TTF_Init())
    {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Rogue SDL", WINDOW_W, WINDOW_H, 0);
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Show each frame in step with the monitor's refresh
    SDL_SetRenderVSync(renderer, 1);

    // Play until the player quits, then clean up, in the reverse order
    // we created things
    bool played = runGame(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return played ? 0 : 1;
}
