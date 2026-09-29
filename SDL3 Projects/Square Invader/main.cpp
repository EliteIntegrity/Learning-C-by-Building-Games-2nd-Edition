/*
    Square Invader
    The Chapter 5 project from Learning C++ by Building Games

    A green player slides along the bottom of the window with A and D,
    or the arrow keys, and Space fires a yellow bullet. A red invader
    marches across the top, dropping a row at each wall. Shoot it to
    score, but let it reach your row and you lose a life. Lose all
    three, and the game is over. Escape, or the window's X, quits.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int   WINDOW_W       = 800;      // window width in pixels
const int   WINDOW_H       = 600;      // window height in pixels
const float PLAYER_SIZE    = 40.0f;    // the player's width and height
const float BULLET_SIZE    = 10.0f;    // the bullet's width and height
const float INVADER_SIZE   = 40.0f;    // the invader's width and height
const float EDGE_GAP       = 10.0f;    // space at the top and bottom
const float PLAYER_SPEED   = 300.0f;   // pixels per second
const float BULLET_SPEED   = 480.0f;   // pixels per second, straight up
const float INVADER_SPEED  = 120.0f;   // pixels per second, sideways
const int   STARTING_LIVES = 3;        // lives at the start of the game

const SDL_Color BACKGROUND    = { 20, 20, 30, 255 };    // near-black
const SDL_Color GAME_OVER_BG  = { 60, 0, 0, 255 };      // dark red
const SDL_Color PLAYER_COLOR  = { 0, 200, 0, 255 };     // green
const SDL_Color PLAYER_DIM    = { 0, 80, 0, 255 };      // dim green
const SDL_Color BULLET_COLOR  = { 255, 255, 0, 255 };   // yellow
const SDL_Color INVADER_COLOR = { 200, 0, 0, 255 };     // red

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Square Invader",
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

    // The player: its top-left corner, centered near the bottom
    float playerX = (WINDOW_W - PLAYER_SIZE) / 2.0f;
    float playerY = WINDOW_H - PLAYER_SIZE - EDGE_GAP;

    // The bullet: only one can be flying at a time
    float bulletX = 0.0f;
    float bulletY = 0.0f;
    bool bulletActive = false;

    // The invader: starts in the top-left corner, heading right
    float invaderX = 0.0f;
    float invaderY = EDGE_GAP;
    float invaderSpeedX = INVADER_SPEED;   // + is right, - is left

    // The score, the lives left, and whether the game is over
    int score = 0;
    int lives = STARTING_LIVES;
    bool gameOver = false;

    // The time at the last frame, in milliseconds
    Uint64 lastTime = SDL_GetTicks();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        // Events: quit on the window's X, or on Escape
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                {
                    running = false;
                }
                else if ((event.key.key == SDLK_SPACE) && !bulletActive &&
                         !gameOver)
                {
                    // Fire: start the bullet just above the player
                    bulletX = playerX + (PLAYER_SIZE - BULLET_SIZE) / 2.0f;
                    bulletY = playerY - BULLET_SIZE;
                    bulletActive = true;
                }
                break;
            }
        }

        // Delta time: how many seconds the last frame took
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;

        // The player: slide while A or D, or an arrow key, is held
        if (!gameOver)
        {
            const bool* keys = SDL_GetKeyboardState(nullptr);
            if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
                playerX -= PLAYER_SPEED * delta;
            if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
                playerX += PLAYER_SPEED * delta;

            // Keep the whole player inside the window
            playerX = SDL_clamp(playerX, 0.0f, WINDOW_W - PLAYER_SIZE);
        }

        // The invader: march sideways, dropping a row at each wall
        if (!gameOver)
        {
            invaderX += invaderSpeedX * delta;

            if (invaderX + INVADER_SIZE > WINDOW_W)
            {
                invaderX = WINDOW_W - INVADER_SIZE;   // snap back inside,
                invaderY += INVADER_SIZE;             // drop a row,
                invaderSpeedX = -INVADER_SPEED;       // and head left
            }
            else if (invaderX < 0.0f)
            {
                invaderX = 0.0f;
                invaderY += INVADER_SIZE;
                invaderSpeedX = INVADER_SPEED;        // head right
            }

            // Has it reached the player's row? Then a life is lost
            if (invaderY + INVADER_SIZE >= playerY)
            {
                lives--;
                SDL_Log("The invader got through! Lives left: %d", lives);

                invaderX = 0.0f;                      // back to the start
                invaderY = EDGE_GAP;
                invaderSpeedX = INVADER_SPEED;

                gameOver = (lives <= 0);
                if (gameOver)
                    SDL_Log("Game over! Final score: %d", score);
            }
        }

        // The bullet: fly up, and vanish off the top or on a hit
        if (bulletActive && !gameOver)
        {
            bulletY -= BULLET_SPEED * delta;

            if (bulletY + BULLET_SIZE < 0.0f)
                bulletActive = false;                 // gone off the top

            // Do the bullet and the invader overlap on both axes?
            bool overlapX = (bulletX < invaderX + INVADER_SIZE) &&
                            (bulletX + BULLET_SIZE > invaderX);
            bool overlapY = (bulletY < invaderY + INVADER_SIZE) &&
                            (bulletY + BULLET_SIZE > invaderY);

            if (overlapX && overlapY)
            {
                score++;
                SDL_Log("Hit! Score: %d", score);
                bulletActive = false;

                invaderX = 0.0f;                      // back to the start
                invaderY = EDGE_GAP;
                invaderSpeedX = INVADER_SPEED;
            }
        }

        // Draw the frame, on a dark red background once the game is over
        SDL_Color backgroundColor = gameOver ? GAME_OVER_BG : BACKGROUND;
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g,
                               backgroundColor.b, backgroundColor.a);
        SDL_RenderClear(renderer);

        // The player, dimmed once the game is over
        SDL_Color playerColor = gameOver ? PLAYER_DIM : PLAYER_COLOR;
        SDL_SetRenderDrawColor(renderer, playerColor.r, playerColor.g,
                               playerColor.b, playerColor.a);
        SDL_FRect playerRect = { playerX, playerY, PLAYER_SIZE, PLAYER_SIZE };
        SDL_RenderFillRect(renderer, &playerRect);

        // The invader
        SDL_SetRenderDrawColor(renderer, INVADER_COLOR.r, INVADER_COLOR.g,
                               INVADER_COLOR.b, INVADER_COLOR.a);
        SDL_FRect invaderRect = { invaderX, invaderY,
                                  INVADER_SIZE, INVADER_SIZE };
        SDL_RenderFillRect(renderer, &invaderRect);

        // The bullet, only while it's flying
        if (bulletActive)
        {
            SDL_SetRenderDrawColor(renderer, BULLET_COLOR.r, BULLET_COLOR.g,
                                   BULLET_COLOR.b, BULLET_COLOR.a);
            SDL_FRect bulletRect = { bulletX, bulletY,
                                     BULLET_SIZE, BULLET_SIZE };
            SDL_RenderFillRect(renderer, &bulletRect);
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up, in the reverse order we created things
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
