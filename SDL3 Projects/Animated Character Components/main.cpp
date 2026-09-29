/*
    Animated Character (Components)
    The Chapter 28 project from Learning C++ by Building Games

    Chapter 23's runners, rebuilt from components. Every runner is the
    same class, a GameObject, made of an input, a physics, and a graphics,
    and only the recipe changes. Hold the left or right arrow, or A or D,
    to run, and press Space, W, or the Up arrow to jump. Press Tab to hand
    the player to the computer, and Tab again to take her back. The number
    in the top-left corner is the frame rate. Escape, or the window's X,
    quits.

    New in this project: the Component pattern. A runner's input, physics,
    and graphics are three small classes of their own, and her input can
    be swapped for another while the game runs.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <memory>   // std::unique_ptr, for the runners and their inputs
#include <vector>   // std::vector, for the runners and the views

#include "IUpdatable.h"
#include "IDrawable.h"
#include "Texture.h"
#include "Scenery.h"
#include "GameObject.h"
#include "KeyboardInput.h"
#include "GhostInput.h"
#include "ShadowInput.h"
#include "HUD.h"

const int   WINDOW_W  = 960;      // window width in pixels
const int   WINDOW_H  = 540;      // window height in pixels
const float MAX_DELTA = 0.1f;     // the longest frame we'll allow, in seconds
const float GROUND_Y  = 452.0f;   // the line the runners stand on

// The ghosts' colors, each one see-through, and the shadow's
const SDL_Color RED_GHOST   = { 255, 110, 110, 150 };
const SDL_Color GREEN_GHOST = { 110, 220, 130, 150 };
const SDL_Color BLUE_GHOST  = { 120, 160, 255, 150 };
const SDL_Color SHADOW_TINT = { 40, 40, 70, 150 };   // dark, and see-through

// The window's title, which says who's running the player
const char* PLAY_TITLE = "Animated Character - press Tab for a demo";
const char* DEMO_TITLE = "Animated Character - demo mode, Tab to take over";

// The player: the keyboard drives her, and she stops at the window's sides
std::unique_ptr<GameObject> makePlayer(const Texture& sheet,
                                       float windowWidth)
{
    return std::make_unique<GameObject>(
        windowWidth / 2.0f, GROUND_Y,
        std::make_unique<KeyboardInput>(),
        PhysicsComponent(GROUND_Y, windowWidth, Edges::Stop),
        GraphicsComponent(sheet));
}

// A ghost: she drives herself, round and round the window, at her own pace,
// and in her own color
std::unique_ptr<GameObject> makeGhost(const Texture& sheet, float x,
                                      int direction, float pace,
                                      SDL_Color tint, float windowWidth)
{
    return std::make_unique<GameObject>(
        x, GROUND_Y,
        std::make_unique<GhostInput>(direction),
        PhysicsComponent(GROUND_Y, windowWidth, Edges::Wrap, pace),
        GraphicsComponent(sheet, tint));
}

// A shadow: she follows her leader wherever she goes, even out of the window
std::unique_ptr<GameObject> makeShadow(const Texture& sheet,
                                       const GameObject& leader, float x,
                                       float windowWidth)
{
    return std::make_unique<GameObject>(
        x, GROUND_Y,
        std::make_unique<ShadowInput>(leader),
        PhysicsComponent(GROUND_Y, windowWidth, Edges::Free),
        GraphicsComponent(sheet, SHADOW_TINT));
}

// Hand the player to the computer, which follows the leader, or give her
// back to the keyboard. The title bar says which
void setDemo(GameObject& player, const GameObject& leader, bool demo,
             SDL_Window* window)
{
    if (demo)
        player.setInput(std::make_unique<ShadowInput>(leader));
    else
        player.setInput(std::make_unique<KeyboardInput>());

    SDL_SetWindowTitle(window, demo ? DEMO_TITLE : PLAY_TITLE);
}

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

    // The recipes take the window's width as a float, so convert it once
    const float windowWidth = static_cast<float>(WINDOW_W);

    // Every runner, owned by this one list, in the order they're drawn: the
    // ghosts at the back, then the shadow, and the player in front
    std::vector<std::unique_ptr<GameObject>> runners;
    runners.push_back(makeGhost(runnerSheet, 150.0f, 1, 0.8f, RED_GHOST,
                                windowWidth));
    runners.push_back(makeGhost(runnerSheet, 700.0f, -1, 1.25f, GREEN_GHOST,
                                windowWidth));
    runners.push_back(makeGhost(runnerSheet, 420.0f, -1, 0.6f, BLUE_GHOST,
                                windowWidth));

    // Two observers, which use runners the list owns: the red ghost, for
    // the demo to follow, and the player, for her shadow and the demo
    GameObject* redGhost = runners[0].get();
    std::unique_ptr<GameObject> newPlayer = makePlayer(runnerSheet,
                                                       windowWidth);
    GameObject* player = newPlayer.get();

    runners.push_back(makeShadow(runnerSheet, *player, 300.0f,
                                 windowWidth));
    runners.push_back(std::move(newPlayer));

    // Two views of everything in the game, which own nothing: what changes
    // from frame to frame, and what's drawn, from the back to the front
    std::vector<IUpdatable*> updatables;
    std::vector<IDrawable*> drawables = { &scenery };
    for (std::unique_ptr<GameObject>& runner : runners)
    {
        updatables.push_back(runner.get());
        drawables.push_back(runner.get());
    }
    updatables.push_back(&hud);
    drawables.push_back(&hud);

    // The window the renderer draws in, for its title, and who's running
    // the player: the keyboard, until Tab is pressed
    SDL_Window* window = SDL_GetRenderWindow(renderer);
    bool demo = false;

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
                case SDLK_TAB:
                    demo = !demo;
                    setDemo(*player, *redGhost, demo, window);
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }
            }
        }

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

    SDL_Window* window = SDL_CreateWindow(PLAY_TITLE, WINDOW_W, WINDOW_H,
                                          0);
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
