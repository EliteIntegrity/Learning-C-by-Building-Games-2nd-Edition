/*
    Act 1 Capstone Shooter
    The Chapter 9 project from Learning C++ by Building Games

    Fly a green ship with W, A, S, and D, and fire lasers with Space.
    Red aliens stream in from the right at random heights and speeds.
    Shooting one scores ten points, and letting one hit you costs one
    of your three lives. Lose them all, and the game is over: press R
    to play again, or Escape to quit.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int   WINDOW_W       = 1024;     // window width in pixels
const int   WINDOW_H       = 600;      // window height in pixels
const float PLAYER_W       = 64.0f;    // the ship's width
const float PLAYER_H       = 48.0f;    // the ship's height
const float PLAYER_START_X = 40.0f;    // where the ship starts, across
const float PLAYER_SPEED   = 300.0f;   // pixels per second
const float LASER_W        = 24.0f;    // a laser's length
const float LASER_H        = 4.0f;     // a laser's thickness
const float LASER_SPEED    = 720.0f;   // pixels per second, to the right
const float ALIEN_SIZE     = 48.0f;    // an alien's width and height
const float ALIEN_MIN_SPD  = 120.0f;   // the slowest alien, pixels per second
const float ALIEN_MAX_SPD  = 300.0f;   // the fastest alien
const float SPAWN_SPREAD   = 200.0f;   // how far past the edge aliens start
const float LIFE_SIZE      = 12.0f;    // each square in the lives display
const float LIFE_GAP       = 8.0f;     // the space around those squares
const int   MAX_LASERS     = 3;        // lasers that can fly at once
const int   MAX_ALIENS     = 12;       // aliens that can be in play at once
const int   START_LIVES    = 3;        // lives at the start of a game
const int   POINTS_PER_HIT = 10;       // the score for each alien shot

const SDL_Color BACKGROUND   = { 10, 10, 30, 255 };     // deep blue
const SDL_Color GAME_OVER_BG = { 60, 0, 0, 255 };       // dark red
const SDL_Color PLAYER_COLOR = { 0, 200, 0, 255 };      // green
const SDL_Color PLAYER_DIM   = { 0, 80, 0, 255 };       // dim green
const SDL_Color LASER_COLOR  = { 120, 255, 120, 255 };  // bright green
const SDL_Color ALIEN_COLOR  = { 220, 60, 60, 255 };    // red

// The player's ship: where it is, and how many lives it has left
struct Player
{
    SDL_FRect rect;
    int lives;
};

// A laser: where it is, and whether it's flying
struct Laser
{
    SDL_FRect rect;
    bool active;
};

// An alien: where it is, how fast it's coming, and whether it's in play
struct Alien
{
    SDL_FRect rect;
    float speed;
    bool active;
};

// Everything the game needs to remember, in one place
struct Game
{
    Player player;
    Laser lasers[MAX_LASERS];
    Alien aliens[MAX_ALIENS];
    int score;
    int highScore;
    bool gameOver;
};

// Put everything back the way it is at the start of a game
void resetGame(Game& game)
{
    game.player.rect = { PLAYER_START_X, (WINDOW_H - PLAYER_H) / 2.0f,
                         PLAYER_W, PLAYER_H };
    game.player.lives = START_LIVES;

    for (int i = 0; i < MAX_LASERS; i++)
        game.lasers[i].active = false;
    for (int i = 0; i < MAX_ALIENS; i++)
        game.aliens[i].active = false;

    game.score = 0;
    game.gameOver = false;
}

// Fill a rectangle with a color
void drawRect(SDL_Renderer* renderer, const SDL_FRect& rect,
              const SDL_Color& color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

// Move the ship with W, A, S, and D, keeping it inside the window
void updatePlayer(Player& player, float delta)
{
    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_W])
        player.rect.y -= PLAYER_SPEED * delta;
    if (keys[SDL_SCANCODE_S])
        player.rect.y += PLAYER_SPEED * delta;
    if (keys[SDL_SCANCODE_A])
        player.rect.x -= PLAYER_SPEED * delta;
    if (keys[SDL_SCANCODE_D])
        player.rect.x += PLAYER_SPEED * delta;

    player.rect.x = SDL_clamp(player.rect.x, 0.0f, WINDOW_W - PLAYER_W);
    player.rect.y = SDL_clamp(player.rect.y, 0.0f, WINDOW_H - PLAYER_H);
}

// A random number from low up to high
float randomBetween(float low, float high)
{
    return low + SDL_randf() * (high - low);
}

// Send an alien in from past the right edge, at a random height and speed
void spawnAlien(Alien& alien)
{
    alien.rect = { WINDOW_W + randomBetween(0.0f, SPAWN_SPREAD),
                   randomBetween(0.0f, WINDOW_H - ALIEN_SIZE),
                   ALIEN_SIZE, ALIEN_SIZE };
    alien.speed = randomBetween(ALIEN_MIN_SPD, ALIEN_MAX_SPD);
    alien.active = true;
}

// Keep every alien slot busy, and move the aliens to the left
void updateAliens(Game& game, float delta)
{
    for (int i = 0; i < MAX_ALIENS; i++)
    {
        Alien& alien = game.aliens[i];
        if (!alien.active)
            spawnAlien(alien);

        alien.rect.x -= alien.speed * delta;
        if (alien.rect.x + alien.rect.w < 0.0f)
            alien.active = false;   // it got past: free the slot
    }
}

// Fire a laser from the ship's nose, if a laser is free
void fireLaser(Game& game)
{
    const SDL_FRect& ship = game.player.rect;

    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (!game.lasers[i].active)
        {
            game.lasers[i].rect = { ship.x + ship.w,
                                    ship.y + (ship.h - LASER_H) / 2.0f,
                                    LASER_W, LASER_H };
            game.lasers[i].active = true;
            return;   // one laser per press
        }
    }
}

// Fly each laser to the right, and free any that leave the window
void updateLasers(Game& game, float delta)
{
    for (int i = 0; i < MAX_LASERS; i++)
    {
        Laser& laser = game.lasers[i];
        if (!laser.active)
            continue;

        laser.rect.x += LASER_SPEED * delta;
        if (laser.rect.x > WINDOW_W)
            laser.active = false;
    }
}

// Chapter 5's test: do two rectangles overlap on both axes?
bool rectsOverlap(const SDL_FRect& a, const SDL_FRect& b)
{
    bool overlapX = (a.x < b.x + b.w) && (a.x + a.w > b.x);
    bool overlapY = (a.y < b.y + b.h) && (a.y + a.h > b.y);
    return overlapX && overlapY;
}

// Did a laser hit an alien? Score it, and free them both
void checkLaserHits(Game& game)
{
    for (int i = 0; i < MAX_LASERS; i++)
    {
        Laser& laser = game.lasers[i];
        if (!laser.active)
            continue;

        for (int j = 0; j < MAX_ALIENS; j++)
        {
            Alien& alien = game.aliens[j];
            if (alien.active && rectsOverlap(laser.rect, alien.rect))
            {
                alien.active = false;
                laser.active = false;
                game.score += POINTS_PER_HIT;
                SDL_Log("Hit! Score: %d", game.score);
                break;   // this laser is used up
            }
        }
    }
}

// Did an alien hit the ship? Lose a life, and maybe the game
void checkPlayerHits(Game& game)
{
    for (int i = 0; i < MAX_ALIENS; i++)
    {
        Alien& alien = game.aliens[i];
        if (alien.active && rectsOverlap(game.player.rect, alien.rect))
        {
            alien.active = false;
            game.player.lives--;
            SDL_Log("Ouch! Lives left: %d", game.player.lives);

            if (game.player.lives <= 0)
            {
                game.gameOver = true;
                if (game.score > game.highScore)
                    game.highScore = game.score;
                SDL_Log("Game over! Score: %d, best: %d. Press R to restart.",
                        game.score, game.highScore);
                return;
            }
        }
    }
}

// Draw the frame: the background, then the aliens, lasers, ship, and lives
void drawGame(SDL_Renderer* renderer, const Game& game)
{
    SDL_Color backgroundColor = game.gameOver ? GAME_OVER_BG : BACKGROUND;
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g,
                           backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

    for (int i = 0; i < MAX_ALIENS; i++)
    {
        if (game.aliens[i].active)
            drawRect(renderer, game.aliens[i].rect, ALIEN_COLOR);
    }

    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (game.lasers[i].active)
            drawRect(renderer, game.lasers[i].rect, LASER_COLOR);
    }

    SDL_Color shipColor = game.gameOver ? PLAYER_DIM : PLAYER_COLOR;
    drawRect(renderer, game.player.rect, shipColor);

    // One small square for each life left, in the top-left corner
    for (int i = 0; i < game.player.lives; i++)
    {
        SDL_FRect life = { LIFE_GAP + i * (LIFE_SIZE + LIFE_GAP), LIFE_GAP,
                           LIFE_SIZE, LIFE_SIZE };
        drawRect(renderer, life, PLAYER_COLOR);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Act 1 Shooter",
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

    // The whole game: the {} starts every member at zero
    Game game{};
    resetGame(game);

    // The time at the last frame, in milliseconds
    Uint64 lastTime = SDL_GetTicks();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        // Events: the window's X, and key presses (not held-key repeats)
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
            {
                if (event.key.key == SDLK_ESCAPE)
                    running = false;
                else if (event.key.key == SDLK_SPACE && !game.gameOver)
                    fireLaser(game);
                else if (event.key.key == SDLK_R && game.gameOver)
                    resetGame(game);
            }
        }

        // Delta time: how many seconds the last frame took
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;

        // Update the game, unless it's over
        if (!game.gameOver)
        {
            updatePlayer(game.player, delta);
            updateAliens(game, delta);
            updateLasers(game, delta);
            checkLaserHits(game);
            checkPlayerHits(game);
        }

        drawGame(renderer, game);
    }

    // Clean up, in the reverse order we created things
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
