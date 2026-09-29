/*
    Animated Character (Inheritance)
    The Chapter 21 project from Learning C++ by Building Games

    The runner from Chapter 19, now with three ghostly rivals who run
    round and round the window on their own. Hold the left or right
    arrow, or A or D, to run, and press Space, W, or the Up arrow to
    jump. The number in the top-left corner is the frame rate. Escape,
    or the window's X, quits.

    New in this project: inheritance. An Entity is anyone drawn from the
    runner's sprite sheet, and the Player and the Ghost are both kinds of
    Entity, each adding only what makes her different.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <vector>   // std::vector, for the ghosts

#include "Texture.h"
#include "Player.h"
#include "Ghost.h"
#include "HUD.h"

const int   WINDOW_W  = 960;      // window width in pixels
const int   WINDOW_H  = 540;      // window height in pixels
const float MAX_DELTA = 0.1f;     // the longest frame we'll allow, in seconds
const float GROUND_Y  = 452.0f;   // the line the runner stands on

// The whole game, from the first frame to the last. Everything made in
// here is destroyed when it returns, before main destroys the renderer
bool runGame(SDL_Renderer* renderer)
{
    Texture sky(renderer, "assets/sky.png");
    Texture farHills(renderer, "assets/hills_far.png");
    Texture nearHills(renderer, "assets/hills_near.png");
    Texture ground(renderer, "assets/ground.png");
    Texture runnerSheet(renderer, "assets/runner.png");
    Texture digits(renderer, "assets/digits.png");

    // If a picture didn't load, its Texture has already said which
    if (!sky.isLoaded() || !farHills.isLoaded() || !nearHills.isLoaded() ||
        !ground.isLoaded() || !runnerSheet.isLoaded() || !digits.isLoaded())
    {
        return false;
    }

    Player player(runnerSheet, GROUND_Y, WINDOW_W);
    HUD hud(digits);

    // Three rivals, each with a pace and a color of her own, see-through
    std::vector<Ghost> ghosts;
    ghosts.push_back(Ghost(runnerSheet, 150.0f, GROUND_Y, 1, 0.8f,
                           { 255, 110, 110, 150 }, WINDOW_W));
    ghosts.push_back(Ghost(runnerSheet, 700.0f, GROUND_Y, -1, 1.25f,
                           { 110, 220, 130, 150 }, WINDOW_W));
    ghosts.push_back(Ghost(runnerSheet, 420.0f, GROUND_Y, -1, 0.6f,
                           { 120, 160, 255, 150 }, WINDOW_W));

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
                    player.jump();
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
        player.run(direction);

        // Delta time, never more than MAX_DELTA
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;
        if (delta > MAX_DELTA)
            delta = MAX_DELTA;

        // Move everything on by one frame
        player.update(delta);
        for (Ghost& ghost : ghosts)
            ghost.update(delta);
        hud.update(delta);

        // Draw the frame, from the back to the front
        SDL_RenderClear(renderer);
        sky.draw(renderer, nullptr, nullptr);
        farHills.draw(renderer, nullptr, nullptr);
        nearHills.draw(renderer, nullptr, nullptr);
        ground.draw(renderer, nullptr, nullptr);
        for (const Ghost& ghost : ghosts)
            ghost.draw(renderer);
        player.draw(renderer);
        hud.draw(renderer);

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
