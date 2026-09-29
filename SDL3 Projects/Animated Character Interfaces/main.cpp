/*
    Animated Character (Interfaces)
    The Chapter 23 project from Learning C++ by Building Games

    The runner from Chapter 21, with her three ghostly rivals, and now a
    shadow of her own, who follows her wherever she goes. Hold the left
    or right arrow, or A or D, to run, and press Space, W, or the Up
    arrow to jump. The number in the top-left corner is the frame rate.
    Escape, or the window's X, quits.

    New in this project: virtual functions and interfaces. Every runner
    is owned by one list, and everything in the game is updated by one
    loop and drawn by another, through IUpdatable and IDrawable.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <memory>   // std::unique_ptr, for the runners
#include <vector>   // std::vector, for the runners and the views

#include "IUpdatable.h"
#include "IDrawable.h"
#include "Texture.h"
#include "Scenery.h"
#include "Player.h"
#include "Ghost.h"
#include "Shadow.h"
#include "HUD.h"

const int   WINDOW_W  = 960;      // window width in pixels
const int   WINDOW_H  = 540;      // window height in pixels
const float MAX_DELTA = 0.1f;     // the longest frame we'll allow, in seconds
const float GROUND_Y  = 452.0f;   // the line the runners stand on

// The ghosts' colors, each one see-through
const SDL_Color RED_GHOST   = { 255, 110, 110, 150 };
const SDL_Color GREEN_GHOST = { 110, 220, 130, 150 };
const SDL_Color BLUE_GHOST  = { 120, 160, 255, 150 };

// The whole game, from the first frame to the last. Everything made in
// here is destroyed when it returns, before main destroys the renderer
bool runGame(SDL_Renderer* renderer)
{
    Scenery scenery(renderer);
    Texture runnerSheet(renderer, "assets/runner.png");
    Texture digits(renderer, "assets/digits.png");

    // If a picture didn't load, its Texture has already said which
    if (!scenery.isLoaded() || !runnerSheet.isLoaded() || !digits.isLoaded())
        return false;

    HUD hud(digits);

    // The runners take the window's width as a float. std::make_unique
    // passes its arguments on exactly as they are, so convert it here, once
    const float windowWidth = static_cast<float>(WINDOW_W);

    // Every runner, owned by this one list, in the order they're drawn: the
    // ghosts at the back, then the shadow, and the player in front
    std::vector<std::unique_ptr<Entity>> runners;
    runners.push_back(std::make_unique<Ghost>(
        runnerSheet, 150.0f, GROUND_Y, 1, 0.8f, RED_GHOST, windowWidth));
    runners.push_back(std::make_unique<Ghost>(
        runnerSheet, 700.0f, GROUND_Y, -1, 1.25f, GREEN_GHOST, windowWidth));
    runners.push_back(std::make_unique<Ghost>(
        runnerSheet, 420.0f, GROUND_Y, -1, 0.6f, BLUE_GHOST, windowWidth));

    // The keyboard needs to reach the player, so keep a pointer to her,
    // which uses her but doesn't own her. The list owns her
    std::unique_ptr<Player> newPlayer =
        std::make_unique<Player>(runnerSheet, GROUND_Y, windowWidth);
    Player* player = newPlayer.get();

    runners.push_back(std::make_unique<Shadow>(runnerSheet, *player, 300.0f,
                                               GROUND_Y));
    runners.push_back(std::move(newPlayer));

    // Two views of everything in the game, which own nothing: what changes
    // from frame to frame, and what's drawn, from the back to the front
    std::vector<IUpdatable*> updatables;
    std::vector<IDrawable*> drawables = { &scenery };
    for (std::unique_ptr<Entity>& runner : runners)
    {
        updatables.push_back(runner.get());
        drawables.push_back(runner.get());
    }
    updatables.push_back(&hud);
    drawables.push_back(&hud);

    // The time at the last frame, in milliseconds
    Uint64 lastTime = SDL_GetTicks();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        // Handle every event that's waiting
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            // A key going down, but not the repeats from holding it
            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
            {
                switch (event.key.key)
                {
                case SDLK_SPACE:
                case SDLK_W:
                case SDLK_UP:
                    player->jump();
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }
            }
        }

        // Run left or right while an arrow key, or A or D, is held
        const bool* keys = SDL_GetKeyboardState(nullptr);
        int direction = 0;
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
            direction -= 1;
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
            direction += 1;
        player->run(direction);

        // Delta time, never more than MAX_DELTA
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;
        if (delta > MAX_DELTA)
            delta = MAX_DELTA;

        // Move everything on by one frame
        for (IUpdatable* thing : updatables)
            thing->update(delta);

        // Draw the frame, from the back to the front
        SDL_RenderClear(renderer);
        for (const IDrawable* thing : drawables)
            thing->draw(renderer);

        SDL_RenderPresent(renderer);
    }

    return true;
}

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Animated Character", WINDOW_W,
                                          WINDOW_H, 0);
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
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
    SDL_Quit();

    return played ? 0 : 1;
}
