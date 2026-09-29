/*
    Cascade
    The Chapter 7 project from Learning C++ by Building Games

    A single for loop draws a diagonal cascade of colored squares
    from the top-left of the window, going around a palette of nine
    colors, and stops as soon as the next square wouldn't fit. The
    colors shift one place every tenth of a second, so the rainbow
    flows along the diagonal. Escape, or the window's X, quits.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int   WINDOW_W  = 800;                   // window width in pixels
const int   WINDOW_H  = 600;                   // window height in pixels
const float SQUARE_SZ = 30.0f;                 // each square's width and height
const float PADDING   = 2.0f;                  // the gap between squares
const float STEP      = SQUARE_SZ + PADDING;   // from one corner to the next

const SDL_Color BACKGROUND = { 30, 30, 30, 255 };   // dark gray

// The palette the cascade goes around, one color per square
const SDL_Color COLORS[] = {
    { 220,  50,  50, 255 },   // red
    { 230, 140,  30, 255 },   // orange
    { 220, 210,  40, 255 },   // yellow
    {  50, 180,  50, 255 },   // green
    {  40, 140, 220, 255 },   // blue
    { 100,  60, 200, 255 },   // indigo
    { 170,  60, 200, 255 },   // violet
    {  50, 200, 180, 255 },   // teal
    { 200,  80, 130, 255 }    // pink
};
const int COLOR_COUNT = SDL_arraysize(COLORS);   // how many colors: 9

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Cascade", WINDOW_W, WINDOW_H, 0);
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

    // The time the program started, in milliseconds
    Uint64 startTime = SDL_GetTicks();

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

        // Draw the frame
        SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g,
                               BACKGROUND.b, BACKGROUND.a);
        SDL_RenderClear(renderer);

        // Shift the colors along one place every 100 milliseconds
        int offset = static_cast<int>((SDL_GetTicks() - startTime) / 100);

        // The cascade: squares marching diagonally from the top-left
        float x = PADDING;
        float y = PADDING;

        for (int i = 0; ; i++)
        {
            // Stop as soon as the next square would cross either edge
            if (x + SQUARE_SZ > WINDOW_W || y + SQUARE_SZ > WINDOW_H)
                break;

            // Pick this square's color, going around the palette
            SDL_Color color = COLORS[(i + offset) % COLOR_COUNT];
            SDL_SetRenderDrawColor(renderer, color.r, color.g,
                                   color.b, color.a);

            SDL_FRect square = { x, y, SQUARE_SZ, SQUARE_SZ };
            SDL_RenderFillRect(renderer, &square);

            // Move along the diagonal to the next square's spot
            x += STEP;
            y += STEP;
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up, in the reverse order we created things
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
