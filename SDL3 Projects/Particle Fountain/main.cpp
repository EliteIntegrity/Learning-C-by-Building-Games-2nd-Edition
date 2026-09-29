/*
    Particle Fountain
    The Chapter 14 project from Learning C++ by Building Games

    Chapter 12's Click Particle, with room for as many particles as you
    like. Each left-click sprays a burst of particles up out of the
    mouse pointer, like a fountain. They fall, bounce, and fade away,
    and each one is deleted as it fades. Right-click to delete them
    all. The title bar counts them. Escape, or the window's X, quits.

    New in this project: a std::vector of pointers to Particles, which
    the program still deletes one by one.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cmath>    // std::cos and std::sin, for a random direction
#include <string>   // std::string and std::to_string, for the title bar
#include <vector>   // std::vector, to keep all the particles

const int       WINDOW_W   = 800;                   // window width in pixels
const int       WINDOW_H   = 600;                   // window height in pixels
const SDL_Color BACKGROUND = { 12, 12, 24, 255 };   // deep blue-black

// New particles
const float MIN_SIZE   = 8.0f;        // width and height, in pixels
const float MAX_SIZE   = 16.0f;
const float MIN_SPEED  = 350.0f;      // pixels per second
const float MAX_SPEED  = 650.0f;
const float TWO_PI     = 6.2831853f;  // a full turn, in radians
const int   MIN_BRIGHT = 100;         // no part of a color is darker

// The fountain
const float UP           = TWO_PI * 0.75f;  // three quarters of a turn
const float SPREAD       = 0.45f;           // radians either side of up
const float GRAVITY      = 600.0f;          // pixels per second, per second
const float FLOOR_BOUNCE = 0.6f;            // speed kept after hitting floor
const int   BURST        = 40;              // particles for each click
const float LIFETIME     = 3.0f;            // seconds until it fades away

// One particle: a colored square with a position and a velocity
struct Particle
{
    float x;           // the top-left corner, in pixels
    float y;
    float velX;        // the velocity, in pixels per second
    float velY;
    float size;        // the width and height, in pixels
    SDL_Color color;
    float life;        // seconds left before it fades away
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

    float angle = randomBetween(UP - SPREAD, UP + SPREAD);
    float speed = randomBetween(MIN_SPEED, MAX_SPEED);
    p->velX = std::cos(angle) * speed;
    p->velY = std::sin(angle) * speed;

    p->color.r = static_cast<Uint8>(MIN_BRIGHT + SDL_rand(256 - MIN_BRIGHT));
    p->color.g = static_cast<Uint8>(MIN_BRIGHT + SDL_rand(256 - MIN_BRIGHT));
    p->color.b = static_cast<Uint8>(MIN_BRIGHT + SDL_rand(256 - MIN_BRIGHT));
    p->color.a = 255;
    p->life = LIFETIME;

    return p;
}

// Draw a particle as a square, in its own color, fading as it ages
void drawParticle(SDL_Renderer* renderer, const Particle* p)
{
    Uint8 alpha = static_cast<Uint8>(p->color.a * (p->life / LIFETIME));
    SDL_SetRenderDrawColor(renderer, p->color.r, p->color.g, p->color.b,
                           alpha);
    SDL_FRect rect = { p->x, p->y, p->size, p->size };
    SDL_RenderFillRect(renderer, &rect);
}

// Move a particle on by one frame, bouncing it off the walls
void moveParticle(Particle* p, float delta)
{
    p->velY += GRAVITY * delta;
    p->life -= delta;

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
        p->velY = -p->velY * FLOOR_BOUNCE;
    }
}

// Delete every particle that has faded away, and take it out of the
// vector. The order doesn't matter, so each gap is filled from the end.
void removeFaded(std::vector<Particle*>& particles)
{
    size_t i = 0;
    while (i < particles.size())
    {
        if (particles[i]->life <= 0.0f)
        {
            delete particles[i];
            particles[i] = particles.back();
            particles.pop_back();
        }
        else
        {
            i++;
        }
    }
}

// Delete every particle, and empty the vector
void deleteAll(std::vector<Particle*>& particles)
{
    for (Particle* p : particles)
        delete p;
    particles.clear();
}

// Show how many particles there are in the title bar
void updateTitle(SDL_Window* window, size_t count)
{
    std::string title = "Particle Fountain    Particles: " +
                        std::to_string(count);
    SDL_SetWindowTitle(window, title.c_str());
}

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Particle Fountain",
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

    // Let see-through colors blend with whatever is behind them
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // All the particles, each one on the heap, with room for 1,000
    std::vector<Particle*> particles;
    particles.reserve(1000);
    updateTitle(window, particles.size());

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
                    for (int i = 0; i < BURST; i++)
                    {
                        particles.push_back(spawnParticle(event.button.x,
                                                          event.button.y));
                    }
                    updateTitle(window, particles.size());
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    deleteAll(particles);
                    updateTitle(window, particles.size());
                }
            }
        }

        // Delta time: how many seconds the last frame took
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;

        // Move every particle
        for (Particle* p : particles)
            moveParticle(p, delta);

        // Remove the particles that have faded, and update the count
        size_t countBefore = particles.size();
        removeFaded(particles);
        if (particles.size() != countBefore)
            updateTitle(window, particles.size());

        // Draw the frame
        SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g,
                               BACKGROUND.b, BACKGROUND.a);
        SDL_RenderClear(renderer);

        for (const Particle* p : particles)
            drawParticle(renderer, p);

        SDL_RenderPresent(renderer);
    }

    // Clean up, in the reverse order we created things
    deleteAll(particles);  // every particle that's still on the heap
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
