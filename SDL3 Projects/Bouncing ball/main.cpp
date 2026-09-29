/*
    Bouncing Ball
    The Chapter 3 project from Learning C++ by Building Games

    A ball, drawn as a square, starts in the middle of the window,
    drifts at a steady speed, and bounces off all four walls.
    Escape, or the window's X, quits.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int   WINDOW_W    = 800;                  // window width in pixels
const int   WINDOW_H    = 600;                  // window height in pixels
const float BALL_RADIUS = 20.0f;                // center to edge, in pixels
const float BALL_SIZE   = BALL_RADIUS * 2.0f;   // the ball's width and height
const float START_VEL_X = 300.0f;               // pixels per second, + is right
const float START_VEL_Y = 250.0f;               // pixels per second, + is down

const SDL_Color BACKGROUND = { 20, 24, 40, 255 };    // deep blue-black
const SDL_Color BALL_COLOR = { 255, 70, 70, 255 };   // bright red

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Bouncing Ball",
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

    // The ball: where its center is, and how fast it's moving
    float ballX = WINDOW_W / 2.0f;
    float ballY = WINDOW_H / 2.0f;
    float ballVelX = START_VEL_X;
    float ballVelY = START_VEL_Y;

    // The time at the last frame, in milliseconds
    Uint64 lastTime = SDL_GetTicks();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        // Events: quit on the window's X, or on Escape
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

        // Move the ball: velocity times the time that has passed
        ballX += ballVelX * delta;
        ballY += ballVelY * delta;

        // Bounce off the left and right walls
        if (ballX - BALL_RADIUS < 0.0f)
        {
            ballX = BALL_RADIUS;                // snap back inside...
            ballVelX = -ballVelX;               // ...then reverse
        }
        if (ballX + BALL_RADIUS > WINDOW_W)
        {
            ballX = WINDOW_W - BALL_RADIUS;
            ballVelX = -ballVelX;
        }

        // Bounce off the top and bottom walls
        if (ballY - BALL_RADIUS < 0.0f)
        {
            ballY = BALL_RADIUS;
            ballVelY = -ballVelY;
        }
        if (ballY + BALL_RADIUS > WINDOW_H)
        {
            ballY = WINDOW_H - BALL_RADIUS;
            ballVelY = -ballVelY;
        }

        // Draw the frame
        SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g,
                               BACKGROUND.b, BACKGROUND.a);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, BALL_COLOR.r, BALL_COLOR.g,
                               BALL_COLOR.b, BALL_COLOR.a);
        SDL_FRect ballRect = {
            ballX - BALL_RADIUS,   // left edge
            ballY - BALL_RADIUS,   // top edge
            BALL_SIZE,             // width
            BALL_SIZE              // height
        };
        SDL_RenderFillRect(renderer, &ballRect);

        SDL_RenderPresent(renderer);
    }

    // Clean up, in the reverse order we created things
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
