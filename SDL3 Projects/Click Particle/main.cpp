/*
    Click Particle
    The Chapter 12 project from Learning C++ by Building Games

    Left-click anywhere, and a colored square is made on the heap and
    flies off, bouncing around the window. Left-click again, and the old
    square is deleted and a new one takes its place. Right-click to
    delete it and leave the window empty. Escape, or the window's X,
    quits.

    New in this project: a Particle made with new, reached through a
    raw pointer with the arrow operator, and deleted with delete.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cmath>   // std::cos and std::sin, for a random direction

const int       WINDOW_W   = 800;                   // window width in pixels
const int       WINDOW_H   = 600;                   // window height in pixels
const SDL_Color BACKGROUND = { 12, 12, 24, 255 };   // deep blue-black

// New particles
const float MIN_SIZE   = 16.0f;       // width and height, in pixels
const float MAX_SIZE   = 32.0f;
const float MIN_SPEED  = 180.0f;      // pixels per second
const float MAX_SPEED  = 360.0f;
const float TWO_PI     = 6.2831853f;  // a full turn, in radians
const int   MIN_BRIGHT = 100;         // no part of a color is darker

// One particle: a colored square with a position and a velocity
struct Particle
{
    float x;           // the top-left corner, in pixels
    float y;
    float velX;        // the velocity, in pixels per second
    float velY;
    float size;        // the width and height, in pixels
    SDL_Color color;
};

// A random number from low up to high
float randomBetween(float low, float high)
{
    return low + SDL_randf() * (high - low);
}

// Make a particle on the heap, centered on a point, and flying off in a
// random direction. It's the caller's job to delete it.
Particle* spawnParticle(float centerX, float centerY)
{
    Particle* p = new Particle{};

    p->size = randomBetween(MIN_SIZE, MAX_SIZE);
    p->x = centerX - p->size / 2.0f;
    p->y = centerY - p->size / 2.0f;

    float angle = randomBetween(0.0f, TWO_PI);
    float speed = randomBetween(MIN_SPEED, MAX_SPEED);
    p->velX = std::cos(angle) * speed;
    p->velY = std::sin(angle) * speed;

    p->color.r = static_cast<Uint8>(MIN_BRIGHT + SDL_rand(256 - MIN_BRIGHT));
    p->color.g = static_cast<Uint8>(MIN_BRIGHT + SDL_rand(256 - MIN_BRIGHT));
    p->color.b = static_cast<Uint8>(MIN_BRIGHT + SDL_rand(256 - MIN_BRIGHT));
    p->color.a = 255;

    return p;
}

// Draw a particle as a square, in its own color
void drawParticle(SDL_Renderer* renderer, const Particle* p)
{
    SDL_SetRenderDrawColor(renderer, p->color.r, p->color.g, p->color.b,
                           p->color.a);
    SDL_FRect rect = { p->x, p->y, p->size, p->size };
    SDL_RenderFillRect(renderer, &rect);
}

// Move a particle on by one frame, bouncing it off the walls
void moveParticle(Particle* p, float delta)
{
    p->x += p->velX * delta;
    p->y += p->velY * delta;

    // The left and right walls
    if (p->x < 0.0f)
    {
        p->x = 0.0f;
        p->velX = -p->velX;
    }
    else if (p->x + p->size > WINDOW_W)
    {
        p->x = WINDOW_W - p->size;
        p->velX = -p->velX;
    }

    // The top and bottom walls
    if (p->y < 0.0f)
    {
        p->y = 0.0f;
        p->velY = -p->velY;
    }
    else if (p->y + p->size > WINDOW_H)
    {
        p->y = WINDOW_H - p->size;
        p->velY = -p->velY;
    }
}

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Click Particle",
                                          WINDOW_W, WINDOW_H, 0);
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

    // The particle on the heap, or nullptr when there isn't one
    Particle* particle = nullptr;

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
            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.key == SDLK_ESCAPE)
            {
                running = false;
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    delete particle;   // the old one, if there is one
                    particle = spawnParticle(event.button.x, event.button.y);
                    SDL_Log("New particle at %p",
                            static_cast<void*>(particle));
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    delete particle;
                    particle = nullptr;
                    SDL_Log("No particle now");
                }
            }
        }

        // Delta time: how many seconds the last frame took
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;

        // Move the particle, if there is one
        if (particle != nullptr)
            moveParticle(particle, delta);

        // Draw the frame
        SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g,
                               BACKGROUND.b, BACKGROUND.a);
        SDL_RenderClear(renderer);

        if (particle != nullptr)
            drawParticle(renderer, particle);

        SDL_RenderPresent(renderer);
    }

    // Clean up, in the reverse order we created things
    delete particle;       // the particle, if there is one
    particle = nullptr;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
