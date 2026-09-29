/*
    Controllable Square
    The Chapter 1 project from Learning C++ by Building Games

    Opens an 800 x 600 window with an orange square in the middle.
    W, A, S, and D move the square. Escape, or the window's X, quits.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int   WINDOW_W  = 800;      // the window's width in pixels
const int   WINDOW_H  = 600;      // the window's height in pixels
const float SQUARE_SZ = 80.0f;    // the length of each side of the square
const float SPEED     = 300.0f;   // the square's speed, in pixels per second

int main(int argc, char* argv[])
{
    // Start SDL's video system, which also gives us the keyboard
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Controllable Square",   // the text in the title bar
        WINDOW_W, WINDOW_H,      // the size in pixels
        0                        // no special options
    );
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

    // The square's top-left corner, starting in the middle of the window
    float x = (WINDOW_W - SQUARE_SZ) / 2.0f;
    float y = (WINDOW_H - SQUARE_SZ) / 2.0f;

    // The time at the last frame, in milliseconds
    Uint64 lastTime = SDL_GetTicks();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        // Events: everything that has happened since the last frame
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.key == SDLK_ESCAPE)
            {
                running = false;
            }
        }

        // Delta time: how many seconds the last frame took
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;

        // Movement: check which keys are held down right now
        const bool* keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_W])
        {
            y -= SPEED * delta;
        }
        if (keys[SDL_SCANCODE_S])
        {
            y += SPEED * delta;
        }
        if (keys[SDL_SCANCODE_A])
        {
            x -= SPEED * delta;
        }
        if (keys[SDL_SCANCODE_D])
        {
            x += SPEED * delta;
        }

        // Keep the whole square inside the window
        x = SDL_clamp(x, 0.0f, WINDOW_W - SQUARE_SZ);
        y = SDL_clamp(y, 0.0f, WINDOW_H - SQUARE_SZ);

        // Draw the frame
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);     // dark gray
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255);    // orange
        SDL_FRect square = { x, y, SQUARE_SZ, SQUARE_SZ };
        SDL_RenderFillRect(renderer, &square);

        SDL_RenderPresent(renderer);
    }

    // Clean up, in the reverse order we created things
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
